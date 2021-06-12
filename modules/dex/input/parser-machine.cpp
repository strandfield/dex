// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-machine.h"

#include "dex/input/format.h"
#include "dex/input/parser-errors.h"

#include "dex/common/file-utils.h"

#include <QDir>
#include <QFile>

#include <QDebug>

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

  while (!c.output().empty())
    send_token(tex::parsing::read(c.output()), rest...);
}

inline bool is_space(char c)
{
  return c == ' ' || c == '\t' || c == '\r';
}

BlockBasedDocument::BlockBasedDocument(std::string text, std::string path)
  : block_delimiters{ "/*!", "*/" },
    filepath(std::move(path)),
    content(std::move(text))
{

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

  m_is_block_based = false;
}

InputStream::InputStream(BlockBasedDocument doc)
  : m_block_delimiters{ std::move(doc.block_delimiters) }
{
  Document document;
  document.content = std::move(doc.content);
  document.file_path = std::move(doc.filepath);
  document.pos = 0;
  m_documents.push(document);

  m_is_block_based = true;
}

InputStream::InputStream(const QFileInfo& file)
  : m_block_delimiters{ "/*!", "*/" }
{
  Document d;
  d.content = file_utils::read_all(file.absoluteFilePath().toStdString());
  d.file_path = file.filePath().toStdString();
  m_documents.push(d);

  m_is_block_based = file.suffix() != "dex";
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
      beginLineInBlock();
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
    return peek(currentDocument().length() - 1 - currentPos());
  else
    return peek(static_cast<int>(index - currentPos()));
}

std::string_view InputStream::readLine()
{
  auto result = peekLine();
  discard(static_cast<int>(result.size()) + 1);
  return result;
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

bool InputStream::isBlockBased() const
{
  return m_is_block_based;
}

bool InputStream::seekBlock()
{
  assert(isBlockBased());

  // The previous code was:
  //    while (!atEnd() && !read(m_block_delimiters.first))
  //      readChar();
  // but it would read the delimiter in the middle of a string constant,
  // e.g. "/*!", which is not what we want.
  // We assume that a block is at the beginning of a line, minus the 
  // possible whitespaces before.

  while (!atEnd())
  {
    discardSpaces();

    if (read(m_block_delimiters.first))
    {
      m_block_pos.offset = currentDocument().pos - static_cast<int>(m_block_delimiters.first.size());
      m_block_pos.line = currentDocument().line;
      m_block_pos.column = currentDocument().column - static_cast<int>(m_block_delimiters.first.size());
      break;
    }

    readLine();
  }

  return isInsideBlock();
}

bool InputStream::isInsideBlock() const
{
  return m_block_pos.offset != -1;
}

InputStream::Position InputStream::blockPosition() const
{
  return m_block_pos;
}

bool InputStream::atBlockEnd() const
{
  return isInsideBlock() && peek(static_cast<int>(m_block_delimiters.second.length())) == m_block_delimiters.second;
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
    m_block_pos = Position();
  }
}

void InputStream::beginLineInBlock()
{
  if (stackSize() > 1 || !isBlockBased())
    return;

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

void InputStream::discardSpaces()
{
  while (!atEnd() && is_space(peekChar()))
    readChar();
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

  m_is_block_based = false;
  m_block_pos = Position();

  return *this;
}

InputStream& InputStream::operator=(const QFileInfo& file)
{
  m_documents = std::stack<Document>();

  Document document;
  document.content = file_utils::read_all(file.absoluteFilePath().toStdString());
  document.file_path = file.filePath().toStdString();
  m_documents.push(document);

  m_is_block_based = file.suffix() != "dex";
  m_block_pos = Position();

  return *this;
}

ParserMachine::ParserMachine()
  : m_model{new Model},
    m_lexercatcodes{},
    m_inputstream {},
    m_lexer{},
    m_preprocessor{},
    m_condeval{*this},
    m_caller{*this},
    m_processor{*this},
    m_state{State::Idle}
{
#if defined(Q_OS_WIN)
  m_lexer.catcodes()[static_cast<size_t>('\r')] = tex::parsing::CharCategory::Ignored;
#endif // defined(Q_OS_WIN)

  for (const tex::parsing::Macro& m : DexFormat::load())
  {
    m_preprocessor.define(m);
  }
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

void ParserMachine::input(const std::string& filename)
{
  QString qt_filename = QString::fromStdString(filename);

  QFileInfo file{ qt_filename };

  if (file.suffix().isEmpty())
    file.setFile(file.filePath() + ".dex");

  if (file.exists())
  {
    m_inputstream.inject(file);
    return;
  }

  QFileInfo current{ QString::fromStdString(m_inputstream.currentDocument().file_path) };

  file.setFile(current.dir().path() + "/" + qt_filename);

  if (file.suffix().isEmpty())
    file.setFile(file.filePath() + ".dex");

  if (!file.exists())
  {
    throw std::runtime_error{ "No such file" };
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

bool ParserMachine::sendTokens()
{
  if (m_preprocessor.output.empty())
    return false;

  tex::parsing::Token t = tex::parsing::read(m_preprocessor.output);

  send_token(std::move(t), m_condeval, m_caller);

  if (m_caller.hasPendingCall())
  {
    m_processor.handle(m_caller.call());
    m_caller.clearPendingCall();
  }

  if (m_caller.output().empty())
    return !m_preprocessor.output.empty();

  interpret(tex::parsing::read(m_caller.output()));


  return !m_preprocessor.output.empty();
}

void ParserMachine::interpret(tex::parsing::Token tok)
{
  if (tok.isCharacterToken())
  {
    switch (tok.characterToken().category)
    {
    case tex::parsing::CharCategory::GroupBegin:
      beginGroup();
      break;
    case tex::parsing::CharCategory::GroupEnd:
      endGroup();
      break;
    case tex::parsing::CharCategory::MathShift:
      m_processor.mathshift();
      break;
    case tex::parsing::CharCategory::AlignmentTab:
      m_processor.alignmenttab();
      break;
    case tex::parsing::CharCategory::Superscript:
      m_processor.superscript();
      break;
    case tex::parsing::CharCategory::Subscript:
      m_processor.subscript();
      break;
    case tex::parsing::CharCategory::Letter:
    case tex::parsing::CharCategory::Other:
      m_processor.write(tok.characterToken().value);
      break;
    case tex::parsing::CharCategory::Space:
      m_processor.write_space(tok.characterToken().value);
      break;
    case tex::parsing::CharCategory::Active:
      m_processor.write_active(tok.characterToken().value);
      break;
    case tex::parsing::CharCategory::Ignored:
    default:
      break;
    }
  }
  else
  {
    static FunctionCall simple_call = {};
    simple_call.function = tok.controlSequence();
    m_processor.handle(simple_call);
  }
}

void ParserMachine::beginGroup()
{
  m_preprocessor.beginGroup();
  m_processor.bgroup();
}

void ParserMachine::endGroup()
{
  m_processor.egroup();
  m_preprocessor.endGroup();
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
      m_state = inputStream().isBlockBased() ? State::SeekBlock : State::ReadChar;
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
      if (inputStream().isBlockBased())
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
      else
      {
        if (inputStream().atEnd())
        {
          m_state = State::EndFile;
        }
        else
        {
          m_lexer.write(inputStream().readChar());
          m_state = m_lexer.output().empty() ? State::ReadChar : State::ReadToken;
        }
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

      if (!m_preprocessor.output.empty())
        m_state = State::SendToken;
      else if (!m_preprocessor.input.empty())
        m_state = State::Preprocess;
      else
        m_state = State::ReadChar;
    }
    break;
    case State::SendToken:
    {
      while (sendTokens());

      if (!m_preprocessor.input.empty())
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

    m_processor.endBlock();
  }

  m_state = State::SeekBlock;

  m_lexer.output().clear();
  m_preprocessor.input.clear();
  m_preprocessor.output.clear();
  m_condeval.output().clear();
  m_caller.output().clear();
  m_processor.recover();

  return true;
}

void ParserMachine::reset()
{
  /* TODO: robustify this function */

  if (inputStream().isInsideBlock())
  {
    m_processor.endBlock();
    m_processor.endFile();
  }

  m_state = State::Idle;

  m_inputstream.clear();
  m_lexer.output().clear();
  m_preprocessor.input.clear();
  m_preprocessor.output.clear();
  m_condeval.output().clear();
  m_caller.output().clear();

  m_processor.reset();
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
  m_processor.beginFile();
}

void ParserMachine::endFile()
{
  m_processor.endFile();
}

void ParserMachine::beginBlock()
{
  m_processor.beginBlock();
}

void ParserMachine::endBlock()
{
  m_processor.endBlock();
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
