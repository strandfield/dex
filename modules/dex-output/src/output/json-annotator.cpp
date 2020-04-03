// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-annotator.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

namespace dex
{

void JsonAnnotator::annotate(const Model& model, json::Object& obj)
{
  visit(model, obj);
}

void JsonPathAnnotator::visit_domnode(const dom::Node& n, json::Object& obj)
{
  obj["_path"] = build_path();
  JsonAnnotator::visit_domnode(n, obj);
}

void JsonPathAnnotator::visit_entity(const cxx::Entity& e, json::Object& obj)
{
  obj["_path"] = build_path();

  JsonAnnotator::visit_entity(e, obj);
}

inline static std::string next_token(const std::string& path, size_t index)
{
  auto is_delim = [](char c) { return c == '.' || c == '[' || c == ']';  };

  const size_t start = index;

  while (index < path.size() && !is_delim(path.at(index))) ++index;

  return std::string(path.begin() + start, path.begin() + index);
}

std::vector<std::variant<size_t, std::string>> JsonPathAnnotator::parse(const std::string& path)
{
  assert(!path.empty());

  std::vector<std::variant<size_t, std::string>> result;

  if (path == "$")
    return result;

  size_t index = 2;
  auto is_num = [](char c) { return c >= '0' && c <= '9'; };

  do
  {
    std::string token = next_token(path, index);

    index += token.size() + 1;

    if (!token.empty())
    {
      if (is_num(token.front()))
        result.push_back(static_cast<size_t>(std::stoi(token)));
      else
        result.push_back(std::move(token));
    }
  } while (index < path.size());

  return result;
}

std::string JsonPathAnnotator::build_path() const
{
  if (stack().empty())
    return "$";

  std::string result = "$";

  for (size_t i(0); i < stack().size(); ++i)
  {
    if (std::holds_alternative<size_t>(stack().at(i)))
    {
      size_t index = std::get<size_t>(stack().at(i));
      result += "[" + std::to_string(index) + "]";
    }
    else
    {
      const std::string& text = std::get<std::string>(stack().at(i));
      result += "." + text;
    }
  }

  return result;
}

} // namespace dex
