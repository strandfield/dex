// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/macro-documentation.h"

namespace dex
{

const std::string MacroDocumentation::TypeId = "macro-documentation";

const std::string& MacroDocumentation::className() const
{
  return TypeId;
}

} // namespace dex
