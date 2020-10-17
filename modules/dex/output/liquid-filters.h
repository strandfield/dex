// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_FILTERS_H
#define DEX_OUTPUT_LIQUID_FILTERS_H

#include "dex/dex-output.h"

#include <dom/content.h>

#include <json-toolkit/json.h>

namespace dom
{
class Image;
class List;
class ListItem;
class Paragraph;
} // namespace dom

namespace dex
{

class DisplayMath;
class GroupTable;
class Sectioning;

class LiquidExporter;

class DEX_OUTPUT_API LiquidFilters
{
public:
  LiquidExporter& renderer;

public:
  explicit LiquidFilters(LiquidExporter& exp);
  ~LiquidFilters();

  json::Json apply(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) const;

protected:
  static json::Array filter_by_field(const json::Array& list, const std::string& field, const std::string& value);
  static json::Array filter_by_type(const json::Array& list, const std::string& type);
  static json::Array filter_by_accessibility(const json::Array& list, const std::string& as);
  json::Array related_non_members(const json::Object& json_class) const;
  json::Array group_get_entities(const json::Object& json_group) const;
  json::Array group_get_manuals(const json::Object& json_group) const;
  json::Array group_get_groups(const json::Object& json_group) const;
};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_FILTERS_H
