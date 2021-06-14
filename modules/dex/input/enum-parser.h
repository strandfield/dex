// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_ENUMPARSER_H
#define DEX_INPUT_ENUMPARSER_H

#include "dex/input/cpp-parser.h"

#include "dex/model/program.h"

#include <cpptok/token.h>

#include <string_view>
#include <vector>

namespace dex
{

class DEX_INPUT_API EnumParser
{
public:
  explicit EnumParser(std::shared_ptr<Enum> e);

  void parse(const std::string& source);

protected:
  static std::vector<cpptok::Token> tokenize(const std::vector<std::string_view>& lines);
  static void removeComments(std::vector<cpptok::Token>& toks);
  static std::vector<std::string> parseValues(const std::vector<cpptok::Token>& tokens);
  
private:
  std::shared_ptr<Enum> m_enum;
};

} // namespace dex

#endif // DEX_INPUT_ENUMPARSER_H
