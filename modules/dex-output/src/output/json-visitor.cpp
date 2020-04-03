// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-visitor.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

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
  visit_namespace(*prog.globalNamespace(), obj);
}

void JsonVisitor::visit_entity(const cxx::Entity& e, json::Object& obj)
{
  if (e.is<cxx::Class>())
    visit_class(static_cast<const cxx::Class&>(e), obj);
  else if (e.is<cxx::Function>())
    visit_function(static_cast<const cxx::Function&>(e), obj);
  else if (e.is<cxx::Namespace>())
    visit_namespace(static_cast<const cxx::Namespace&>(e), obj);
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

} // namespace dex
