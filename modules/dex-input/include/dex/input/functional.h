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

} // namespace dex

#endif // DEX_INPUT_FUNCTIONAL_H
