// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_EXPORT_H
#define DEX_OUTPUT_JSON_EXPORT_H

#include "dex/dex-output.h"

#include "dex/model/model-visitor.h"

namespace dex
{

class DEX_OUTPUT_API JsonExporter
{
public:
  const Model& model;

public:
  explicit JsonExporter(const Model& m);

  static json::Object serialize(const Model& model);
  json::Object serialize();

protected:

  json::Object serializeGroup(const Group& group);

};

class JsonDocumentSerializer : public DocumentVisitor
{
public:
  json::Object result;

public:
  JsonDocumentSerializer()
  {

  }

  json::Object serialize(dex::Document& doc);
  json::Object serialize(dex::DocumentNode& n);

  json::Array serializeArray(const DomNodeList& nodes);

private:

  void visitNode(dex::DocumentNode& n) override;

  void visit(dex::Image& img) override;
  void visit(dex::List& l) override;
  void visit(dex::ListItem& li) override;
  void visit(dex::Paragraph& par) override;
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

public:
  JsonProgramSerializer()
  {

  }

  json::Object serialize(dex::Program& prog);
  json::Object serialize(dex::Entity& e);

  static std::string path(dex::Entity& e, dex::Program& prog);

private:
  json::Array serializeArray(const std::vector<std::shared_ptr<dex::Entity>>& nodes);

  template<typename T>
  json::Array serializeArray(const std::vector<std::shared_ptr<T>>& nodes)
  {
    json::Array res;

    for (auto n : nodes)
      res.push(serialize(static_cast<dex::Entity&>(*n)));

    return res;
  }

  json::Array serializeRelatedMembers(dex::RelatedNonMembers& rnm, dex::Program& prog);

  void write_documentation(dex::Entity& e);

  void visit(dex::Entity& e) override;

  void visit(dex::Namespace& ns) override;
  void visit(dex::Class& cla) override;
  void visit(dex::Enum& en) override;
  void visit(dex::EnumValue& ev) override;
  void visit(dex::Function& f) override;
  void visit(dex::FunctionParameter& fp) override;
  void visit(dex::Variable& v) override;
  void visit(dex::Typedef& t) override;
  void visit(dex::Macro& m) override;
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_EXPORT_H
