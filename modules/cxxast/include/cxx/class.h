// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_CLASS_H
#define CXXAST_CLASS_H

#include "cxx/entity.h"

#include <utility>
#include <vector>

namespace cxx
{

enum class AccessSpecifier
{
  PUBLIC,
  PROTECTED,
  PRIVATE,
};

class CXXAST_API Class : public Entity
{
public:
  ~Class() = default;

  explicit Class(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static const std::string TypeId;
  const std::string& type() const override;

  std::vector<std::pair<std::shared_ptr<Entity>, AccessSpecifier>>& members();
  const std::vector<std::pair<std::shared_ptr<Entity>, AccessSpecifier>>& members() const;

private:
  std::vector<std::pair<std::shared_ptr<Entity>, AccessSpecifier>> m_members;
};

} // namespace cxx

namespace cxx
{

inline Class::Class(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline std::vector<std::pair<std::shared_ptr<Entity>, AccessSpecifier>>& Class::members()
{
  return m_members;
}

inline const std::vector<std::pair<std::shared_ptr<Entity>, AccessSpecifier>>& Class::members() const
{
  return m_members;
}

} // namespace cxx

#endif // CXXAST_CLASS_H
