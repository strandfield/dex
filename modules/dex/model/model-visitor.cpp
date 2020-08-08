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

class ModelVisitorHelper
{
public:
  ModelVisitor& visitor;

public:
  ModelVisitorHelper(ModelVisitor* v)
    : visitor(*v)
  {

  }

  std::vector<Model::PathElement>& stack()
  {
    return visitor.m_stack;
  }

  void beginVisitObject(const char* name)
  {
    visitor.beginVisitObject(name);
  }

  void endVisitObject()
  {
    visitor.endVisitObject();
  }

  void beginVisitArray(const char* name)
  {
    visitor.beginVisitArray(name);
  }

  void endVisitArray()
  {
    visitor.endVisitArray();
  }
};

class RaiiObjectVisitor : public ModelVisitorHelper
{
public:
  RaiiObjectVisitor(ModelVisitor* v, const char* name)
    : ModelVisitorHelper(v)
  {
    stack().push_back(std::string_view(name));
    beginVisitObject(name);
  }

  ~RaiiObjectVisitor()
  {
    endVisitObject();
    stack().pop_back();
  }
};

class RaiiArrayVisitor : public ModelVisitorHelper
{
public:
  RaiiArrayVisitor(ModelVisitor* v, const char* name)
    : ModelVisitorHelper(v)
  {
    stack().push_back(std::string_view(name));
    beginVisitArray(name);
  }

  ~RaiiArrayVisitor()
  {
    endVisitArray();
    stack().pop_back();
  }
};

class RaiiArrayElementVisitor : public ModelVisitorHelper
{
public:
  RaiiArrayElementVisitor(ModelVisitor* v, size_t n)
    : ModelVisitorHelper(v)
  {
    stack().back().index = n;
  }

  ~RaiiArrayElementVisitor()
  {
    stack().back().index = std::numeric_limits<size_t>::max();
  }
};

void ModelVisitor::visit(const Model& model)
{
  m_model = &model;

  if (model.program())
  {
    RaiiObjectVisitor state_updater{ this, "program" };

    visit_program(*model.program());
  }

  if (!model.manuals().empty())
  {
    RaiiArrayVisitor state_updater{ this, "manuals" };

    for (size_t i(0); i < model.manuals().size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };
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

const Model::Path& ModelVisitor::path() const
{
  return m_stack;
}

void ModelVisitor::beginVisitObject(const char* name)
{

}

void ModelVisitor::endVisitObject() 
{

}

void ModelVisitor::beginVisitArray(const char* name)
{

}

void ModelVisitor::endVisitArray()
{

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
    RaiiArrayVisitor state_updater{ this, "items" };

    for (size_t i(0); i < l.items.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*l.items.at(i));
    }
  }
}

void ModelVisitor::visit_domlistitem(const dom::ListItem& li)
{
  if (!li.content.empty())
  {
    RaiiArrayVisitor state_updater{ this, "content" };

    for (size_t i(0); i < li.content.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

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

void ModelVisitor::visit_program(const dex::Program& prog)
{
  {
    RaiiObjectVisitor state_updater{ this, "global_namespace" };
    visit_entity(*prog.globalNamespace());
  }

  if(!prog.macros.empty())
  {
    RaiiArrayVisitor state_updater{ this, "macros" };
    
    for (size_t i(0); i < prog.macros.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

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
      RaiiObjectVisitor state_updater{ this, "documentation" };
      visit_entitydocumentation(*edoc);
    }
  }
}

void ModelVisitor::visit_namespace(const cxx::Namespace& ns)
{
  if (!ns.entities.empty())
  {
    RaiiArrayVisitor state_updater{ this, "entities" };

    for (size_t i(0); i < ns.entities.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_entity(*ns.entities.at(i));
    }
  }
}

void ModelVisitor::visit_class(const cxx::Class& cla)
{
  if (!cla.members.empty())
  {
    RaiiArrayVisitor state_updater{ this, "members" };

    for (size_t i(0); i < cla.members.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_entity(*cla.members.at(i));
    }
  }
}

void ModelVisitor::visit_enum(const cxx::Enum& en)
{
  if (!en.values.empty())
  {
    RaiiArrayVisitor state_updater{ this, "values" };

    for (size_t i(0); i < en.values.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

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
    RaiiArrayVisitor state_updater{ this, "parameters" };

    for (size_t i(0); i < f.parameters.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

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
    RaiiArrayVisitor state_updater{ this, "description" };
    for (size_t i(0); i < edoc.description().size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*edoc.description().at(i));
    }
  }
}

void ModelVisitor::visit_manual(const dex::Manual& man)
{
  if (!man.content.empty())
  {
    RaiiArrayVisitor state_updater{ this, "content" };
    for (size_t i(0); i < man.content.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*man.content.at(i));
    }
  }
}

void ModelVisitor::visit_sectioning(const dex::Sectioning& section)
{
  if (!section.content.empty())
  {
    RaiiArrayVisitor state_updater{ this, "content" };
    for (size_t i(0); i < section.content.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*section.content.at(i));
    }
  }
}

} // namespace dex
