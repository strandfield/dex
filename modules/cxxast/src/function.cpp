// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/function.h"

namespace cxx
{

const std::string Function::TypeId = "function";

const std::string& Function::type() const
{
  return TypeId;
}

} // namespace cxx
