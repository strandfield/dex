// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/variable-documentation.h"

namespace dex
{

const std::string VariableDocumentation::TypeId = "variable-documentation";

const std::string& VariableDocumentation::className() const
{
  return TypeId;
}

} // namespace dex
