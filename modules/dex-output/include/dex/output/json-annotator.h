// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_ANNOTATOR_H
#define DEX_OUTPUT_JSON_ANNOTATOR_H

#include "dex/output/json-visitor.h"

namespace dex
{

class DEX_OUTPUT_API JsonAnnotator : public JsonVisitor
{
public:

  void annotate(const cxx::Program& prog, json::Object& obj);
};

class DEX_OUTPUT_API JsonPathAnnotator : public JsonAnnotator
{
public:

  static std::vector<std::variant<size_t, std::string>> parse(const std::string& path);

protected:
  void visit_entity(const cxx::Entity& e, json::Object& obj) override;

private:
  std::string build_path() const;
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_ANNOTATOR_H
