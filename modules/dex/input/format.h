// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_FORMAT_H
#define DEX_INPUT_FORMAT_H

#include "dex/dex-input.h"

#include <tex/parsing/format.h>

namespace dex
{

class DEX_INPUT_API DexFormat
{
public:
  static std::vector<tex::parsing::Macro> load();
};

} // namespace dex

#endif // DEX_INPUT_FORMAT_H
