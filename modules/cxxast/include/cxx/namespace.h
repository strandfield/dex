// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_NAMESPACE_H
#define CXXAST_NAMESPACE_H

#include "cxx/entity.h"

#include <map>
#include <memory>
#include <vector>

namespace cxx
{

class CXXAST_API Namespace : public Entity
{
public:
  ~Namespace() = default;

  static const std::string TypeId;
  const std::string& type() const override;

  explicit Namespace(std::string name, std::shared_ptr<Entity> parent = nullptr);

  std::vector<std::shared_ptr<Entity>>& entities();
  const std::vector<std::shared_ptr<Entity>>& entities() const;

private:
  std::vector<std::shared_ptr<Entity>> m_entities;
};

} // namespace cxx

namespace cxx
{

inline Namespace::Namespace(std::string name, std::shared_ptr<Entity> parent)
  : Entity{ std::move(name), std::move(parent) }
{

}

inline std::vector<std::shared_ptr<Entity>>& Namespace::entities()
{
  return m_entities;
}

inline const std::vector<std::shared_ptr<Entity>>& Namespace::entities() const
{
  return m_entities;
}

} // namespace cxx

#endif // CXXAST_NAMESPACE_H
