// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_FUNCTIONAL_H
#define DEX_INPUT_FUNCTIONAL_H

#include "dex/dex-input.h"

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace dex
{

typedef std::variant<bool, int, double, std::string> Argument;
typedef std::map<std::string, Argument> Options;

struct FunctionCall
{
  std::string function;
  std::vector<Argument> arguments;
  Options options;
};

struct Functions
{
  static const std::string PAR;
  static const std::string CLASS;
  static const std::string ENDCLASS;
  static const std::string FUNCTION;
  static const std::string ENDFN;
  static const std::string NAMESPACE;
  static const std::string ENDNAMESPACE;
  static const std::string BRIEF;
  static const std::string SINCE;
  static const std::string BEGINSINCE;
  static const std::string ENDSINCE;
  static const std::string PARAM;
  static const std::string RETURNS;
};

} // namespace dex

#endif // DEX_INPUT_FUNCTIONAL_H
