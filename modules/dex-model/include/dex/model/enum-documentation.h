// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_ENUM_DOCUMENTATION_H
#define DEX_MODEL_ENUM_DOCUMENTATION_H

#include "dex/model/entity-documentation.h"

#include "dex/model/since.h"

#include <dom/content.h>

#include <optional>

namespace dex
{

class DEX_MODEL_API EnumValueDocumentation : public EntityDocumentation
{
public:
  std::optional<int> value;

public:
  static const std::string TypeId;
  const std::string& type() const override;
};

class DEX_MODEL_API EnumDocumentation : public EntityDocumentation
{
public:

  static const std::string TypeId;
  const std::string& type() const override;
};

} // namespace dex

#endif // DEX_MODEL_ENUM_DOCUMENTATION_H
