// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/typedef-documentation.h"

namespace dex
{

const std::string TypedefDocumentation::TypeId = "typedef-documentation";

const std::string& TypedefDocumentation::type() const
{
  return TypeId;
}

} // namespace dex
