// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid/liquid-wrapper.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/paragraph-annotations.h"
#include "dex/model/since.h"

namespace dex
{

liquid::Value to_liquid(const Since& since)
{
  return since.version();
}

template <typename T>
liquid::Value to_liquid(const std::optional<T>& val)
{
  return val.has_value() ? to_liquid(val.value()) : liquid::Value();
}

class LiquidPropertyVisitorEntity : public ProgramVisitor
{
public:
  const std::string& propertyName;
  liquid::Value result;

public:
  LiquidPropertyVisitorEntity(const std::string& pname)
    : propertyName(pname)
  {

  }

  void setval(liquid::Value val)
  {
    result = val;
  }

  void visit(dex::Entity& e) override
  {
    if (propertyName == "name")
      return setval(e.name);
    else if (propertyName == "parent")
      return setval(to_liquid(e.parent()));
    else if (propertyName == "type")
      return setval(e.className());
    else if (propertyName == "accessibility")
      return setval(to_string(e.getAccessSpecifier()));
    else if (propertyName == "brief")
      return setval(to_liquid(e.brief));
    else if (propertyName == "since")
      return setval(to_liquid(e.since));
    else if (propertyName == "description")
      return setval(e.description ? to_liquid(e.description->nodes) : liquid::Value());
    else
      return dispatch(e);
  }

  void visit(dex::Namespace& ns) override
  {
    if (propertyName == "entities")
      return setval(to_liquid(ns.entities));
  }

  void visit(dex::Class& cla) override
  {
    if (propertyName == "members")
      return setval(to_liquid(cla.members));
  }

  void visit(dex::Enum& en) override
  {
    if (propertyName == "values")
      return setval(to_liquid(en.values));
  }

  void visit(dex::EnumValue& ev) override
  {
    if (propertyName == "value")
      return setval(ev.value());
  }

  void visit(dex::Function& f) override
  {
    if (propertyName == "return_type")
      return setval(f.return_type.type);
    else if (propertyName == "returns")
      return setval(to_liquid(f.return_type.brief));
    else if (propertyName == "specifiers")
      return setval(f.specifiersList());
    else if (propertyName == "parameters")
      return setval(to_liquid(f.parameters));
  }

  void visit(dex::FunctionParameter& fp) override
  {
    if (propertyName == "ptype")
      return setval(fp.type);
  }

  void visit(dex::Variable& v) override
  {
    if (propertyName == "vartype")
      return setval(v.type());
  }

  void visit(dex::Typedef& t) override
  {
    if (propertyName == "typedef")
      return setval(t.type);
  }

  void visit(dex::Macro& m) override
  {
    if (propertyName == "parameters")
      return setval(to_liquid(m.parameters));
  }
};

class LiquidPropertyVisitorDom : public DocumentVisitor
{
public:
  const std::string& propertyName;
  liquid::Value result;

public:
  LiquidPropertyVisitorDom(const std::string& pname)
    : propertyName(pname)
  {

  }

  void setval(liquid::Value val)
  {
    result = val;
  }

  void visitNode(dex::DocumentNode& n) override
  {
    if (propertyName == "parent")
      return setval(to_liquid(n.weak_parent.lock()));
    else
      return dispatch(n);
  }

  void visit(dex::Image& img) override
  {
    if (propertyName == "width")
      return setval(img.width);
    else if (propertyName == "height")
      return setval(img.height);
    else if (propertyName == "src")
      return setval(img.src);
  }

  void visit(dex::List& l) override
  {

  }

  void visit(dex::ListItem& li) override
  {

  }

  void visit(dex::Paragraph& par) override
  {
    if (propertyName == "text")
      return setval(par.text());
    else if (propertyName == "metadata")
      return setval(to_liquid(par.metadata()));
  }

  void visit(dex::BeginSince& bsince) override
  {
    if (propertyName == "version")
      return setval(bsince.version);
  }

  void visit(dex::EndSince& esince) override
  {
    if (propertyName == "version")
      return setval(esince.beginsince.lock()->version);
  }

  void visit(dex::DisplayMath& math) override
  {
    if (propertyName == "source")
      return setval(math.source);
  }

  void visit(dex::GroupTable& table) override
  {

  }

  void visit(dex::CodeBlock& codeblock) override
  {
    if (propertyName == "lang")
      return setval(codeblock.lang);
    else if (propertyName == "code")
      return setval(codeblock.code);
  }


  void visit(dex::FrontMatter& fm) override
  {

  }

  void visit(dex::MainMatter& mm) override
  {

  }

  void visit(dex::BackMatter& bm) override
  {

  }

  void visit(dex::Sectioning& section) override
  {

  }

  void visit(dex::TableOfContents& toc) override
  {

  }

  void visit(dex::Index& idx) override
  {

  }
};

LiquidModel::LiquidModel(const std::shared_ptr<Model>& m)
  : model(m)
{

}

std::type_index LiquidModel::type_index() const
{
  return std::type_index(typeid(std::shared_ptr<Model>));
}

void* LiquidModel::data()
{
  return reinterpret_cast<void*>(&model);
}

bool LiquidModel::is_map() const
{
  return true;
}

std::set<std::string> LiquidModel::propertyNames() const
{
  return { "program", "documents" };
}

liquid::Value LiquidModel::property(const std::string& name) const
{
  if (name == "program")
  {
    return to_liquid(model->program());
  }
  else if (name == "documents")
  {
    return to_liquid(model->documents);
  }
  else if (name == "groups")
  {
    return to_liquid(model->groups.groups);
  }
  else
  {
    return {};
  }
}


LiquidModelObject::LiquidModelObject(std::shared_ptr<model::Object> obj)
  : object(obj)
{

}

std::type_index LiquidModelObject::type_index() const
{
  return std::type_index(typeid(std::shared_ptr<model::Object>));
}

void* LiquidModelObject::data()
{
  return &object;
}

bool LiquidModelObject::is_map() const
{
  return true;
}

std::set<std::string> LiquidModelObject::propertyNames() const
{
  // @TODO
  return {};
}

liquid::Value LiquidModelObject::property(const std::string& name) const
{
  if (!object)
  {
    return {};
  }
  else if (object->isProgramEntity())
  {
    LiquidPropertyVisitorEntity visitor{ name };
    visitor.visit(static_cast<dex::Entity&>(*object));
    return visitor.result;
  }
  else if (object->isDocumentNode())
  {
    if (object->is<dex::Document>())
    {
      dex::Document& doc = static_cast<dex::Document&>(*object);

      if (name == "doctype")
        return doc.doctype;
      else if (name == "type")
        return doc.className();
      else if (name == "title")
        return doc.title;
      else if (name == "content")
        return to_liquid(doc.nodes);
    }
    else
    {
      LiquidPropertyVisitorDom visitor{ name };
      visitor.visitNode(static_cast<dex::DocumentNode&>(*object));
      return visitor.result;
    }
  }
  else if (object->is<dex::Program>())
  {
    auto& prog = static_cast<dex::Program&>(*object);

    if (name == "global_namespace")
      return to_liquid(prog.global_namespace);
    else if (name == "macros")
      return to_liquid(prog.macros);
  }

  return {};
}

} // namespace dex
