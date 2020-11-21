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

} // namespace dex
