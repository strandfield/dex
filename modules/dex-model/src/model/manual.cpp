// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/manual.h"

namespace dex
{

const std::string Sectioning::TypeId = "sectioning";

const std::string& Sectioning::type() const
{
  return TypeId;
}

const std::string Manual::TypeId = "manual";

const std::string& Manual::type() const
{
  return TypeId;
}

Sectioning::Sectioning(Depth d, std::string n)
  : depth(d),
    name(std::move(n))
{

}

Manual::Manual(std::string t)
  : title(std::move(t))
{

}


} // namespace dex
