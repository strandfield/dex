// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/display-math.h"

namespace dex
{

const std::string DisplayMath::TypeId = "math";

const std::string& DisplayMath::className() const
{
  return TypeId;
}

DisplayMath::DisplayMath()
{

}

inline static bool is_letter(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline static bool is_brace_or_bracket(char c)
{
  return c == '{' || c == '}' || c == '[' || c == ']';
}


void DisplayMath::normalize(std::string& str, size_t offset)
{
  char* data = str.data();

  size_t w = offset;

  while (offset < str.size())
  {
    while (offset < str.size() && data[offset] != '\\')
      data[w++] = data[offset++];

    if (offset == str.size())
      break;

    data[w++] = data[offset++];

    while (offset < str.size() && is_letter(data[offset]))
      data[w++] = data[offset++];

    if (offset == str.size())
      break;

    if (offset == str.size() - 1 && data[offset] == ' ')
    {
      ++offset;
      break;
    }

    if (offset < str.size() - 1 && data[offset] == ' ' && is_brace_or_bracket(data[offset+1]))
    {
      ++offset;
    }
    else
    {
      data[w++] = data[offset++];
    }
  }

  str.resize(w);
}

void DisplayMath::normalize()
{
  normalize(source, 0);
}

} // namespace dex
