// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_EXPORT_H
#define DEX_OUTPUT_JSON_EXPORT_H

#include "dex/dex-output.h"

#include "dex/output/json-mapping.h"

#include "dex/model/model-visitor.h"

#include <dom/paragraph.h>

namespace dex
{

class DEX_OUTPUT_API JsonExporter
{
public:
  const Model& model;

public:
  explicit JsonExporter(const Model& m);

  JsonExportMapping mapping;

  static json::Object serialize(const Model& model);
  json::Object serialize();

protected:

  json::Object serializeGroup(const Group& group);

};

class JsonDocumentSerializer : public DocumentVisitor
{
public:
  json::Object result;
  JsonExportMapping& mapping;

public:
  JsonDocumentSerializer(JsonExportMapping& map)
    : mapping(map)
  {

  }

  json::Object serialize(dex::Document& doc);
  json::Object serialize(dom::Node& n);

  json::Array serializeArray(const dom::NodeList& nodes);

private:

  void visitNode(dom::Node& n) override;

  void visit(dom::Image& img) override;
  void visit(dom::List& l) override;
  void visit(dom::ListItem& li) override;
  void visit(dom::Paragraph& par) override;
  void visit(dex::BeginSince& bsince) override;
  void visit(dex::EndSince& esince) override;
  void visit(dex::DisplayMath& math) override;
  void visit(dex::GroupTable& table) override;
  void visit(dex::CodeBlock& codeblock) override;

  void visit(dex::FrontMatter& fm) override;
  void visit(dex::MainMatter& mm) override;
  void visit(dex::BackMatter& bm) override;
  void visit(dex::Sectioning& section) override;
  void visit(dex::TableOfContents& toc) override;
  void visit(dex::Index& idx) override;
};

class JsonProgramSerializer : public ProgramVisitor
{
public:
  json::Object result;
  JsonExportMapping& mapping;

public:
  JsonProgramSerializer(JsonExportMapping& map)
    : mapping(map)
  {

  }

  json::Object serialize(dex::Program& prog);
  json::Object serialize(cxx::Entity& e);

  static std::string path(cxx::Entity& e, dex::Program& prog);

private:
  json::Array serializeArray(const std::vector<std::shared_ptr<cxx::Entity>>& nodes);

  template<typename T>
  json::Array serializeArray(const std::vector<std::shared_ptr<T>>& nodes)
  {
    json::Array res;

    for (auto n : nodes)
      res.push(serialize(static_cast<cxx::Entity&>(*n)));

    return res;
  }

  json::Array serializeRelatedMembers(dex::RelatedNonMembers& rnm, dex::Program& prog);

  void write_documentation(cxx::Entity& e);

  void visit(cxx::Entity& e) override;

  void visit(cxx::Namespace& ns) override;
  void visit(cxx::Class& cla) override;
  void visit(cxx::Enum& en) override;
  void visit(cxx::EnumValue& ev) override;
  void visit(cxx::Function& f) override;
  void visit(cxx::FunctionParameter& fp) override;
  void visit(cxx::Variable& v) override;
  void visit(cxx::Typedef& t) override;
  void visit(cxx::Macro& m) override;
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_EXPORT_H
