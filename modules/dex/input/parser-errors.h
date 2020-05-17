// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_ERRORS_H
#define DEX_INPUT_PARSER_ERRORS_H

#include "dex/dex-input.h"

#include "dex/common/errors.h"

namespace dex
{

class DEX_INPUT_API ParserException : public Exception
{
private:
  std::string filepath_;
  int line_ = -1;
  int column_ = -1;

public:

  explicit ParserException(std::string str);

  const std::string& filepath() const noexcept;
  int line() const noexcept;
  int column() const noexcept;

  void setSourceLocation(std::string path, int line, int col);
};

class DEX_INPUT_API UnexpectedControlSequence : public ParserException
{
public:
  std::string csname;

  UnexpectedControlSequence(std::string cs);
};

class DEX_INPUT_API UnknownControlSequence : public ParserException
{
public:
  std::string csname;

  UnknownControlSequence(std::string cs);
};

class DEX_INPUT_API ExpectedControlSequence : public ParserException
{
public:
  std::string prev_csname;

  ExpectedControlSequence(std::string prev_cs);
};

class DEX_INPUT_API BadControlSequence : public ParserException
{
public:
  std::string csname;

  BadControlSequence(std::string cs);
};

class DEX_INPUT_API BadCall : public ParserException
{
public:
  std::string fnname;
  std::string message;

  BadCall(std::string fn, std::string mssg);
};

DEX_INPUT_API Logger& operator<<(Logger& logger, const ParserException& ex);

} // namespace dex

#endif // DEX_INPUT_PARSER_ERRORS_H
