// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model-visitor.h"

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

struct VisitorContext
{
  std::vector<Model::PathElement>* stack;

  VisitorContext(std::vector<Model::PathElement>* s, size_t n)
    : stack(s)
  {
    stack->back().index = n;
  }

  VisitorContext(std::vector<Model::PathElement>* s, std::string name)
    : stack(s)
  {
    stack->push_back(std::move(name));
  }

  ~VisitorContext()
  {
    if (stack->back().index != std::numeric_limits<size_t>::max())
      stack->back().index = std::numeric_limits<size_t>::max();
    else
      stack->pop_back();
  }
};

void ModelVisitor::visit(const Model& model)
{
  m_model = &model;

  if (model.program())
  {
    VisitorContext context{ &m_stack, "program" };

    visit_program(*model.program());
  }

  if (!model.manuals().empty())
  {
    VisitorContext context{ &m_stack, "manuals" };
    for (size_t i(0); i < model.manuals().size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };
      visit_domnode(*model.manuals().at(i));
    }
  }
}

const Model& ModelVisitor::model() const
{
  return *m_model;
}

const std::vector<Model::PathElement>& ModelVisitor::stack() const
{
  return m_stack;
}

Model::Path ModelVisitor::path() const
{
  return m_stack;
}

void ModelVisitor::visit_domnode(const dom::Node& n)
{
  if (n.is<dom::Image>())
    visit_domimage(static_cast<const dom::Image&>(n));
  else if (n.is<dom::List>())
    visit_domlist(static_cast<const dom::List&>(n));
  else if (n.is<dom::ListItem>())
    visit_domlistitem(static_cast<const dom::ListItem&>(n));
  else if (n.is<dom::Paragraph>())
    visit_domparagraph(static_cast<const dom::Paragraph&>(n));
  else if (n.is<dex::Manual>())
    visit_manual(static_cast<const dex::Manual&>(n));
  else if (n.is<dex::Sectioning>())
    visit_sectioning(static_cast<const dex::Sectioning&>(n));
  else if (n.is<dex::DisplayMath>())
    visit_displaymath(static_cast<const dex::DisplayMath&>(n));
}

void ModelVisitor::visit_domimage(const dom::Image& /* img */)
{

}

void ModelVisitor::visit_domlist(const dom::List& l)
{
  if (!l.items.empty())
  {
    VisitorContext context{ &m_stack, "items" };

    for (size_t i(0); i < l.items.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_domnode(*l.items.at(i));
    }
  }
}

void ModelVisitor::visit_domlistitem(const dom::ListItem& li)
{
  if (!li.content.empty())
  {
    VisitorContext context{ &m_stack, "content" };

    for (size_t i(0); i < li.content.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_domnode(*li.content.at(i));
    }
  }
}

void ModelVisitor::visit_domparagraph(const dom::Paragraph& /* par */)
{

}

void ModelVisitor::visit_displaymath(const dex::DisplayMath& /* math */)
{

}

void ModelVisitor::visit_program(const cxx::Program& prog)
{
  {
    VisitorContext context{ &m_stack, "global_namespace" };
    visit_entity(*prog.globalNamespace());
  }

  {
    VisitorContext context{ &m_stack, "macros" };
    
    for (size_t i(0); i < prog.macros.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_entity(*prog.macros.at(i));
    }
  }
}

void ModelVisitor::visit_entity(const cxx::Entity& e)
{
  if (e.is<cxx::Class>())
    visit_class(static_cast<const cxx::Class&>(e));
  else if (e.is<cxx::Enum>())
    visit_enum(static_cast<const cxx::Enum&>(e));
  else if (e.is<cxx::EnumValue>())
    visit_enumvalue(static_cast<const cxx::EnumValue&>(e));
  else if (e.is<cxx::Function>())
    visit_function(static_cast<const cxx::Function&>(e));
  else if (e.is<cxx::FunctionParameter>())
    visit_functionparameter(static_cast<const cxx::FunctionParameter&>(e));
  else if (e.is<cxx::Macro>())
    visit_macro(static_cast<const cxx::Macro&>(e));
  else if (e.is<cxx::Namespace>())
    visit_namespace(static_cast<const cxx::Namespace&>(e));
  else if (e.is<cxx::Typedef>())
    visit_typedef(static_cast<const cxx::Typedef&>(e));
  else if (e.is<cxx::Variable>())
    visit_variable(static_cast<const cxx::Variable&>(e));

  if (e.documentation)
  {
    auto edoc = std::dynamic_pointer_cast<EntityDocumentation>(e.documentation);

    if (edoc)
    {
      VisitorContext context{ &m_stack, "documentation" };
      visit_entitydocumentation(*edoc);
    }
  }
}

void ModelVisitor::visit_namespace(const cxx::Namespace& ns)
{
  if (!ns.entities.empty())
  {
    VisitorContext context{ &m_stack, "entities" };

    for (size_t i(0); i < ns.entities.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_entity(*ns.entities.at(i));
    }
  }
}

void ModelVisitor::visit_class(const cxx::Class& cla)
{
  if (!cla.members.empty())
  {
    VisitorContext context{ &m_stack, "members" };

    for (size_t i(0); i < cla.members.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_entity(*cla.members.at(i));
    }
  }
}

void ModelVisitor::visit_enum(const cxx::Enum& en)
{
  if (!en.values.empty())
  {
    VisitorContext context{ &m_stack, "values" };

    for (size_t i(0); i < en.values.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_entity(*en.values.at(i));
    }
  }
}

void ModelVisitor::visit_enumvalue(const cxx::EnumValue& /* ev */)
{

}

void ModelVisitor::visit_function(const cxx::Function& f)
{
  if (!f.parameters.empty())
  {
    VisitorContext context{ &m_stack, "parameters" };

    for (size_t i(0); i < f.parameters.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_entity(*f.parameters.at(i));
    }
  }
}

void ModelVisitor::visit_functionparameter(const cxx::FunctionParameter& /* fp */)
{

}

void ModelVisitor::visit_variable(const cxx::Variable& /* v */)
{

}

void ModelVisitor::visit_typedef(const cxx::Typedef& /* t */)
{

}

void ModelVisitor::visit_macro(const cxx::Macro& /* m */)
{

}

void ModelVisitor::visit_entitydocumentation(const EntityDocumentation& edoc)
{
  if (!edoc.description().empty())
  {
    VisitorContext context{ &m_stack, "description" };
    for (size_t i(0); i < edoc.description().size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_domnode(*edoc.description().at(i));
    }
  }
}

void ModelVisitor::visit_manual(const dex::Manual& man)
{
  if (!man.content.empty())
  {
    VisitorContext context{ &m_stack, "content" };
    for (size_t i(0); i < man.content.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_domnode(*man.content.at(i));
    }
  }
}

void ModelVisitor::visit_sectioning(const dex::Sectioning& section)
{
  if (!section.content.empty())
  {
    VisitorContext context{ &m_stack, "content" };
    for (size_t i(0); i < section.content.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      visit_domnode(*section.content.at(i));
    }
  }
}

} // namespace dex
