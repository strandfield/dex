// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/manual.h"

namespace dex
{

const std::string Manual::TypeId = "manual";

const std::string& Manual::type() const
{
  return TypeId;
}

} // namespace dex
