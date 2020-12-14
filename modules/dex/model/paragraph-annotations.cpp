// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/paragraph-annotations.h"

namespace dex
{

const std::string ParIndexEntry::TypeId = "ParIndexEntry";

ParIndexEntry::ParIndexEntry(std::string k)
  : key(std::move(k))
{

}

} // namespace dex
