// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-visitor.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

#include <dom/list.h>

namespace dex
{

struct VisitorContext
{
  std::vector<std::variant<size_t, std::string>>* stack;

  VisitorContext(std::vector<std::variant<size_t, std::string>>* s, size_t n)
    : stack(s)
  {
    stack->push_back(n);
  }

  VisitorContext(std::vector<std::variant<size_t, std::string>>* s, std::string name)
    : stack(s)
  {
    stack->push_back(std::move(name));
  }

  ~VisitorContext()
  {
    stack->pop_back();
  }
};

void JsonVisitor::visit(const Model& model, json::Object& obj)
{
  if (model.program())
  {
    VisitorContext context{ &m_stack, "program" };

    json::Object prog = obj["program"].toObject();
    visit(*model.program(), prog);
  }
}

void JsonVisitor::visit(const cxx::Program& prog, json::Object& obj)
{
  {
    VisitorContext context{ &m_stack, "global_namespace" };
    json::Object global_namespace = obj["global_namespace"].toObject();
    visit_namespace(*prog.globalNamespace(), global_namespace);
  }
}

void JsonVisitor::visit_domnode(const dom::Node& n, json::Object& obj)
{
  if (n.is<dom::List>())
    visit_domlist(static_cast<const dom::List&>(n), obj);
  else if(n.is<dom::ListItem>())
    visit_domlistitem(static_cast<const dom::ListItem&>(n), obj);
}

void JsonVisitor::visit_domlist(const dom::List& l, json::Object& obj)
{
  if (!l.items.empty())
  {
    VisitorContext context{ &m_stack, "items" };
    json::Json& js_items = obj["items"];

    for (size_t i(0); i < l.items.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      json::Object js_listitem = js_items.at(static_cast<int>(i)).toObject();
      visit_domnode(*l.items.at(i), js_listitem);
    }
  }
}

void JsonVisitor::visit_domlistitem(const dom::ListItem& li, json::Object& obj)
{
  if (!li.content.empty())
  {
    VisitorContext context{ &m_stack, "content" };
    json::Json& js_content = obj["content"];

    for (size_t i(0); i < li.content.size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      json::Object js_domnode = js_content.at(static_cast<int>(i)).toObject();
      visit_domnode(*li.content.at(i), js_domnode);
    }
  }
}

void JsonVisitor::visit_entity(const cxx::Entity& e, json::Object& obj)
{
  if (e.is<cxx::Class>())
    visit_class(static_cast<const cxx::Class&>(e), obj);
  else if (e.is<cxx::Function>())
    visit_function(static_cast<const cxx::Function&>(e), obj);
  else if (e.is<cxx::Namespace>())
    visit_namespace(static_cast<const cxx::Namespace&>(e), obj);

  if (e.documentation())
  {
    auto edoc = std::dynamic_pointer_cast<EntityDocumentation>(e.documentation());

    if (edoc)
    {
      json::Object json_doc = obj["documentation"].toObject();

      VisitorContext context{ &m_stack, "documentation" };
      visit_entitydocumentation(*edoc, json_doc);
    }
  }
}

void JsonVisitor::visit_namespace(const cxx::Namespace& ns, json::Object& obj)
{
  if(!ns.entities().empty())
  {
    VisitorContext context{ &m_stack, "entities" };
    json::Json& js_entities = obj["entities"];

    for (size_t i(0); i < ns.entities().size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      json::Object js_entity = js_entities.at(static_cast<int>(i)).toObject();
      visit_entity(*ns.entities().at(i), js_entity);
    }
  }
}

void JsonVisitor::visit_class(const cxx::Class& cla, json::Object& obj)
{
  if (!cla.members().empty())
  {
    VisitorContext context{ &m_stack, "members" };
    json::Json& js_members = obj["members"];

    for (size_t i(0); i < cla.members().size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      json::Object js_entity = js_members.at(static_cast<int>(i)).toObject();
      visit_entity(*cla.members().at(i).first, js_entity);
    }
  }
}

void JsonVisitor::visit_function(const cxx::Function& f, json::Object& obj)
{
  /* no-op */
}

void JsonVisitor::visit_entitydocumentation(const EntityDocumentation& edoc, json::Object& obj)
{
  if (!edoc.description().empty())
  {
    VisitorContext context{ &m_stack, "description" };
    json::Json& js_description = obj["description"];

    for (size_t i(0); i < edoc.description().size(); ++i)
    {
      VisitorContext inner_context{ &m_stack, i };

      json::Object js_domnode = js_description.at(static_cast<int>(i)).toObject();
      visit_domnode(*edoc.description().at(i), js_domnode);
    }
  }
}

} // namespace dex
