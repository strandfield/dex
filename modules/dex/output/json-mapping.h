// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_MAPPING_H
#define DEX_OUTPUT_JSON_MAPPING_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <json-toolkit/json.h>

#include <unordered_map>

namespace dex
{

class DEX_OUTPUT_API JsonExportMapping
{
public:

  struct BackwardMaps
  {
    std::unordered_map<std::shared_ptr<json::details::Node>, std::shared_ptr<cxx::Entity>> entities;
    std::unordered_map<std::shared_ptr<json::details::Node>, const Document*> documents;
    std::unordered_map<std::shared_ptr<json::details::Node>, dom::Node*> domnodes;
    std::unordered_map<std::shared_ptr<json::details::Node>, std::shared_ptr<const Group>> groups;
  };

  BackwardMaps backward_maps;
  std::unordered_map<const void*, json::Json> forward;

  json::Json get(const cxx::Entity& n) const
  {
    return forward.at(&n);
  }

  json::Json get(const Document& doc) const
  {
    return forward.at(&doc);
  }

  json::Json get(const dom::Node& n) const
  {
    return forward.at(&n);
  }

  json::Json get(const Group& g) const
  {
    return forward.at(&g);
  }

  void bind(cxx::Entity& e, const json::Json& o)
  {
    backward_maps.entities[o.impl()] = e.shared_from_this();
    forward[&e] = o;
  }

  void bind(const Document& doc, const json::Json& o)
  {
    backward_maps.documents[o.impl()] = &doc;
    forward[&doc] = o;
  }

  void bind(dom::Node& n, const json::Json& o)
  {
    backward_maps.domnodes[o.impl()] = &n;
    forward[&n] = o;
  }

  void bind(const Group& g, const json::Json& o)
  {
    backward_maps.groups[o.impl()] = g.shared_from_this();
    forward[&g] = o;
  }

  template<typename T>
  struct get_helper_t {};

  template<typename T>
  auto get(const json::Json& obj) const
  {
    return get_impl(obj, get_helper_t<T>());
  }

protected:

  std::shared_ptr<cxx::Entity> get_impl(const json::Json& obj, get_helper_t<cxx::Entity>) const
  {
    return backward_maps.entities.at(obj.impl());
  }

  const Document* get_impl(const json::Json& obj, get_helper_t<Document>) const
  {
    return backward_maps.documents.at(obj.impl());
  }

  std::shared_ptr<dom::Node> get_impl(const json::Json& obj, get_helper_t<dom::Node>) const
  {
    return backward_maps.domnodes.at(obj.impl())->shared_from_this();
  }

  std::shared_ptr<const Group> get_impl(const json::Json& obj, get_helper_t<Group>) const
  {
    return backward_maps.groups.at(obj.impl());
  }
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_MAPPING_H
