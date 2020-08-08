// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
#define DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H

#include "dex/output/json-mapping.h"

#include "dex/model/model-visitor.h"

#include <json-toolkit/json.h>

namespace dex
{

class DEX_OUTPUT_API JsonUrlAnnotator : public ModelVisitor
{
private:
  json::Object& serialized_model;
  const JsonExportMapping& json_mapping;

public:
  JsonUrlAnnotator(json::Object& js_model, const JsonExportMapping& mapping);

  void annotate(const Model& model);

  static json::Json get(const Model::Path& path, const json::Json& val);

protected:
  void visit_entity(const cxx::Entity& e) override;
  void visit_manual(const dex::Manual& man) override;

private:
  virtual std::string get_url(const cxx::Entity& e) const = 0;
  virtual std::string get_url(const dex::Manual& man) const = 0;
};

} // namespace dex

namespace dex
{

inline json::Json JsonUrlAnnotator::get(const Model::Path& path, const json::Json& val)
{
  auto result = val;

  // @TODO: try to avoid potentially costly std::string conv
  for (const auto& p : path)
  {
    if (p.index != std::numeric_limits<size_t>::max())
      result = result[std::string(p.name)][static_cast<int>(p.index)];
    else
      result = result[std::string(p.name)];
  }

  return result;
}

} // namespace dex

#endif // DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
