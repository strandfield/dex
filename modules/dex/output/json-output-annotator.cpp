// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-output-annotator.h"

namespace dex
{

JsonUrlAnnotator::JsonUrlAnnotator(json::Object& js_model, const JsonExportMapping& mapping)
  : serialized_model(js_model),
    json_mapping(mapping)
{

}

void JsonUrlAnnotator::annotate(const Model& model)
{
  visit(model);
}

void JsonUrlAnnotator::visit_entity(const cxx::Entity& e)
{
  std::string url = get_url(e);

  if (!url.empty())
  {
    json::Object obj = json_mapping.get(e).toObject();
    obj["url"] = url;
  }
  
  ModelVisitor::visit_entity(e);
}

void JsonUrlAnnotator::visit_document(const dex::Document& doc)
{
  std::string url = get_url(doc);

  if (!url.empty())
  {
    json::Object obj = JsonUrlAnnotator::get(path(), serialized_model).toObject();
    obj["url"] = url;
  }

  ModelVisitor::visit_document(doc);
}

} // namespace dex
