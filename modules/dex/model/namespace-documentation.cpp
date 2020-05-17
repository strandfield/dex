// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/namespace-documentation.h"

namespace dex
{

const std::string NamespaceDocumentation::TypeId = "namespace-documentation";

const std::string& NamespaceDocumentation::type() const
{
  return TypeId;
}

} // namespace dex
