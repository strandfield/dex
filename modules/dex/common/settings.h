// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_SETTINGS_H
#define DEX_COMMON_SETTINGS_H

#include "dex/dex-common.h"

#include <filesystem>
#include <map>
#include <string>
#include <variant>

namespace dex
{

typedef std::variant<bool, int, double, std::string> SettingsValue;
typedef std::map<std::string, SettingsValue> SettingsMap;

namespace settings
{

DEX_COMMON_API SettingsMap load(const std::filesystem::path& p);

template<typename T>
T read(SettingsMap& settings, const std::string& key, T&& default_value)
{
  auto it = settings.find(key);

  if (it != settings.end())
  {
    T result = std::get<T>(it->second);
    settings.erase(it);
    return result;
  }
  else
  {
    return default_value;
  }
}

} // namespace settings

} // namespace dex

#endif // DEX_COMMON_SETTINGS_H
