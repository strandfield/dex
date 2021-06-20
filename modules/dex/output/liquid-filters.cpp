// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-filters.h"

#include "dex/output/liquid-exporter.h"
#include "dex/output/liquid-wrapper.h"

#include <liquid/filters.h>

#include <algorithm>

namespace dex
{

static liquid::Array array_arg(const liquid::Value& object)
{
  if (object.isNull())
    return liquid::Array();
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

liquid::Value LiquidFilters::apply(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args) const
{
  if (name == "filter_by_type")
  {
    return filter_by_type(array_arg(object), args.front().as<std::string>());
  }
  else if (name == "filter_by_accessibility")
  {
    return filter_by_accessibility(array_arg(object), args.front().as<std::string>());
  }
  else if (name == "filter_by_field")
  {
    return filter_by_field(array_arg(object), args.front().as<std::string>(), args.back().as<std::string>());
  }
  else if (name == "funsig")
  {
    return funsig(object.toMap());
  }
  else if (name == "related_non_members")
  {
    return related_non_members(object.toMap());
  }
  else if (name == "group_get_entities")
  {
    return group_get_entities(object.toMap());
  }
  else if (name == "group_get_manuals")
  {
    return group_get_manuals(object.toMap());
  }
  else if (name == "get_url")
  {
    return get_url(object);
  }
  else if (name == "has_any_documented_param")
  {
    return has_any_documented_param(object, args);
  }
  else if (name == "param_brief_or_name")
  {
    return param_brief_or_name(object, args);
  }

  return liquid::BuiltinFilters::apply(name, object, args);
}

liquid::Array LiquidFilters::filter_by_field(const liquid::Array& list, const std::string& field, const std::string& value)
{
  liquid::Array result;

  for (size_t i(0); i < list.length(); ++i)
  {
    liquid::Value obj = list.at(i);
    liquid::Value prop = obj.property(field);

    if (prop.is<std::string>() && prop.as<std::string>() == value)
      result.push(obj);
  }

  return result;
}

liquid::Array LiquidFilters::filter_by_type(const liquid::Array& list, const std::string& type)
{
  static const std::string field = "type";
  return filter_by_field(list, field, type);
}

liquid::Array LiquidFilters::filter_by_accessibility(const liquid::Array& list, const std::string& as)
{
  static const std::string field = "accessibility";
  return filter_by_field(list, field, as);
}

std::string LiquidFilters::funsig(const liquid::Map& liqfun) const
{
  auto fun = liquid_cast<dex::Function>(liqfun);
  return fun->signature();
}

liquid::Array LiquidFilters::related_non_members(const liquid::Map& liq_class) const
{
  auto the_class = liquid_cast<dex::Class>(liq_class);

  if (the_class == nullptr)
    return {};

  std::shared_ptr<RelatedNonMembers::Entry> entry = renderer.model()->program()->related.getRelated(the_class);

  liquid::Array result;

  if (entry == nullptr)
    return result;

  for (auto f : entry->non_members)
  {
    result.push(to_liquid(f));
  }

  return result;
}

liquid::Array LiquidFilters::group_get_entities(const liquid::Map& liqgroup) const
{
  liquid::Array result;

  auto g = liquid_cast<Group>(liqgroup);

  if (g == nullptr)
  {
    // @TODO: maybe log something
    return result;
  }

  for (auto e : g->content.entities)
  {
    result.push(to_liquid(e));
  }

  return result;
}

liquid::Array LiquidFilters::group_get_manuals(const liquid::Map& liqgroup) const
{
  liquid::Array result;

  auto g = liquid_cast<Group>(liqgroup);

  if (g == nullptr)
  {
    // @TODO: maybe log something
    return result;
  }

  for (auto doc : g->content.documents)
  {
    result.push(to_liquid(doc));
  }

  return result;
}

liquid::Value LiquidFilters::get_url(const liquid::Value& object) const
{
  return renderer.get_url(from_liquid(object));
}

bool LiquidFilters::has_any_documented_param(const Function& fun)
{
  return std::any_of(fun.parameters.begin(), fun.parameters.end(), [](const std::shared_ptr<FunctionParameter>& p) -> bool {
    return p->brief.has_value();
    });
}

liquid::Value LiquidFilters::has_any_documented_param(const liquid::Value& object, const std::vector<liquid::Value>& /* args */)
{
  auto fun = liquid_cast<Function>(object);
  return fun && has_any_documented_param(*fun);
}

std::string LiquidFilters::param_brief_or_name(const FunctionParameter& fp)
{
  return fp.brief.value_or(fp.name);
}

liquid::Value LiquidFilters::param_brief_or_name(const liquid::Value& object, const std::vector<liquid::Value>& args)
{
  auto fp = liquid_cast<FunctionParameter>(object);
  return fp ? param_brief_or_name(*fp) : std::string();
}

} // namespace dex
