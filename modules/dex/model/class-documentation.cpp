// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/class-documentation.h"

namespace dex
{

const std::string ClassDocumentation::TypeId = "class-documentation";

const std::string& ClassDocumentation::className() const
{
  return TypeId;
}

} // namespace dex
