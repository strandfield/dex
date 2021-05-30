// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUIDWRAPPER_H
#define DEX_OUTPUT_LIQUIDWRAPPER_H

#include "dex/dex-output.h"

#include "dex/model/model-visitor.h"

#include <liquid/value.h>

namespace dex
{

class DEX_OUTPUT_API LiquidModel : public liquid::IValue
{
public:
  std::shared_ptr<Model> model;

public:
  explicit LiquidModel(const std::shared_ptr<Model>& m);

  std::type_index type_index() const override;
  void* data() override;

  bool is_map() const override;
  std::set<std::string> propertyNames() const override;
  liquid::Value property(const std::string& name) const override;
};

class DEX_OUTPUT_API LiquidModelObject : public liquid::IValue
{
public:
  std::shared_ptr<model::Object> object;

public:
  explicit LiquidModelObject(std::shared_ptr<model::Object> obj);

  std::type_index type_index() const override;
  void* data() override;

  bool is_map() const override;
  std::set<std::string> propertyNames() const override;
  liquid::Value property(const std::string& name) const override;
};

inline liquid::Value to_liquid(std::shared_ptr<Model> m)
{
  return liquid::Value(std::make_shared<LiquidModel>(m));
}

inline liquid::Value to_liquid(std::shared_ptr<model::Object> obj)
{
  return obj ? liquid::Value(std::make_shared<LiquidModelObject>(obj)) : liquid::Value();
}

template<typename T>
liquid::Value to_liquid(const std::vector<T>& objs)
{
  liquid::Array r;
  
  for (auto o : objs)
    r.push(to_liquid(o));

  return r;
}

inline std::shared_ptr<model::Object> from_liquid(const liquid::Value& val)
{
  return val.is<std::shared_ptr<model::Object>>() ? val.as<std::shared_ptr<model::Object>>() : nullptr;
}

template<typename T>
std::shared_ptr<T> liquid_cast(const liquid::Value& val)
{
  return std::dynamic_pointer_cast<T>(from_liquid(val));
}

} // namespace dex

#endif // DEX_OUTPUT_LIQUIDWRAPPER_H
