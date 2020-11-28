// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/since.h"

namespace dex
{

const std::string Since::TypeId = "since";
const std::string BeginSince::TypeId = "BeginSince";
const std::string EndSince::TypeId = "EndSince";

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

const std::string& BeginSince::className() const
{
  return TypeId;
}

EndSince::EndSince(std::shared_ptr<BeginSince> bsince)
  : beginsince(bsince)
{

}

const std::string& EndSince::className() const
{
  return TypeId;
}

} // namespace dex
