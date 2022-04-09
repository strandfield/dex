// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "string-utils.h"

#include <cstring>

namespace dex
{

inline static bool replace_at(std::string& result, const std::string& str, size_t& index, const std::string& pattern, const std::string& replacement)
{
  if (pattern.size() <= (str.size() - index)
    && std::strncmp(str.data() + index, pattern.data(), pattern.size()) == 0)
  {
    result.insert(result.end(), replacement.begin(), replacement.end());
    index += pattern.size();
    return true;
  }

  return false;
}

std::string StdStringCRef::replace(const std::string& str, const std::string& replacement) const
{
  const std::string& self = *str_;

  std::string r;
  r.reserve(self.size());

  for (size_t i(0); i < self.size();)
  {
    if (!replace_at(r, self, i, str, replacement))
      r.push_back(self.at(i++));
  }

  return r;
}

inline static bool replace_at(std::string& result, const std::string& str, size_t& index, std::initializer_list<std::pair<std::string_view, std::string_view>>& replacements)
{
  for (const std::pair<std::string_view, std::string_view>& repl : replacements)
  {
    if (repl.first.size() <= (str.size() - index) 
      && std::strncmp(str.data() + index, repl.first.data(), repl.first.size()) == 0)
    {
      result.insert(result.end(), repl.second.begin(), repl.second.end());
      index += repl.first.size();
      return true;
    }
  }

  return false;
}

std::string StdStringCRef::replace(std::initializer_list<std::pair<std::string_view, std::string_view>>&& replacements) const
{
  const std::string& str = *str_;

  std::string r;
  r.reserve(str.size());

  for (size_t i(0); i < str.size();)
  {
    if (!replace_at(r, str, i, replacements))
      r.push_back(str.at(i++));
  }

  return r;
}

} // namespace dex

