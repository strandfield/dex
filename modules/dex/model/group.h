// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_GROUP_H
#define DEX_MODEL_GROUP_H

#include "dex/dex-model.h"

#include <cxx/entity.h>

#include <json-toolkit/json.h>

#include <unordered_map>
#include <vector>

namespace dex
{

class Document;

class Group;

class DEX_MODEL_API GroupManager
{
public:
  std::vector<std::shared_ptr<Group>> groups;
  std::map<std::string, std::shared_ptr<Group>> name_map;

  std::shared_ptr<Group> get(const std::string& name) const;
  std::shared_ptr<Group> getOrCreate(const std::string& name);

  template<typename T>
  void multiInsert(const std::vector<std::string>& groupnames, T elem);
};

class DEX_MODEL_API Group : public std::enable_shared_from_this<Group>
{
public:
  size_t index;
  std::string name;

  struct Content
  {
    std::vector<std::shared_ptr<cxx::Entity>> entities;
    std::vector<std::shared_ptr<dex::Document>> documents;
  };

  Content content;

  void insert(std::shared_ptr<cxx::Entity> e);
  void insert(std::shared_ptr<dex::Document> doc);

public:
  Group(size_t index, std::string n);
};

template<typename T>
inline void GroupManager::multiInsert(const std::vector<std::string>& groupnames, T elem)
{
  for (const auto& gname : groupnames)
  {
    std::shared_ptr<Group> g = getOrCreate(gname);
    g->insert(elem);
  }
}

} // namespace dex

#endif // DEX_MODEL_GROUP_H
