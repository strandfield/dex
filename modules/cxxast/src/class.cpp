// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/class.h"

namespace cxx
{

const std::string Class::TypeId = "class";

const std::string& Class::type() const
{
  return TypeId;
}

} // namespace cxx
