// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/display-math.h"

namespace dex
{

const std::string DisplayMath::TypeId = "math";

const std::string& DisplayMath::type() const
{
  return TypeId;
}

DisplayMath::DisplayMath()
{

}

} // namespace dex
