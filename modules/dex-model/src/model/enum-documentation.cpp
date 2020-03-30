// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/enum-documentation.h"

namespace dex
{

const std::string EnumDocumentation::TypeId = "enum-documentation";

const std::string& EnumDocumentation::type() const
{
  return TypeId;
}

std::vector<EnumValueDocumentation>& EnumDocumentation::values()
{
  return m_values;
}

const std::vector<EnumValueDocumentation>& EnumDocumentation::values() const
{
  return m_values;
}

} // namespace dex
