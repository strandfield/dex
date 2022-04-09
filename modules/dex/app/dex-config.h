// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_CONFIG_H
#define DEX_APP_CONFIG_H

#include "dex/dex-app.h"

#include <json-toolkit/json.h>

#include <filesystem>
#include <set>
#include <string>

namespace dex
{

struct Config
{
  bool valid = false;
  std::set<std::string> inputs;
  std::set<std::string> suffixes;
  std::string output;
  json::Object variables;
};

DEX_APP_API Config parse_config(const std::filesystem::path& file);
DEX_APP_API Config parse_config();

} // namespace dex

#endif // DEX_APP_CONFIG_H
