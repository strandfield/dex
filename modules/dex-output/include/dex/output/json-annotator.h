// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_PATHANNOTATOR_H
#define DEX_OUTPUT_JSON_PATHANNOTATOR_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <json-toolkit/json.h>

#include <variant>

namespace dex
{

class DEX_OUTPUT_API JsonPathAnnotator
{
public:

  static std::vector<std::variant<size_t, std::string>> parse(const std::string& path);

  static json::Json get(const Model::Path& path, const json::Json& val);

  void annotate(json::Object& obj);

protected:
  void visit(json::Json val);

private:
  std::string build_path() const;

private:
  std::vector<std::variant<size_t, std::string>> m_stack;
};

} // namespace dex

namespace dex
{

inline json::Json JsonPathAnnotator::get(const Model::Path& path, const json::Json& val)
{
  auto result = val;

  for (const auto& p : path)
  {
    if (p.index != std::numeric_limits<size_t>::max())
      result = result[p.name][static_cast<int>(p.index)];
    else
      result = result[p.name];
  }

  return result;
}

} // namespace dex

#endif // DEX_OUTPUT_JSON_PATHANNOTATOR_H
