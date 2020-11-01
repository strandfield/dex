// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MACRO_DOCUMENTATION_H
#define DEX_MODEL_MACRO_DOCUMENTATION_H

#include "dex/model/entity-documentation.h"

namespace dex
{

class DEX_MODEL_API MacroDocumentation : public EntityDocumentation
{

public:

  static const std::string TypeId;
  const std::string& className() const override;

};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_MODEL_MACRO_DOCUMENTATION_H
