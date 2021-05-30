// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model.h"

#include <stdexcept>

namespace dex
{

model::Kind Model::kind() const
{
  return ClassKind;
}

bool Model::empty() const
{
  return m_program == nullptr && documents.empty();
}

} // namespace dex
