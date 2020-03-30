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

struct DEX_MODEL_API EnumValueDocumentation
{
  std::string name;
  std::optional<int> value;
  std::optional<Since> since;
  dom::Content description;
};

class DEX_MODEL_API EnumDocumentation : public EntityDocumentation
{
private:
  std::vector<EnumValueDocumentation> m_values;

public:

  static const std::string TypeId;
  const std::string& type() const override;

  std::vector<EnumValueDocumentation>& values();
  const std::vector<EnumValueDocumentation>& values() const;
};

} // namespace dex

#endif // DEX_MODEL_ENUM_DOCUMENTATION_H
