// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
#define DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H

#include "dex/output/json-annotator.h"

namespace dex
{

class DEX_OUTPUT_API JsonUrlAnnotator : public JsonAnnotator
{
protected:
  void visit_entity(const cxx::Entity& e, json::Object& obj) override;

private:
  virtual std::string get_url(const cxx::Entity& e) const = 0;
};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_JSON_OUTPUT_ANNOTATOR_H
