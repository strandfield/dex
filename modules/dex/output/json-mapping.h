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
    std::unordered_map<std::shared_ptr<json::details::Node>, std::shared_ptr<const cxx::Entity>> entities;
    std::unordered_map<std::shared_ptr<json::details::Node>, const Manual*> manuals;
  };

  BackwardMaps backward_maps;
  std::unordered_map<const void*, json::Json> forward;

  json::Json get(const cxx::Entity& n) const
  {
    return forward.at(&n);
  }

  json::Json get(const Manual& m) const
  {
    return forward.at(&m);
  }

  void bind(const cxx::Entity& e, const json::Json& o)
  {
    backward_maps.entities[o.impl()] = e.shared_from_this();
    forward[&e] = o;
  }

  void bind(const Manual& m, const json::Json& o)
  {
    backward_maps.manuals[o.impl()] = &m;
    forward[&m] = o;
  }

  template<typename T>
  struct get_helper_t {};

  template<typename T>
  auto get(const json::Json& obj) const
  {
    return get_impl(obj, get_helper_t<T>());
  }

protected:

  std::shared_ptr<const cxx::Entity> get_impl(const json::Json& obj, get_helper_t<cxx::Entity>) const
  {
    return backward_maps.entities.at(obj.impl());
  }

  const Manual* get_impl(const json::Json& obj, get_helper_t<Manual>) const
  {
    return backward_maps.manuals.at(obj.impl());
  }
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_MAPPING_H
