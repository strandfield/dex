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

  void annotate(const Model& model, json::Object& obj);
};

class DEX_OUTPUT_API JsonPathAnnotator
{
public:

  static std::vector<std::variant<size_t, std::string>> parse(const std::string& path);

  void annotate(json::Object& obj);

protected:
  void visit(json::Json val);

private:
  std::string build_path() const;

private:
  std::vector<std::variant<size_t, std::string>> m_stack;
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_ANNOTATOR_H
