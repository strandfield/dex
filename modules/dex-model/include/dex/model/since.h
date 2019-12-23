// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_SINCE_H
#define DEX_MODEL_SINCE_H

#include "dex/dex-model.h"

#include <string>

namespace dex
{

class DEX_MODEL_API Since
{
private:
  std::string m_version;

public:
  Since(std::string version);

  static const std::string TypeId;

  const std::string& version() const;
};

} // namespace dex

#endif // DEX_MODEL_SINCE_H
