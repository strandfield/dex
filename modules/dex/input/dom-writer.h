// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOMWRITER_H
#define DEX_INPUT_DOMWRITER_H

#include "dex/dex-input.h"

namespace dex
{

inline bool is_space(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

} // namespace dex

#endif // DEX_INPUT_DOMWRITER_H
