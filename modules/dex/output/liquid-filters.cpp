// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-filters.h"

#include "dex/output/liquid-exporter.h"

#include <liquid/filters.h>

namespace dex
{

static json::Array array_arg(const json::Json& object)
{
  if (object.isNull())
    return json::Array();
  else if (object.isArray())
    return object.toArray();
  else
    throw std::runtime_error{ "Object is not an array" };
}

LiquidFilters::LiquidFilters(LiquidExporter& exp)
  : renderer(exp)
{

}

LiquidFilters::~LiquidFilters()
{

}

json::Json LiquidFilters::apply(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) const
{
  if (name == "filter_by_type")
  {
    return filter_by_type(array_arg(object), args.front().toString());
  }
  else if (name == "filter_by_accessibility")
  {
    return filter_by_accessibility(array_arg(object), args.front().toString());
  }
  else if (name == "filter_by_field")
  {
    return filter_by_field(array_arg(object), args.front().toString(), args.back().toString());
  }
  else if (name == "funsig")
  {
    return funsig(object.toObject());
  }
  else if (name == "related_non_members")
  {
    return related_non_members(object.toObject());
  }
  else if (name == "group_get_entities")
  {
    return group_get_entities(object.toObject());
  }
  else if (name == "group_get_manuals")
  {
    return group_get_manuals(object.toObject());
  }

  return liquid::BuiltinFilters::apply(name, object, args);
}

json::Array LiquidFilters::filter_by_field(const json::Array& list, const std::string& field, const std::string& value)
{
  json::Array result;

  for (const auto& obj : list.data())
  {
    if (obj[field] == value)
      result.push(obj);
  }

  return result;
}

json::Array LiquidFilters::filter_by_type(const json::Array& list, const std::string& type)
{
  static const std::string field = "type";
  return filter_by_field(list, field, type);
}

json::Array LiquidFilters::filter_by_accessibility(const json::Array& list, const std::string& as)
{
  static const std::string field = "accessibility";
  return filter_by_field(list, field, as);
}

std::string LiquidFilters::funsig(const json::Object& json_fun) const
{
  auto fun_entity = renderer.modelMapping().get<cxx::Entity>(json_fun);
  auto fun = std::static_pointer_cast<const cxx::Function>(fun_entity);
  return fun->signature();
}

json::Array LiquidFilters::related_non_members(const json::Object& json_class) const
{
  auto path_it = json_class.data().find("_path");

  if (path_it == json_class.data().end())
  {
    assert(("element has no path", false));
    return {};
  }

  Model::Path path = Model::parse_path(path_it->second.toString());
  Model::Node model_node = renderer.model()->get(path);

  if (!std::holds_alternative<std::shared_ptr<cxx::Entity>>(model_node))
  {
    return {};
  }

  auto the_class = std::dynamic_pointer_cast<cxx::Class>(std::get<std::shared_ptr<cxx::Entity>>(model_node));

  if (the_class == nullptr)
    return {};

  std::shared_ptr<RelatedNonMembers::Entry> entry = renderer.model()->program()->related.getRelated(the_class);

  json::Array result;

  if (entry == nullptr)
    return result;

  for (auto f : entry->non_members)
  {
    json::Json f_json = renderer.modelMapping().get(*f);
    result.push(f_json);
  }

  return result;
}

json::Array LiquidFilters::group_get_entities(const json::Object& json_group) const
{
  json::Array result;

  auto g = renderer.modelMapping().get<Group>(json_group);

  if (g == nullptr)
  {
    // @TODO: maybe log something
    return result;
  }

  for (auto e : g->content.entities)
  {
    result.push(renderer.modelMapping().get(*e));
  }

  return result;
}

json::Array LiquidFilters::group_get_manuals(const json::Object& json_group) const
{
  json::Array result;

  auto g = renderer.modelMapping().get<Group>(json_group);

  if (g == nullptr)
  {
    // @TODO: maybe log something
    return result;
  }

  for (auto doc : g->content.documents)
  {
    result.push(renderer.modelMapping().get(*doc));
  }

  return result;
}

} // namespace dex
