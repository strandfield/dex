// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_YAMLCONFIG_H
#define DEX_COMMON_YAMLCONFIG_H

#include "dex/dex-common.h"

#include <json-toolkit/json.h>

#include <filesystem>

namespace YAML
{
class Node;
} // namespace YAML

namespace dex
{

DEX_COMMON_API json::Json yaml_to_json(const YAML::Node& n);

DEX_COMMON_API json::Json parse_yaml_config(const std::filesystem::path& p);

namespace config
{

inline json::Json read(const json::Json& conf, const std::string& key, const json::Json& dval = json::null)
{
  if (!conf.isObject())
    return dval;

  const auto& map = conf.toObject().data();
  auto it = map.find(key);
  return it != map.end() ? it->second : dval;
}

} // namespace config

} // namespace dex

#endif // DEX_COMMON_YAMLCONFIG_H
