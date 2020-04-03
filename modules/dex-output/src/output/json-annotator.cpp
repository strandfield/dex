// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-annotator.h"

namespace dex
{

inline static std::string next_token(const std::string& path, size_t index)
{
  auto is_delim = [](char c) { return c == '.' || c == '[' || c == ']';  };

  const size_t start = index;

  while (index < path.size() && !is_delim(path.at(index))) ++index;

  return std::string(path.begin() + start, path.begin() + index);
}

struct RAIIJsonPathAnnotatorContext
{
  std::vector<std::variant<size_t, std::string>>* stack;

  RAIIJsonPathAnnotatorContext(std::vector<std::variant<size_t, std::string>>* s, size_t n)
    : stack(s)
  {
    stack->push_back(n);
  }

  RAIIJsonPathAnnotatorContext(std::vector<std::variant<size_t, std::string>>* s, std::string name)
    : stack(s)
  {
    stack->push_back(std::move(name));
  }

  ~RAIIJsonPathAnnotatorContext()
  {
    stack->pop_back();
  }
};

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

void JsonPathAnnotator::annotate(json::Object& obj)
{
  for (std::pair<const std::string, json::Json>& entry : obj.data())
  {
    RAIIJsonPathAnnotatorContext context{ &m_stack, entry.first };

    visit(entry.second);
  }
}

void JsonPathAnnotator::visit(json::Json val)
{
  if (val.isArray())
  {
    for (int i(0); i < val.length(); ++i)
    {
      RAIIJsonPathAnnotatorContext context{ &m_stack, static_cast<size_t>(i) };
      visit(val[i]);
    }
  }
  else if (val.isObject())
  {
    json::Object obj = val.toObject();
    obj["_path"] = build_path();

    for (std::pair<const std::string, json::Json>& entry : obj.data())
    {
      RAIIJsonPathAnnotatorContext context{ &m_stack, entry.first };
      visit(entry.second);
    }
  }
}

std::string JsonPathAnnotator::build_path() const
{
  if (m_stack.empty())
    return "$";

  std::string result = "$";

  for (size_t i(0); i < m_stack.size(); ++i)
  {
    if (std::holds_alternative<size_t>(m_stack.at(i)))
    {
      size_t index = std::get<size_t>(m_stack.at(i));
      result += "[" + std::to_string(index) + "]";
    }
    else
    {
      const std::string& text = std::get<std::string>(m_stack.at(i));
      result += "." + text;
    }
  }

  return result;
}

} // namespace dex
