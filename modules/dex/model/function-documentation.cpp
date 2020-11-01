// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/function-documentation.h"

namespace dex
{

const std::string FunctionDocumentation::TypeId = "function-documentation";

const std::string& FunctionDocumentation::className() const
{
  return TypeId;
}

FunctionParameterDocumentation::FunctionParameterDocumentation(std::string str)
  : brief(std::move(str))
{

}

} // namespace dex
