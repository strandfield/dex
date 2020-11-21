// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_CODEBLOCK_H
#define DEX_MODEL_CODEBLOCK_H

#include "dex/dex-model.h"

#include <dom/element.h>

namespace dex
{

class DEX_MODEL_API CodeBlock : public dom::Element
{
public:
  std::string lang;
  std::string code;

public:
  CodeBlock();

  static const std::string TypeId;
  const std::string& className() const override;
};

} // namespace dex

#endif // DEX_MODEL_CODEBLOCK_H
