// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_MACHINE_H
#define DEX_INPUT_PARSER_MACHINE_H

#include "dex/dex-input.h"

#include "dex/model/model.h"

#include "dex/input/conditional-evaluator.h"
#include "dex/input/function-caller.h"

#include <cxx/program.h>

#include <tex/lexer.h>
#include <tex/parsing/registers.h>
#include <tex/parsing/preprocessor.h>

#include <QFileInfo>

#include <stack>
#include <string_view>

namespace dex
{

class DEX_INPUT_API InputStream
{
public:
  InputStream();
  InputStream(std::string doc);
  explicit InputStream(const QFileInfo& file);
  InputStream(const InputStream &) = default;

  void setBlockDelimiters(std::string start, std::string end);

  void inject(std::string content);
  void inject(const QFileInfo& file);

  char peekChar() const;
  inline char nextChar() const { return peekChar(); }
  char readChar();

  std::string_view peek(int n) const;
  std::string_view peekLine() const;

  bool read(const std::string_view& text);

  void discard(int n);

  bool seekBlock();
  bool isInsideBlock() const;
  bool atBlockEnd() const;
  void exitBlock();

  struct Document
  {
    int pos = 0;
    int line = 0;
    int column = 0;
    std::string content;
    std::string file_path;

    inline int length() const { return static_cast<int>(content.length()); }
  };

  Document & currentDocument();
  const Document & currentDocument() const;
  int currentPos() const;
  bool atEnd() const;

  void clear();

  inline int stackSize() const { return static_cast<int>(m_documents.size()); }

  InputStream& operator=(std::string str);
  InputStream& operator=(const QFileInfo& file);

protected:
  void beginLine();

private:
  std::stack<Document> m_documents;
  std::pair<std::string, std::string> m_block_delimiters;
  bool m_inside_block = false;
};

class ParserMode;

class DEX_INPUT_API ParserMachine
{
public:
  ParserMachine();
  ~ParserMachine();

  enum State
  {
    Idle,
    BeginFile,
    SeekBlock,
    ReadChar,
    ReadToken,
    Preprocess,
    SendToken,
    EndFile,
  };

  State state() const;

  void process(const QFileInfo& file);

  tex::parsing::Registers& registers();

  void input(const std::string& filename);
  InputStream& inputStream();

  void setBlockDelimiters(std::string start, std::string end);

  tex::parsing::Lexer& lexer();

  tex::parsing::Preprocessor& preprocessor();

  dex::FunctionCaller& caller();

  const std::vector<std::unique_ptr<ParserMode>>& modes() const;
  ParserMode& currentMode() const;

  void resume();
  void advance();

  const std::shared_ptr<cxx::Program>& output() const;

protected:

  void processFile(const std::string& path);

  void beginFile();
  void endFile();

  void beginBlock();
  void endBlock();

  bool seekBlock();
  bool atBlockEnd() const;

  bool sendTokens();

private:
  tex::parsing::Registers m_registers;
  std::stack<tex::parsing::Lexer::CatCodeTable> m_lexercatcodes;
  InputStream m_inputstream;
  tex::parsing::Lexer m_lexer;
  tex::parsing::Preprocessor m_preprocessor;
  dex::ConditionalEvaluator m_condeval;
  dex::FunctionCaller m_caller;
  std::vector<std::unique_ptr<ParserMode>> m_modes;
  State m_state = State::Idle;
  std::shared_ptr<cxx::Program> m_program;
};

} // namespace dex

#endif // DEX_INPUT_PARSER_MACHINE_H
