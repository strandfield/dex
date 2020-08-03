// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_EXPORT_H
#define DEX_OUTPUT_JSON_EXPORT_H

#include "dex/dex-output.h"

#include "dex/model/model.h"
#include "dex/model/model-visitor.h"

#include <dom/paragraph.h>

#include <json-toolkit/json.h>

#include <unordered_map>

namespace dom
{
class Image;
class List;
class ListItem;
} // namespace dom

namespace cxx
{
class SourceLocation;
} // namespace cxx

namespace dex
{

struct DEX_OUTPUT_API JsonExportMapping
{
  std::unordered_map<std::shared_ptr<json::details::Node>, std::shared_ptr<const cxx::Entity>> backward;
  std::unordered_map<const cxx::Entity*, json::Json> forward;

  json::Json get(const cxx::Entity& n) const
  {
    return forward.at(&n);
  }

  std::shared_ptr<const cxx::Entity> get(const json::Json& obj) const
  {
    return backward.at(obj.impl());
  }

  void bind(const cxx::Entity& e, const json::Json& o)
  {
    backward[o.impl()] = e.shared_from_this();
    forward[&e] = o;
  }
};

class DEX_OUTPUT_API JsonExport : public ModelVisitor
{
public:
  JsonExport(const Model& model);

  json::Object result;
  JsonExportMapping mapping;

  static json::Object serialize(const Model& model);

protected:

  void beginVisitArray(const char* name) override;
  void endVisitArray() override;

  void visit_domnode(const dom::Node& n) override;
  void visit_domimage(const dom::Image& img) override;
  void visit_domlist(const dom::List& l) override;
  void visit_domlistitem(const dom::ListItem& li) override;
  void visit_domparagraph(const dom::Paragraph& par) override;
  void visit_displaymath(const dex::DisplayMath& math) override;

  void visit_program(const dex::Program& prog) override;
  void visit_entity(const cxx::Entity& e) override;
  void visit_namespace(const cxx::Namespace& ns) override;
  void visit_class(const cxx::Class& cla) override;
  void visit_enum(const cxx::Enum& en) override;
  void visit_enumvalue(const cxx::EnumValue& ev) override;
  void visit_function(const cxx::Function& f) override;
  void visit_functionparameter(const cxx::FunctionParameter& fp) override;
  void visit_variable(const cxx::Variable& v) override;
  void visit_typedef(const cxx::Typedef& t) override;
  void visit_macro(const cxx::Macro& m) override;

  void visit_entitydocumentation(const EntityDocumentation& edoc) override;

  void visit_manual(const Manual& man) override;
  void visit_sectioning(const Sectioning& sec) override;

protected:
  static void write_location(json::Object& obj, const cxx::SourceLocation& loc);

protected:
  json::Object& object();

private:
  friend class RAIIJsonExportContext;
  struct JsonStacks
  {
    std::vector<json::Object> objects;
    std::vector<json::Array> arrays;
  };
  JsonStacks m_json_stacks;
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_EXPORT_H
