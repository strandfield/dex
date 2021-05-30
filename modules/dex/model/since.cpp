// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/since.h"

namespace dex
{

Since::Since(std::string version)
  : m_version(std::move(version))
{

}

const std::string& Since::version() const
{
  return m_version;
}

BeginSince::BeginSince(std::string v)
  : version(std::move(v))
{

}

model::Kind BeginSince::kind() const
{
  return ClassKind;
}

EndSince::EndSince(std::shared_ptr<BeginSince> bsince)
  : beginsince(bsince)
{

}

model::Kind EndSince::kind() const
{
  return ClassKind;
}

} // namespace dex
