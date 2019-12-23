// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/multiline-comment.h"

namespace cxx
{

const std::string MultilineComment::TypeId = "multiline-comment";

const std::string& MultilineComment::type() const
{
  return TypeId;
}

} // namespace cxx
