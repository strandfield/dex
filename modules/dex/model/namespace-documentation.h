// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_NAMESPACE_DOCUMENTATION_H
#define DEX_MODEL_NAMESPACE_DOCUMENTATION_H

#include "dex/model/entity-documentation.h"

#include <cxx/documentation.h>

namespace dex
{

class DEX_MODEL_API NamespaceDocumentation : public EntityDocumentation
{
public:

  static const std::string TypeId;
  const std::string& className() const override;

};

} // namespace dex

#endif // DEX_MODEL_NAMESPACE_DOCUMENTATION_H
