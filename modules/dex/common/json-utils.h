// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_JSONUTILS_H
#define DEX_COMMON_JSONUTILS_H

#include "dex/common/settings.h"

#include <json-toolkit/json.h>

#include <string>
#include <variant>
#include <vector>

namespace dex
{

DEX_COMMON_API json::Json make_json_value(const SettingsValue& v);
DEX_COMMON_API json::Object build_json(const SettingsMap& map);

class DEX_COMMON_API JsonPathAnnotator
{
public:

  void annotate(json::Object& obj);

protected:
  void visit(json::Json val);

private:
  std::string build_path() const;

private:
  std::vector<std::variant<size_t, std::string>> m_stack;
};

} // namespace dex

#endif // DEX_COMMON_JSONUTILS_H
