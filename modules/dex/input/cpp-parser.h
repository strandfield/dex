// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_CPPPARSER_H
#define DEX_INPUT_CPPPARSER_H

#include "dex/dex-input.h"

#include "dex/model/program.h"

#include <cpptok/token.h>

#include <stdexcept>
#include <vector>

namespace dex
{

class CppParserError : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;

  explicit CppParserError(const std::string& text)
    : std::runtime_error(text.data())
  {

  }
};

class DEX_INPUT_API CppParser
{
public:

  static dex::Type parseType(const std::string& str);
  static std::shared_ptr<Function> parseFunctionSignature(const std::string& str);
  static std::shared_ptr<Variable> parseVariable(const std::string& str);
  static std::shared_ptr<Typedef> parseTypedef(const std::string& str);
  static std::shared_ptr<Macro> parseMacro(const std::string& str);

protected:
  CppParser(const std::string* src);
  ~CppParser() = default;

  Type parseType();
  Type tryReadFunctionSignature(size_t start);

  Name parseName();

  std::shared_ptr<Function> parseFunctionSignature();

  std::shared_ptr<Variable> parseVariable();

  std::shared_ptr<Typedef> parseTypedef();

  std::shared_ptr<Macro> parseMacro();

protected:
  bool atEnd() const;
  cpptok::Token read();
  cpptok::Token unsafe_read();
  cpptok::Token peek();
  cpptok::Token unsafe_peek() const;
  bool isDiscardable(const cpptok::Token& t) const;
  cpptok::Token read(cpptok::TokenType::Value tokt);
  size_t pos() const;
  void seek(size_t pos);
  void seekBegin();
  void seekEnd();

  std::string tostring(size_t begin, size_t end) const;
  std::string viewstring() const;
  std::string stringtoend() const;

protected:
  Name readOperatorName();
  Name readUserDefinedName();
  Name readTemplateArguments(const Name base);

protected:
  TemplateArgument parseDelimitedTemplateArgument();
  std::shared_ptr<TemplateParameter> parseDelimitedTemplateParameter();

protected:
  std::shared_ptr<Function::Parameter> parseFunctionParameter();

private:
  std::vector<cpptok::Token> m_buffer;
  std::pair<size_t, size_t> m_view;
  size_t m_index = 0;
};

} // namespace dex

#endif // DEX_INPUT_CPPPARSER_H
