// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_TYPE_H
#define CXXAST_TYPE_H

#include "cxx/entity.h"

#include <memory>

namespace cxx
{

class CXXAST_API Type
{
private:
  std::string m_name;
  std::shared_ptr<Entity> m_def;

public:
  explicit Type(std::string name, std::shared_ptr<Entity> def = nullptr);

  const std::string& name() const;
  const std::shared_ptr<Entity>& definition() const;
};

} // namespace cxx

namespace cxx
{

inline Type::Type(std::string name, std::shared_ptr<Entity> def)
  : m_name(std::move(name)), 
    m_def(std::move(def))
{

}

inline const std::string& Type::name() const
{
  return m_name;
}

inline const std::shared_ptr<Entity>& Type::definition() const
{
  return m_def;
}

} // namespace cxx

#endif // CXXAST_TYPE_H
