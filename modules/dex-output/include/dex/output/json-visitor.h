// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_VISITOR_H
#define DEX_OUTPUT_JSON_VISITOR_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <json-toolkit/json.h>

#include <variant>
#include <vector>

namespace cxx
{
class Entity;
class Class;
class Documentation;
class Function;
class Namespace;
class Program;
} // namespace cxx

namespace dex
{

class DEX_OUTPUT_API JsonVisitor
{
public:

  void visit(const cxx::Program& prog, json::Object& obj);

  const std::vector<std::variant<size_t, std::string>>& stack() const;

protected:
  virtual void visit_entity(const cxx::Entity& e, json::Object& obj);
  virtual void visit_namespace(const cxx::Namespace& ns, json::Object& obj);
  virtual void visit_class(const cxx::Class& cla, json::Object& obj);
  virtual void visit_function(const cxx::Function& f, json::Object& obj);

private:
  std::vector<std::variant<size_t, std::string>> m_stack;
};

} // namespace dex

namespace dex
{

inline const std::vector<std::variant<size_t, std::string>>& JsonVisitor::stack() const
{
  return m_stack;
}

} // namespace dex

#endif // DEX_OUTPUT_JSON_VISITOR_H
