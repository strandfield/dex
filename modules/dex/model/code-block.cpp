// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/code-block.h"

namespace dex
{

const std::string CodeBlock::TypeId = "CodeBlock";

const std::string& CodeBlock::className() const
{
  return TypeId;
}

CodeBlock::CodeBlock()
{

}


class CodeBlockNormalizer
{
public:
  std::string& code;
  size_t r = 0;
  size_t w = 0;
  size_t spaceindent = 0;

  CodeBlockNormalizer(std::string& c)
    : code(c)
  {

  }

  void normalize()
  {
    /* Remove trailing spaces */

    while (!code.empty() && (code.back() == ' ' || code.back() == '\n'))
      code.pop_back();

    /* Remove leading spaces */

    size_t i = 0;

    while (i < code.size() && code.at(i) == ' ')
      ++i;

    if (i == code.size())
    {
      code.clear();
      return;
    }
    else if (code.at(i) == '\n')
    {
      while (i-- > 0)
        discard();
      discard();
    }

    /* Remove extra indent */

    spaceindent = detect_space_indent();

    while (!atEnd())
    {
      remove_spaces();
      seek_next_line();
    }

    resize();
  }

protected:

  size_t detect_space_indent() const
  {
    if (code.empty())
      return 0;

    size_t i = 0;

    if (code.at(i) == '\n')
      ++i;

    while (i < code.size() && code.at(i) == ' ')
      ++i;

    return i - (code.at(0) == '\n' ? 1 : 0);
  }

  bool atEnd() const
  {
    return r == code.size();
  }

  char peek() const
  {
    return code.at(r);
  }

  void read()
  {
    code[w++] = code[r++];
  }

  void discard()
  {
    ++r;
  }

  void resize()
  {
    code.resize(w);
  }

  void remove_spaces()
  {
    size_t n = 0;

    while (!atEnd() && (n < spaceindent) && peek() == ' ')
    {
      ++n;
      discard();
    }
  }

  void seek_next_line()
  {
    while (!atEnd() && peek() != '\n')
      read();

    if(!atEnd())
      read();
  }
};

void CodeBlock::normalize()
{
  CodeBlockNormalizer normalizer{ code };
  normalizer.normalize();
}

} // namespace dex
