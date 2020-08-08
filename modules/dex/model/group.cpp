// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/group.h"

namespace dex
{

std::shared_ptr<Group> GroupManager::get(const std::string& name) const
{
  return name_map.at(name);
}

std::shared_ptr<Group> GroupManager::getOrCreate(const std::string& name)
{
  auto& g = name_map[name];

  if (g == nullptr)
  {
    g = std::make_shared<Group>(groups.size(), name);
    groups.push_back(g);
  }

  return g;
}

Group::Group(size_t i, std::string n)
  : index(i),
    name(std::move(n))
{

}

} // namespace dex
