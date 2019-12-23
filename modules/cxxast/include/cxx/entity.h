// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_ENTITY_H
#define CXXAST_ENTITY_H

#include "cxx/sourcelocation.h"

#include <dom/node.h>

#include <memory>

namespace cxx
{

class Documentation;

class CXXAST_API Entity : public dom::Node
{
private:
  std::string m_name;
  std::weak_ptr<Entity> m_parent;
  SourceLocation m_location;
  std::shared_ptr<Documentation> m_documentation;

public:
  explicit Entity(std::string name, std::shared_ptr<Entity> parent = nullptr);

  const std::string& name() const;
  std::shared_ptr<Entity> parent() const;

  const SourceLocation& location() const;
  SourceLocation& location();

  const std::shared_ptr<Documentation>& documentation() const;
  void setDocumentation(std::shared_ptr<Documentation> doc);
};

} // namespace cxx

namespace cxx
{

inline Entity::Entity(std::string name, std::shared_ptr<Entity> parent)
  : m_name(std::move(name)),
    m_parent(parent)
{

}

inline const std::string& Entity::name() const
{
  return m_name;
}

inline std::shared_ptr<Entity> Entity::parent() const
{
  return m_parent.lock();
}

inline const SourceLocation& Entity::location() const
{
  return m_location;
}

inline SourceLocation& Entity::location()
{
  return m_location;
}

inline const std::shared_ptr<Documentation>& Entity::documentation() const
{
  return m_documentation;
}

inline void Entity::setDocumentation(std::shared_ptr<Documentation> doc)
{
  m_documentation = doc;
}

} // namespace cxx

#endif // CXXAST_ENTITY_H
