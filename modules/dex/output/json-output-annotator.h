// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
#define DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H

#include "dex/output/json-mapping.h"

#include "dex/model/model-visitor.h"

#include <json-toolkit/json.h>

namespace dex
{

class DEX_OUTPUT_API JsonUrlAnnotator
{
public:
  json::Object& serialized_model;
  const JsonExportMapping& json_mapping;

public:
  JsonUrlAnnotator(json::Object& js_model, const JsonExportMapping& mapping);

  void annotate(const Model& model);

  virtual std::string get_url(const cxx::Entity& e) const = 0;
  virtual std::string get_url(const dex::Document& doc) const = 0;

protected:
  void annotate_document(const dex::Document& man);
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
