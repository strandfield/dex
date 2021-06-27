// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_STRINGUTILS_H
#define DEX_COMMON_STRINGUTILS_H

#include "dex/dex-common.h"

#include <algorithm>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace dex
{

class DEX_COMMON_API StdStringCRef
{
public:
  const std::string* str_;

  StdStringCRef(const std::string& s)
    : str_(&s)
  {

  }

  bool starts_with(const std::string& other) const
  {
    if (other.size() > str_->size())
      return false;

    for (size_t i(0); i < other.size(); ++i)
    {
      if (str_->at(i) != other.at(i))
        return false;
    }

    return true;
  }

  bool ends_with(const std::string& other) const
  {
    if (other.empty())
      return true;

    if (other.size() > str_->size())
      return false;

    for (size_t i(0); i < other.size(); ++i)
    {
      if (str_->at(str_->size() - 1 - i) != other.at(other.size() - 1 - i))
        return false;
    }

    return true;
  }

  std::string_view view(std::string::const_iterator begin, std::string::const_iterator end) const
  {
    const std::string& str = *str_;
    size_t offset = std::distance(str.begin(), begin);
    size_t len = std::distance(begin, end);
    return std::string_view(str.data() + offset, len);
  }

  std::vector<std::string_view> split(char sep) const
  {
    const std::string& str = *str_;

    std::vector<std::string_view> r;

    auto it = str.begin();
    auto next = std::find(str.begin(), str.end(), sep);

    while (next != str.end())
    {
      r.push_back(view(it, next));
      it = next + 1;
      next = std::find(it, str.end(), sep);
    }

    if (it != next)
      r.push_back(view(it, next));

    return r;
  }

  std::string replace(std::initializer_list<std::pair<std::string_view, std::string_view>>&& replacements) const;

};

inline StdStringCRef StdString(const std::string& str)
{
  return StdStringCRef{ str };
}

inline std::vector<std::string> str_split(const std::string& str, char sep)
{
  std::vector<std::string> r;

  auto it = str.begin();
  auto next = std::find(str.begin(), str.end(), sep);

  while (next != str.end())
  {
    r.push_back(std::string(it, next));
    it = next + 1;
    next = std::find(it, str.end(), sep);
  }

  if (it != next)
    r.push_back(std::string(it, next));

  return r;
}

} // namespace dex

#endif // DEX_COMMON_STRINGUTILS_H
