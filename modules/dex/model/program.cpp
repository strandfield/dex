// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/program.h"

namespace dex
{

bool RelatedNonMembers::empty() const
{
  return class_map.empty();
}

void RelatedNonMembers::relates(const std::shared_ptr<cxx::Function>& f, const std::shared_ptr<cxx::Class>& c)
{
  auto& entry = class_map[c];

  if (entry == nullptr)
    entry = std::make_shared<Entry>(c);

  entry->non_members.push_back(f);
  functions_map[f] = entry;
}

std::shared_ptr<RelatedNonMembers::Entry> RelatedNonMembers::getRelated(const std::shared_ptr<cxx::Class>& c) const
{
  auto it = class_map.find(c);
  return it == class_map.end() ? nullptr : it->second;
}

std::shared_ptr<RelatedNonMembers::Entry> RelatedNonMembers::getRelated(const std::shared_ptr<cxx::Function>& f) const
{
  auto it = functions_map.find(f);
  return it == functions_map.end() ? nullptr : it->second;
}

} // namespace dex
