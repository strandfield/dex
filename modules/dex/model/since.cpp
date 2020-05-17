// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/since.h"

namespace dex
{

const std::string Since::TypeId = "since";

Since::Since(std::string version)
  : m_version(std::move(version))
{

}

const std::string& Since::version() const
{
  return m_version;
}

} // namespace dex
