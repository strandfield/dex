// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODELVISITOR_H
#define DEX_MODEL_MODELVISITOR_H

#include "dex/model/model.h"

namespace dom
{
class Image;
class List;
class ListItem;
class Paragraph;
} // namespace dom

namespace cxx
{
class Entity;
class Class;
class Documentation;
class Enum;
class EnumValue;
class Function;
class FunctionParameter;
class Macro;
class Namespace;
class Typedef;
class Variable;
} // namespace cxx

namespace dex
{

class BackMatter;
class BeginSince;
class CodeBlock;
class DisplayMath;
class EndSince;
class FrontMatter;
class Index;
class MainMatter;
class TableOfContents;

class Program;

class DEX_MODEL_API ModelVisitor
{
public:

  void visit(const Model& model);

protected:
  const Model& model() const;
  const std::vector<Model::PathElement>& stack() const;
  const Model::Path& path() const;

protected:

  friend class ModelVisitorHelper;

  virtual void beginVisitObject(const char* name);
  virtual void endVisitObject();
  virtual void beginVisitArray(const char* name);
  virtual void endVisitArray();

  virtual void visit_domnode(const dom::Node& n);
  virtual void visit_domimage(const dom::Image& img);
  virtual void visit_domlist(const dom::List& l);
  virtual void visit_domlistitem(const dom::ListItem& li);
  virtual void visit_domparagraph(const dom::Paragraph& par);
  virtual void visit_beginsince(const dex::BeginSince& bsince);
  virtual void visit_endsince(const dex::EndSince& esince);
  virtual void visit_displaymath(const dex::DisplayMath& math);
  virtual void visit_grouptable(const dex::GroupTable& table);
  virtual void visit_codeblock(const dex::CodeBlock& codeblock);

  virtual void visit_program(const dex::Program& prog);
  virtual void visit_entity(const cxx::Entity& e);
  virtual void visit_namespace(const cxx::Namespace& ns);
  virtual void visit_class(const cxx::Class& cla);
  virtual void visit_enum(const cxx::Enum& en);
  virtual void visit_enumvalue(const cxx::EnumValue& ev);
  virtual void visit_function(const cxx::Function& f);
  virtual void visit_functionparameter(const cxx::FunctionParameter& fp);
  virtual void visit_variable(const cxx::Variable& v);
  virtual void visit_typedef(const cxx::Typedef& t);
  virtual void visit_macro(const cxx::Macro& m);

  virtual void visit_entitydocumentation(const EntityDocumentation& edoc);

  virtual void visit_document(const dex::Document& doc);
  virtual void visit_frontmatter(const dex::FrontMatter& fm);
  virtual void visit_mainmatter(const dex::MainMatter& mm);
  virtual void visit_backmatter(const dex::BackMatter& bm);
  virtual void visit_sectioning(const dex::Sectioning& section);
  virtual void visit_tableofcontents(const dex::TableOfContents& toc);
  virtual void visit_index(const dex::Index& idx);

  virtual void visit_group(const dex::Group& group);

private:
  const Model* m_model = nullptr;
  std::vector<Model::PathElement> m_stack;
};

class DEX_MODEL_API ProgramVisitor
{
public:

  void visit(cxx::Program& prog);

protected:
  virtual void visit(cxx::Entity& e);

  void dispatch(cxx::Entity& e);

protected:
  virtual void visit(cxx::Namespace& ns);
  virtual void visit(cxx::Class& cla);
  virtual void visit(cxx::Enum& en);
  virtual void visit(cxx::EnumValue& ev);
  virtual void visit(cxx::Function& f);
  virtual void visit(cxx::FunctionParameter& fp);
  virtual void visit(cxx::Variable& v);
  virtual void visit(cxx::Typedef& t);
  virtual void visit(cxx::Macro& m);
};

} // namespace dex

#endif // DEX_MODEL_MODELVISITOR_H
