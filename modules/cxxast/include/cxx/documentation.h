// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DOCUMENTATION_H
#define CXXAST_DOCUMENTATION_H

#include "cxx/cxxast-defs.h"

#include "cxx/sourcelocation.h"

#include <dom/paragraph.h>

#include <memory>
#include <utility>
#include <vector>

namespace cxx
{

class CXXAST_API Documentation : public dom::Node
{
private:
  SourceLocation m_location;

public:
  Documentation() = default;
  ~Documentation() = default;

  explicit Documentation(const SourceLocation& loc);

  const SourceLocation& location() const;
  SourceLocation& location();

};

} // namespace cxx

namespace cxx
{

inline Documentation::Documentation(const SourceLocation& loc)
  : m_location(loc)
{

}

inline const SourceLocation& Documentation::location() const
{
  return m_location;
}

inline SourceLocation& Documentation::location()
{
  return m_location;
}

} // namespace cxx

#endif // CXXAST_DOCUMENTATION_H
