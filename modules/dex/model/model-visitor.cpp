// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model-visitor.h"

#include <dex/model/code-block.h>
#include <dex/model/display-math.h>

#include <stdexcept>

namespace dex
{

void ProgramVisitor::visit(dex::Program& prog)
{
  visit(static_cast<dex::Entity&>(*prog.globalNamespace()));

  for (auto m : prog.macros)
    visit(*m);
}

void ProgramVisitor::visit(dex::Entity& e)
{
  dispatch(e);
}


void ProgramVisitor::dispatch(dex::Entity& e)
{
  switch (e.kind())
  {
  case model::Kind::Class:
    return visit(static_cast<dex::Class&>(e));
  case model::Kind::Enum:
    return visit(static_cast<dex::Enum&>(e));
  case model::Kind::EnumValue:
    return visit(static_cast<dex::EnumValue&>(e));
  case model::Kind::Function:
    return visit(static_cast<dex::Function&>(e));
  case model::Kind::FunctionParameter:
    return visit(static_cast<dex::FunctionParameter&>(e));
  case model::Kind::Namespace:
    return visit(static_cast<dex::Namespace&>(e));
  case model::Kind::Typedef:
    return visit(static_cast<dex::Typedef&>(e));
  case model::Kind::Variable:
    return visit(static_cast<dex::Variable&>(e));
  case model::Kind::Macro:
    return visit(static_cast<dex::Macro&>(e));
  default: 
    break;
  }
}


void ProgramVisitor::visit(dex::Namespace& ns)
{
  if (!ns.entities.empty())
  {
    for (size_t i(0); i < ns.entities.size(); ++i)
    {
      visit(*ns.entities.at(i));
    }
  }
}

void ProgramVisitor::visit(dex::Class& cla)
{
  if (!cla.members.empty())
  {
    for (size_t i(0); i < cla.members.size(); ++i)
    {
      visit(*cla.members.at(i));
    }
  }
}

void ProgramVisitor::visit(dex::Enum& en)
{
  if (!en.values.empty())
  {
    for (size_t i(0); i < en.values.size(); ++i)
    {
      visit(*en.values.at(i));
    }
  }
}

void ProgramVisitor::visit(dex::EnumValue& ev)
{

}

void ProgramVisitor::visit(dex::Function& f)
{
  if (!f.parameters.empty())
  {
    for (size_t i(0); i < f.parameters.size(); ++i)
    {
      visit(*f.parameters.at(i));
    }
  }
}

void ProgramVisitor::visit(dex::FunctionParameter& fp)
{

}

void ProgramVisitor::visit(dex::Variable& v)
{

}

void ProgramVisitor::visit(dex::Typedef& t)
{

}

void ProgramVisitor::visit(dex::Macro& m)
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
