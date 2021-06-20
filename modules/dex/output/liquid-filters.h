// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_FILTERS_H
#define DEX_OUTPUT_LIQUID_FILTERS_H

#include "dex/dex-output.h"

#include <liquid/value.h>

namespace dex
{

class Function;
class FunctionParameter;

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

  liquid::Value apply(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args) const;

protected:
  static liquid::Array filter_by_field(const liquid::Array& list, const std::string& field, const std::string& value);
  static liquid::Array filter_by_type(const liquid::Array& list, const std::string& type);
  static liquid::Array filter_by_accessibility(const liquid::Array& list, const std::string& as);
  std::string funsig(const liquid::Map& liqfun) const;
  liquid::Array related_non_members(const liquid::Map& liqclass) const;
  liquid::Array group_get_entities(const liquid::Map& liqgroup) const;
  liquid::Array group_get_manuals(const liquid::Map& liqgroup) const;
  liquid::Value get_url(const liquid::Value& object) const;
  static bool has_any_documented_param(const Function& fun);
  static liquid::Value has_any_documented_param(const liquid::Value& object, const std::vector<liquid::Value>& args);
  static std::string param_brief_or_name(const FunctionParameter& fp);
  static liquid::Value param_brief_or_name(const liquid::Value& object, const std::vector<liquid::Value>& args);
};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_FILTERS_H
