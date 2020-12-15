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

  if (!model.documents.empty())
  {
    RaiiArrayVisitor state_updater{ this, "documents" };

    for (size_t i(0); i < model.documents.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };
      visit_domnode(*model.documents.at(i));
    }
  }

  if (!model.groups.groups.empty())
  {
    RaiiArrayVisitor state_updater{ this, "groups" };

    for (size_t i(0); i < model.groups.groups.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };
      visit_group(*model.groups.groups.at(i));
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
  else if (n.is<dex::BeginSince>())
    visit_beginsince(static_cast<const dex::BeginSince&>(n));
  else if (n.is<dex::EndSince>())
    visit_endsince(static_cast<const dex::EndSince&>(n));
  else if (n.is<dom::Document>())
    visit_document(static_cast<const dex::Document&>(n));
  else if (n.is<dex::FrontMatter>())
    visit_frontmatter(static_cast<const dex::FrontMatter&>(n));
  else if (n.is<dex::MainMatter>())
    visit_mainmatter(static_cast<const dex::MainMatter&>(n));
  else if (n.is<dex::BackMatter>())
    visit_backmatter(static_cast<const dex::BackMatter&>(n));
  else if (n.is<dex::Sectioning>())
    visit_sectioning(static_cast<const dex::Sectioning&>(n));
  else if (n.is<dex::DisplayMath>())
    visit_displaymath(static_cast<const dex::DisplayMath&>(n));
  else if (n.is<dex::GroupTable>())
    visit_grouptable(static_cast<const dex::GroupTable&>(n));
  else if (n.is<dex::CodeBlock>())
    visit_codeblock(static_cast<const dex::CodeBlock&>(n));
  else if (n.is<dex::TableOfContents>())
    visit_tableofcontents(static_cast<const dex::TableOfContents&>(n));  
  else if (n.is<dex::Index>())
    visit_index(static_cast<const dex::Index&>(n));
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

void ModelVisitor::visit_beginsince(const dex::BeginSince& /* bsince */)
{

}

void ModelVisitor::visit_endsince(const dex::EndSince& /* esince */)
{

}

void ModelVisitor::visit_displaymath(const dex::DisplayMath& /* math */)
{

}

void ModelVisitor::visit_grouptable(const dex::GroupTable& /* table */)
{

}

void ModelVisitor::visit_codeblock(const dex::CodeBlock& /* codeblock */)
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
  if (edoc.description && !edoc.description->childNodes().empty())
  {
    RaiiArrayVisitor state_updater{ this, "description" };
    const auto& childnodes = edoc.description->childNodes();
    for (size_t i(0); i < childnodes.size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*childnodes.at(i));
    }
  }
}

void ModelVisitor::visit_document(const dex::Document& doc)
{
  if (!doc.childNodes().empty())
  {
    RaiiArrayVisitor state_updater{ this, "content" };
    for (size_t i(0); i < doc.childNodes().size(); ++i)
    {
      RaiiArrayElementVisitor inner_state_updater{ this, i };

      visit_domnode(*doc.childNodes().at(i));
    }
  }
}

void ModelVisitor::visit_frontmatter(const dex::FrontMatter& /* fm */)
{

}

void ModelVisitor::visit_mainmatter(const dex::MainMatter& /* mm */)
{

}

void ModelVisitor::visit_backmatter(const dex::BackMatter& /* bm */)
{

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

void ModelVisitor::visit_tableofcontents(const dex::TableOfContents& /* toc */)
{

}

void ModelVisitor::visit_index(const dex::Index& /* idx */)
{

}

void ModelVisitor::visit_group(const dex::Group& /* group */)
{

}

void ProgramVisitor::visit(cxx::Program& prog)
{
  visit(*prog.globalNamespace());

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

} // namespace dex
