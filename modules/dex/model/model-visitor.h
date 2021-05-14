// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODELVISITOR_H
#define DEX_MODEL_MODELVISITOR_H

#include "dex/model/model.h"

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

class Program;

class DEX_MODEL_API ProgramVisitor
{
public:

  void visit(dex::Program& prog);

protected:
  virtual void visit(dex::Entity& e);

  void dispatch(dex::Entity& e);

protected:
  virtual void visit(dex::Namespace& ns);
  virtual void visit(dex::Class& cla);
  virtual void visit(dex::Enum& en);
  virtual void visit(dex::EnumValue& ev);
  virtual void visit(dex::Function& f);
  virtual void visit(dex::FunctionParameter& fp);
  virtual void visit(dex::Variable& v);
  virtual void visit(dex::Typedef& t);
  virtual void visit(dex::Macro& m);
};

class DEX_MODEL_API DocumentVisitor
{
public:

  void visitDocument(const dex::Document& doc);

protected:
  virtual void visitNode(dex::DocumentNode& n);

  void dispatch(dex::DocumentNode& n);

protected:
  void visitAll(const DomNodeList& nodes);

protected:
  virtual void visit(dex::Image& img);
  virtual void visit(dex::List& l);
  virtual void visit(dex::ListItem& li);
  virtual void visit(dex::Paragraph& par);
  virtual void visit(dex::BeginSince& bsince);
  virtual void visit(dex::EndSince& esince);
  virtual void visit(dex::DisplayMath& math);
  virtual void visit(dex::GroupTable& table);
  virtual void visit(dex::CodeBlock& codeblock);

  virtual void visit(dex::FrontMatter& fm);
  virtual void visit(dex::MainMatter& mm);
  virtual void visit(dex::BackMatter& bm);
  virtual void visit(dex::Sectioning& section);
  virtual void visit(dex::TableOfContents& toc);
  virtual void visit(dex::Index& idx);
};

} // namespace dex

#endif // DEX_MODEL_MODELVISITOR_H
