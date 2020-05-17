// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_STRINGUTILS_H
#define DEX_COMMON_STRINGUTILS_H

#include <string>

namespace dex
{

class StdStringCRef
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
};

StdStringCRef StdString(const std::string& str)
{
  return StdStringCRef{ str };
}

} // namespace dex

#endif // DEX_COMMON_STRINGUTILS_H
