// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_MACHINE_H
#define DEX_INPUT_PARSER_MACHINE_H

#include "dex/dex-input.h"

#include "dex/model/model.h"

#include "dex/input/conditional-evaluator.h"
#include "dex/input/function-caller.h"
#include "dex/input/parser-frontend.h"
#include "dex/input/parser-errors.h"

#include <cxx/program.h>

#include <tex/lexer.h>
#include <tex/parsing/preprocessor.h>

#include <QFileInfo>

#include <stack>
#include <string_view>

namespace dex
{

class DEX_INPUT_API BlockBasedDocument
{
public:
  std::pair<std::string, std::string> block_delimiters;
  std::string filepath;
  std::string content;

public:
  explicit BlockBasedDocument(std::string text, std::string path = "");
};

class DEX_INPUT_API InputStream
{
public:
  InputStream();
  InputStream(std::string doc);
  InputStream(BlockBasedDocument doc);
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

  bool isBlockBased() const;

  bool seekBlock();
  bool isInsideBlock() const;
  bool atBlockEnd() const;
  void seekBlockEnd();
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
  bool m_is_block_based = false;
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

  void input(const std::string& filename);
  InputStream& inputStream();

  void setBlockDelimiters(std::string start, std::string end);

  tex::parsing::Lexer& lexer();

  tex::parsing::Preprocessor& preprocessor();

  dex::FunctionCall& call();
  dex::FunctionCaller& caller();

  void resume();
  void advance();

  bool recover();

  void reset();

  const std::shared_ptr<Model>& output() const;

protected:

  void processFile(const std::string& path);

  void beginFile();
  void endFile();

  void beginBlock();
  void endBlock();

  bool seekBlock();
  bool atBlockEnd() const;

  bool sendTokens();

  void interpret(tex::parsing::Token tok);

  void beginGroup();
  void endGroup();

private:
  std::shared_ptr<Model> m_model;
  dex::FunctionCall m_call;
  std::stack<tex::parsing::Lexer::CatCodeTable> m_lexercatcodes;
  InputStream m_inputstream;
  tex::parsing::Lexer m_lexer;
  tex::parsing::Preprocessor m_preprocessor;
  dex::ConditionalEvaluator m_condeval;
  dex::FunctionCaller m_caller;
  ParserFrontend m_processor;
  State m_state = State::Idle;
};

} // namespace dex

namespace dex
{

inline dex::FunctionCall& ParserMachine::call()
{
  return m_call;
}

} // namespace dex

#endif // DEX_INPUT_PARSER_MACHINE_H
