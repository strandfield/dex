// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-output-annotator.h"

namespace dex
{

class JsonUrlAnnotatorProgramVisitor : public ProgramVisitor
{
public:
  JsonUrlAnnotator& annotator;

  JsonUrlAnnotatorProgramVisitor(JsonUrlAnnotator& ann)
    : annotator(ann)
  {

  }

  void visit(cxx::Entity& e) override
  {
    std::string url = annotator.get_url(e);

    if (!url.empty())
    {
      json::Object obj = annotator.json_mapping.get(e).toObject();
      obj["url"] = url;
    }

    ProgramVisitor::visit(e);
  }
};

JsonUrlAnnotator::JsonUrlAnnotator(json::Object& js_model, const JsonExportMapping& mapping)
  : serialized_model(js_model),
    json_mapping(mapping)
{

}

void JsonUrlAnnotator::annotate(const Model& model)
{
  for (auto d : model.documents)
    annotate_document(*d);

  if (model.program())
  {
    JsonUrlAnnotatorProgramVisitor pvisitor{ *this };
    static_cast<ProgramVisitor&>(pvisitor).visit(*model.program());
  }
}

void JsonUrlAnnotator::annotate_document(const dex::Document& doc)
{
  std::string url = get_url(doc);

  if (!url.empty())
  {
    json::Object obj = json_mapping.get(doc).toObject();
    obj["url"] = url;
  }
}

} // namespace dex
