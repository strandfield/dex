// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_PAR_ANNOTATIONS_H
#define DEX_MODEL_PAR_ANNOTATIONS_H

#include "dex/model/since.h"

namespace dex
{

class DEX_MODEL_API ParIndexEntry
{
public:
  std::string key;

public:
  ParIndexEntry(std::string k);

  static const std::string TypeId;
};

} // namespace dex

#endif // DEX_MODEL_PAR_ANNOTATIONS_H
