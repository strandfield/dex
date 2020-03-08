// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-machine.h"

#include "dex/input/format.h"
#include "dex/input/parser-mode.h"
#include "dex/input/program-mode.h"
#include "dex/input/parser-errors.h"

#include "dex/common/file-utils.h"

#include <QDebug>
#include <QFile>

namespace dex
{

template<typename C>
void send_token(tex::parsing::Token&& tok, C& c)
{
  c.write(std::move(tok));
}

template<typename C, typename...Components, typename = std::enable_if_t<sizeof...(Components) != 0, void>>
void send_token(tex::parsing::Token&& tok, C& c, Components& ... rest)
{
  c.write(std::move(tok));

  if (!c.output().empty())
    send_token(tex::parsing::read(c.output()), rest...);
}

InputStream::InputStream()
  : m_block_delimiters{ "/*!", "*/" }
{

}

InputStream::InputStream(std::string doc)
  : m_block_delimiters{ "/*!", "*/" }
{
  Document document;
  document.content = std::move(doc);
  document.pos = 0;
  m_documents.push(document);
}

InputStream::InputStream(const QFileInfo& file)
  : m_block_delimiters{ "/*!", "*/" }
{
  Document d;
  d.content = file_utils::read_all(file.absoluteFilePath().toStdString());
  d.file_path = file.filePath().toStdString();
  m_documents.push(d);
}

void InputStream::setBlockDelimiters(std::string start, std::string end)
{
  m_block_delimiters.first = std::move(start);
  m_block_delimiters.second = std::move(end);
}

void InputStream::inject(std::string content)
{
  Document d;
  d.content = std::move(content);
  m_documents.push(d);
}

void InputStream::inject(const QFileInfo& file)
{
  Document d;
  d.content = file_utils::read_all(file.absoluteFilePath().toStdString());
  d.file_path = file.filePath().toStdString();
  m_documents.push(d);
}

char InputStream::peekChar() const
{
  return currentDocument().content[currentPos()];
}

char InputStream::readChar()
{
  int& pos = currentDocument().pos;
  int& line = currentDocument().line;
  int& col = currentDocument().column;

  char result = currentDocument().content[pos++];
  col++;

  if (result == '\n')
  {
    col = 0;
    line++;

    if (stackSize() == 1 && pos < currentDocument().length())
      beginLine();
  }

  if (pos == currentDocument().length() && stackSize() > 1)
  {
    m_documents.pop();
  }

  return result;
}

std::string_view InputStream::peek(int n) const
{
  auto & doc = currentDocument();
  return std::string_view(doc.content.data() + doc.pos, n);
}

std::string_view InputStream::peekLine() const
{
  const size_t index = currentDocument().content.find('\n', static_cast<size_t>(currentDocument().pos));

  if (index == std::string::npos)
    return peek(currentDocument().length() - currentPos());
  else
    return peek(static_cast<int>(index - currentPos()));
}

bool InputStream::read(const std::string_view& text)
{
  if (peek(static_cast<int>(text.size())) != text)
    return false;

  discard(static_cast<int>(text.size()));

  return true;
}

void InputStream::discard(int n)
{
  while (n > 0)
    readChar(), --n;
}

bool InputStream::seekBlock()
{
  while (!atEnd() && !read(m_block_delimiters.first))
    readChar();

  m_inside_block = !atEnd();
  return isInsideBlock();
}

bool InputStream::isInsideBlock() const
{
  return m_inside_block;
}

bool InputStream::atBlockEnd() const
{
  return peek(static_cast<int>(m_block_delimiters.second.length())) == m_block_delimiters.second;
}

void InputStream::seekBlockEnd()
{
  while (!atEnd() && !atBlockEnd())
    readChar();
}

void InputStream::exitBlock()
{
  if (atBlockEnd())
  {
    discard(static_cast<int>(m_block_delimiters.second.length()));
    m_inside_block = false;
  }
}

void InputStream::beginLine()
{
  if (stackSize() > 1)
    return;

  auto is_space = [](char c) -> bool {
    return c == ' ' || c == '\t' || c == '\r';
  };

  auto line = peekLine();

  size_t n = 0;

  while (n < line.size() && is_space(line.at(n)))
    ++n;

  if (n == line.size() || line.at(n) != '*')
    return;

  size_t blockend = line.find(m_block_delimiters.second, n);

  if (blockend != std::string_view::npos)
  {
    discard(static_cast<int>(blockend));
  }
  else
  {
    discard(static_cast<int>(n+1));
  }
}

InputStream::Document & InputStream::currentDocument()
{
  return m_documents.top();
}

const InputStream::Document & InputStream::currentDocument() const
{
  return m_documents.top();
}

int InputStream::currentPos() const
{
  return currentDocument().pos;
}

bool InputStream::atEnd() const
{
  return m_documents.empty() ||
    (m_documents.size() == 1 && currentDocument().pos == currentDocument().content.length());
}

void InputStream::clear()
{
  m_documents = std::stack<Document>();
}

InputStream& InputStream::operator=(std::string str)
{
  m_documents = std::stack<Document>();

  Document document;
  document.content = std::move(str);
  m_documents.push(document);

  return *this;
}

InputStream& InputStream::operator=(const QFileInfo& file)
{
  m_documents = std::stack<Document>();

  Document document;
  document.content = file_utils::read_all(file.absoluteFilePath().toStdString());
  document.file_path = file.filePath().toStdString();
  m_documents.push(document);

  return *this;
}

ParserMachine::ParserMachine()
  : m_registers{}, 
    m_lexercatcodes{},
    m_inputstream {},
    m_lexer{},
    m_preprocessor{m_registers},
    m_condeval{*this},
    m_caller{*this},
    m_modes{},
    m_state{State::Idle},
    m_model{}
{
#if defined(Q_OS_WIN)
  m_lexer.catcodes()[static_cast<size_t>('\r')] = tex::parsing::CharCategory::Ignored;
#endif // defined(Q_OS_WIN)

  m_model = std::make_shared<Model>();

  for (const tex::parsing::Macro& m : DexFormat.macros())
  {
    m_preprocessor.define(m);
  }

  m_modes.push_back(std::make_unique<ProgramMode>(*this));
}

ParserMachine::~ParserMachine()
{

}

ParserMachine::State ParserMachine::state() const
{
  return m_state;
}

void ParserMachine::process(const QFileInfo& file)
{
  processFile(file.absoluteFilePath().toStdString());
}

tex::parsing::Registers& ParserMachine::registers()
{
  return m_registers;
}

void ParserMachine::input(const std::string& filename)
{
  QString qt_filename = QString::fromStdString(filename);

  QFileInfo file{ qt_filename };

  if (!file.exists())
  {
    qDebug() << "Could not find input file " << qt_filename;
    return;
  }

  m_inputstream.inject(file);
}

InputStream& ParserMachine::inputStream()
{
  return m_inputstream;
}

void ParserMachine::setBlockDelimiters(std::string start, std::string end)
{
  m_inputstream.setBlockDelimiters(std::move(start), std::move(end));
}

tex::parsing::Lexer& ParserMachine::lexer()
{
  return m_lexer;
}

tex::parsing::Preprocessor& ParserMachine::preprocessor()
{
  return m_preprocessor;
}

dex::FunctionCaller& ParserMachine::caller()
{
  return m_caller;
}

const std::vector<std::unique_ptr<ParserMode>>& ParserMachine::modes() const
{
  return m_modes;
}

ParserMode& ParserMachine::currentMode() const
{
  return *m_modes.back();
}

bool ParserMachine::sendTokens()
{
  if (m_preprocessor.output().empty())
    return false;

  tex::parsing::Token t = tex::parsing::read(m_preprocessor.output());

  send_token(std::move(t), m_condeval, m_caller);

  if (m_caller.output().empty())
    return !m_preprocessor.output().empty();

  m_modes.back()->write(tex::parsing::read(m_caller.output()));

  if (m_modes.back()->done())
  {
    std::unique_ptr<ParserMode> mode{ std::move(m_modes.back()) };
    m_modes.pop_back();
    m_modes.back()->childFinished(*mode);
  }

  return !m_preprocessor.output().empty();
}

void ParserMachine::resume()
{
  while (state() != ParserMachine::Idle)
  {
    advance();
  }
}

void ParserMachine::advance()
{
  try
  {
    switch (state())
    {
    case State::BeginFile:
    {
      beginFile();
      m_state = State::SeekBlock;
    }
    break;
    case State::SeekBlock:
    {
      if (inputStream().seekBlock())
      {
        beginBlock();
        m_state = State::ReadChar;
      }
      else
      {
        m_state = State::EndFile;
      }
    }
    break;
    case State::ReadChar:
    {
      if (inputStream().atBlockEnd())
      {
        inputStream().exitBlock();
        endBlock();
        m_state = State::SeekBlock;
      }
      else
      {
        m_lexer.write(inputStream().readChar());
        m_state = m_lexer.output().empty() ? State::ReadChar : State::ReadToken;
      }
    }
    break;
    case State::ReadToken:
    {
      if (!m_lexer.output().empty())
      {
        m_preprocessor.write(tex::parsing::read(m_lexer.output()));
        m_state = State::SendToken;
      }
      else
      {
        m_state = State::ReadChar;
      }
    }
    break;
    case State::Preprocess:
    {
      m_preprocessor.advance();

      if (!m_preprocessor.output().empty())
        m_state = State::SendToken;
      else if (!m_preprocessor.input().empty())
        m_state = State::Preprocess;
      else
        m_state = State::ReadChar;
    }
    break;
    case State::SendToken:
    {
      while (sendTokens());

      if (!m_preprocessor.input().empty())
        m_state = State::Preprocess;
      else
        m_state = State::ReadToken;
    }
    break;
    case State::EndFile:
    {
      endFile();
      m_inputstream.clear();
      m_state = State::Idle;
    }
    break;
    default:
      break;
    }
  }
  catch (ParserException& ex)
  {
    const InputStream::Document& doc = m_inputstream.currentDocument();
    ex.setSourceLocation(doc.file_path, doc.line, doc.column);
    throw;
  }
}

bool ParserMachine::recover()
{
  /* TODO: robustify this function */

  if (inputStream().isInsideBlock())
  {
    inputStream().seekBlockEnd();

    if (inputStream().atEnd())
      return false;

    inputStream().exitBlock();

    currentMode().endBlock();
  }

  m_state = State::SeekBlock;

  m_lexer.output().clear();
  m_preprocessor.input().clear();
  m_preprocessor.output().clear();
  m_condeval.output().clear();
  m_caller.output().clear();

  while (m_modes.size() > 1)
  {
    m_modes.pop_back();
  }

  return true;
}

void ParserMachine::reset()
{
  /* TODO: robustify this function */

  if (inputStream().isInsideBlock())
  {
    currentMode().endBlock();
    currentMode().endFile();
  }

  m_state = State::Idle;

  m_inputstream.clear();
  m_lexer.output().clear();
  m_preprocessor.input().clear();
  m_preprocessor.output().clear();
  m_condeval.output().clear();
  m_caller.output().clear();

  m_modes.clear();

  m_modes.push_back(std::make_unique<ProgramMode>(*this));
}

const std::shared_ptr<Model>& ParserMachine::output() const
{
  return m_model;
}

void ParserMachine::processFile(const std::string& path)
{
  QString qt_path = QString::fromStdString(path);

  m_inputstream = QFileInfo{ qt_path };

  m_state = State::BeginFile;
  resume();
}

void ParserMachine::beginFile()
{
  currentMode().beginFile();
}

void ParserMachine::endFile()
{
  currentMode().endFile();
}

void ParserMachine::beginBlock()
{
  currentMode().beginBlock();
}

void ParserMachine::endBlock()
{
  currentMode().endBlock();
}

bool ParserMachine::seekBlock()
{
  return m_inputstream.seekBlock();
}

bool ParserMachine::atBlockEnd() const
{
  return m_inputstream.atBlockEnd();
}

} // namespace dex
