// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-output-annotator.h"

namespace dex
{

void JsonUrlAnnotator::annotate(const Model& model, json::Object& obj)
{
  serialized_model = obj;
  visit(model);
}


void JsonUrlAnnotator::visit_entity(const cxx::Entity& e)
{
  std::string url = get_url(e);

  if (!url.empty())
  {
    json::Object obj = JsonPathAnnotator::get(path(), serialized_model).toObject();
    obj["url"] = url;
  }
  
  ModelVisitor::visit_entity(e);
}

void JsonUrlAnnotator::visit_manual(const dex::Manual& man)
{
  std::string url = get_url(man);

  if (!url.empty())
  {
    json::Object obj = JsonPathAnnotator::get(path(), serialized_model).toObject();
    obj["url"] = url;
  }

  ModelVisitor::visit_manual(man);
}

} // namespace dex
