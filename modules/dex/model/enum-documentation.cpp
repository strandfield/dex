// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/enum-documentation.h"

namespace dex
{

const std::string EnumDocumentation::TypeId = "enum-documentation";
const std::string EnumValueDocumentation::TypeId = "enum-value-documentation";

const std::string& EnumDocumentation::className() const
{
  return TypeId;
}

const std::string& EnumValueDocumentation::className() const
{
  return TypeId;
}

} // namespace dex
