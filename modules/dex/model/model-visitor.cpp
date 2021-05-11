// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model-visitor.h"

#include <dex/model/code-block.h>
#include <dex/model/display-math.h>

#include <cxx/class.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/macro.h>
#include <cxx/namespace.h>
#include <cxx/typedef.h>
#include <cxx/variable.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph.h>

#include <stdexcept>

namespace dex
{

void ProgramVisitor::visit(cxx::Program& prog)
{
  visit(static_cast<cxx::Entity&>(*prog.globalNamespace()));

  for (auto m : prog.macros)
    visit(*m);
}

void ProgramVisitor::visit(cxx::Entity& e)
{
  dispatch(e);
}


void ProgramVisitor::dispatch(cxx::Entity& e)
{
  switch (e.kind())
  {
  case cxx::NodeKind::Class:
    return visit(static_cast<cxx::Class&>(e));
  case cxx::NodeKind::Enum:
    return visit(static_cast<cxx::Enum&>(e));
  case cxx::NodeKind::EnumValue:
    return visit(static_cast<cxx::EnumValue&>(e));
  case cxx::NodeKind::Function:
    return visit(static_cast<cxx::Function&>(e));
  case cxx::NodeKind::FunctionParameter:
    return visit(static_cast<cxx::FunctionParameter&>(e));
  case cxx::NodeKind::Namespace:
    return visit(static_cast<cxx::Namespace&>(e));
  case cxx::NodeKind::Typedef:
    return visit(static_cast<cxx::Typedef&>(e));
  case cxx::NodeKind::Variable:
    return visit(static_cast<cxx::Variable&>(e));
  case cxx::NodeKind::Macro:
    return visit(static_cast<cxx::Macro&>(e));
  default: 
    break;
  }
}


void ProgramVisitor::visit(cxx::Namespace& ns)
{
  if (!ns.entities.empty())
  {
    for (size_t i(0); i < ns.entities.size(); ++i)
    {
      visit(*ns.entities.at(i));
    }
  }
}

void ProgramVisitor::visit(cxx::Class& cla)
{
  if (!cla.members.empty())
  {
    for (size_t i(0); i < cla.members.size(); ++i)
    {
      visit(*cla.members.at(i));
    }
  }
}

void ProgramVisitor::visit(cxx::Enum& en)
{
  if (!en.values.empty())
  {
    for (size_t i(0); i < en.values.size(); ++i)
    {
      visit(*en.values.at(i));
    }
  }
}

void ProgramVisitor::visit(cxx::EnumValue& ev)
{

}

void ProgramVisitor::visit(cxx::Function& f)
{
  if (!f.parameters.empty())
  {
    for (size_t i(0); i < f.parameters.size(); ++i)
    {
      visit(*f.parameters.at(i));
    }
  }
}

void ProgramVisitor::visit(cxx::FunctionParameter& fp)
{

}

void ProgramVisitor::visit(cxx::Variable& v)
{

}

void ProgramVisitor::visit(cxx::Typedef& t)
{

}

void ProgramVisitor::visit(cxx::Macro& m)
{

}

void DocumentVisitor::visitDocument(const dex::Document& doc)
{
  for (auto n : doc.nodes)
    visitNode(*n);
}

void DocumentVisitor::visitNode(dex::DocumentNode& n)
{
  dispatch(n);

  visitAll(n.childNodes());
}

void DocumentVisitor::dispatch(dex::DocumentNode& n)
{
  if (n.is<dex::Image>())
    visit(static_cast<dex::Image&>(n));
  else if (n.is<dex::List>())
    visit(static_cast<dex::List&>(n));
  else if (n.is<dex::ListItem>())
    visit(static_cast<dex::ListItem&>(n));
  else if (n.is<dex::Paragraph>())
    visit(static_cast<dex::Paragraph&>(n));
  else if (n.is<dex::BeginSince>())
    visit(static_cast<dex::BeginSince&>(n));
  else if (n.is<dex::EndSince>())
    visit(static_cast<dex::EndSince&>(n));
  else if (n.is<dex::FrontMatter>())
    visit(static_cast<dex::FrontMatter&>(n));
  else if (n.is<dex::MainMatter>())
    visit(static_cast<dex::MainMatter&>(n));
  else if (n.is<dex::BackMatter>())
    visit(static_cast<dex::BackMatter&>(n));
  else if (n.is<dex::Sectioning>())
    visit(static_cast<dex::Sectioning&>(n));
  else if (n.is<dex::DisplayMath>())
    visit(static_cast<dex::DisplayMath&>(n));
  else if (n.is<dex::GroupTable>())
    visit(static_cast<dex::GroupTable&>(n));
  else if (n.is<dex::CodeBlock>())
    visit(static_cast<dex::CodeBlock&>(n));
  else if (n.is<dex::TableOfContents>())
    visit(static_cast<dex::TableOfContents&>(n));
  else if (n.is<dex::Index>())
    visit(static_cast<dex::Index&>(n));
}

void DocumentVisitor::visitAll(const DomNodeList& nodes)
{
  for (auto child : nodes)
    visitNode(*child);
}

void DocumentVisitor::visit(dex::Image& img)
{

}

void DocumentVisitor::visit(dex::List& l)
{

}

void DocumentVisitor::visit(dex::ListItem& li)
{

}

void DocumentVisitor::visit(dex::Paragraph& par)
{

}

void DocumentVisitor::visit(dex::BeginSince& bsince)
{

}

void DocumentVisitor::visit(dex::EndSince& esince)
{

}

void DocumentVisitor::visit(dex::DisplayMath& math)
{

}

void DocumentVisitor::visit(dex::GroupTable& table)
{

}

void DocumentVisitor::visit(dex::CodeBlock& codeblock)
{

}

void DocumentVisitor::visit(dex::FrontMatter& fm)
{

}

void DocumentVisitor::visit(dex::MainMatter& mm)
{

}

void DocumentVisitor::visit(dex::BackMatter& bm)
{

}

void DocumentVisitor::visit(dex::Sectioning& section)
{

}

void DocumentVisitor::visit(dex::TableOfContents& toc)
{

}

void DocumentVisitor::visit(dex::Index& idx)
{

}

} // namespace dex
