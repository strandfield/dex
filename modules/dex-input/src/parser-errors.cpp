// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-errors.h"

#include "dex/common/logging.h"

namespace dex
{

ParserException::ParserException(std::string str)
  : Exception(std::move(str))
{

}

const std::string& ParserException::filepath() const noexcept
{
  return filepath_;
}

int ParserException::line() const noexcept
{
  return line_;
}

int ParserException::column() const noexcept
{
  return column_;
}

void ParserException::setSourceLocation(std::string path, int line, int col)
{
  filepath_ = std::move(path);
  line_ = line;
  column_ = col;

  what_ = path + ":" + std::to_string(line) + ":" + std::to_string(col) + ": " + what_;
}

UnexpectedControlSequence::UnexpectedControlSequence(std::string cs)
  : ParserException("unexpected control sequence '" + cs + "'"),
    csname(std::move(cs))
{

}

UnknownControlSequence::UnknownControlSequence(std::string cs)
  : ParserException("unknown control sequence '" + cs + "'"),
    csname(std::move(cs))
{

}

ExpectedControlSequence::ExpectedControlSequence(std::string prev_cs)
  : ParserException("expected control sequence after '" + prev_cs + "'"),
    prev_csname(std::move(prev_cs))
{

}

BadControlSequence::BadControlSequence(std::string cs)
  : ParserException("bad control sequence '" + cs + "'"),
    csname(std::move(cs))
{

}

Logger& operator<<(Logger& logger, const ParserException& ex)
{
  json::Object obj;

  obj["type"] = "ParserException";
  obj["what"] = ex.what();

  logger << obj;

  return logger;
}

} // namespace dex
