// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-export.h"

#include "dex/model/display-math.h"
#include "dex/model/since.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>
#include <cxx/typedef.h>
#include <cxx/variable.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

static std::string to_string(cxx::AccessSpecifier as)
{
  switch (as)
  {
  case cxx::AccessSpecifier::PRIVATE:
    return "private";
  case cxx::AccessSpecifier::PROTECTED:
    return "protected";
  default:
    return "public";
  }
}

static std::string to_string(cxx::NodeKind n)
{
  switch (n)
  {
  case cxx::NodeKind::Class:
    return "class";
  case cxx::NodeKind::Enum:
    return "enum";
  case cxx::NodeKind::EnumValue:
    return "enum-value";
  case cxx::NodeKind::Function:
    return "function";
  case cxx::NodeKind::FunctionParameter:
    return "function-parameter";
  case cxx::NodeKind::Namespace:
    return "namespace";
  case cxx::NodeKind::TemplateParameter:
    return "template-parameter";
  case cxx::NodeKind::Typedef:
    return "typedef";
  case cxx::NodeKind::Variable:
    return "variable";
  case cxx::NodeKind::MultilineComment:
    return "multiline-comment";
  case cxx::NodeKind::Documentation:
    return "documentation";
  default:
    assert(false);
    return "";
  }
}

template<typename T>
void write_if(json::Object& obj, const char* field, T&& val, bool cond)
{
  if (cond)
    obj[field] = std::forward<T>(val);
}

struct RAIIJsonExportContext
{
  std::vector<json::Object>* stack;

  RAIIJsonExportContext(std::vector<json::Object>* s, const Model::PathElement& pe)
    : stack(s)
  {
    if (pe.index == std::numeric_limits<size_t>::max())
    {
      json::Object obj{};
      stack->back()[pe.name] = obj;
      stack->push_back(obj);
    }
    else
    {
      json::Object obj{};

      if (pe.index == 0)
      {
        stack->back()[pe.name] = json::Array();
      }

      stack->back()[pe.name].push(obj);
      stack->push_back(obj);
    }
  }

  ~RAIIJsonExportContext()
  {
    stack->pop_back();
  }
};

json::Object JsonExport::serialize(const Model& model)
{
  json::Object result{};

  JsonExport jexport;

  jexport.m_json_stack.push_back(result);

  jexport.visit(model);

  return result;
}

json::Object& JsonExport::object()
{
  return m_json_stack.back();
}

static json::Json serialize_par_metadata(const dom::ParagraphMetaData& pmd)
{
  json::Json result;

  result["type"] = pmd.type();
  result["begin"] = static_cast<int>(pmd.range().begin());
  result["end"] = static_cast<int>(pmd.range().end());

  if (pmd.is<dex::Since>())
  {
    result["version"] = pmd.get<dex::Since>().version();
  }
  else if (pmd.is<dom::TextStyle>())
  {
    result["style"] = static_cast<const dom::TextStyle&>(pmd).style();
  }
  else if (pmd.is<dom::Link>())
  {
    result["url"] = static_cast<const dom::Link&>(pmd).url();
  }
  else
  {
    // TODO: log or throw ?
  }

  return result;
}

void JsonExport::visit_domnode(const dom::Node& n)
{
  RAIIJsonExportContext context{ &m_json_stack, path().back() };

  object()["type"] = n.type();

  ModelVisitor::visit_domnode(n);
}

void JsonExport::visit_domimage(const dom::Image& img)
{
  object()["src"] = img.src;

  write_if(object(), "height", img.height, img.height != -1);
  write_if(object(), "width", img.width, img.width != -1);
}

void JsonExport::visit_domlist(const dom::List& l)
{
  write_if(object(), "marker", l.marker, !l.marker.empty());

  object()["ordered"] = l.ordered;

  write_if(object(), "reversed", l.reversed, l.ordered);

  ModelVisitor::visit_domlist(l);
}

void JsonExport::visit_domlistitem(const dom::ListItem& li)
{
  write_if(object(), "marker", li.marker, !li.marker.empty());
  write_if(object(), "value", li.value, li.value != -1);

  ModelVisitor::visit_domlistitem(li);
}

void JsonExport::visit_domparagraph(const dom::Paragraph& par)
{
  object()["text"] = par.text();

  if (!par.metadata().empty())
  {
    json::Array metadatas;

    for (const auto& md : par.metadata())
    {
      metadatas.push(serialize_par_metadata(*md));
    }

    object()["metadata"] = metadatas;
  }

  ModelVisitor::visit_domparagraph(par);
}

void JsonExport::visit_displaymath(const dex::DisplayMath& math)
{
  object()["source"] = math.source;

  ModelVisitor::visit_displaymath(math);
}


void JsonExport::visit_program(const cxx::Program& prog)
{
  RAIIJsonExportContext context{ &m_json_stack, path().back() };
  ModelVisitor::visit_program(prog);
}

void JsonExport::visit_entity(const cxx::Entity& e)
{
  RAIIJsonExportContext context{ &m_json_stack, path().back() };

  object()["name"] = e.name;
  object()["type"] = to_string(e.kind());

  write_location(object(), e.location);

  ModelVisitor::visit_entity(e);
}

void JsonExport::visit_namespace(const cxx::Namespace& ns)
{
  ModelVisitor::visit_namespace(ns);
}

void JsonExport::visit_class(const cxx::Class& cla)
{
  ModelVisitor::visit_class(cla);

  if (!cla.members.empty())
  {
    json::Array members = object()["members"].toArray();

    for (size_t i(0); i < cla.members.size(); ++i)
    {
      members[static_cast<int>(i)]["accessibility"] = to_string(cla.members.at(i)->getAccessSpecifier());
    }
  }
}

void JsonExport::visit_enum(const cxx::Enum& en)
{
  ModelVisitor::visit_enum(en);
}

void JsonExport::visit_enumvalue(const cxx::EnumValue& ev)
{
  write_if(object(), "value", ev.value(), !ev.value().empty());

  ModelVisitor::visit_enumvalue(ev);
}

void JsonExport::visit_function(const cxx::Function& f)
{
  ModelVisitor::visit_function(f);

  object()["return_type"] = f.return_type.toString();

  if (f.specifiers != 0)
  {
    std::string specifiers;

    if (f.specifiers & cxx::FunctionSpecifier::Inline)
      specifiers += "inline,";
    if (f.specifiers & cxx::FunctionSpecifier::Static)
      specifiers += "static,";
    if (f.specifiers & cxx::FunctionSpecifier::Constexpr)
      specifiers += "constexpr,";
    if (f.specifiers & cxx::FunctionSpecifier::Virtual)
      specifiers += "virtual,";
    if (f.specifiers & cxx::FunctionSpecifier::Override)
      specifiers += "override,";
    if (f.specifiers & cxx::FunctionSpecifier::Final)
      specifiers += "final,";
    if (f.specifiers & cxx::FunctionSpecifier::Const)
      specifiers += "const,";
    
    specifiers.pop_back();

    object()["specifiers"] = specifiers;
  }
}

void JsonExport::visit_functionparameter(const cxx::FunctionParameter& fp)
{
  ModelVisitor::visit_functionparameter(fp);

  object()["type"] = fp.type.toString();
  write_if(object(), "default_value", fp.default_value.toString(), fp.default_value != cxx::Expression());
  
  if (fp.documentation != nullptr)
    object()["documentation"] = static_cast<dex::FunctionParameterDocumentation*>(fp.documentation.get())->brief;
}

void JsonExport::visit_variable(const cxx::Variable& v)
{
  ModelVisitor::visit_variable(v);

  object()["vartype"] = v.type().toString();
  write_if(object(), "default_value", v.defaultValue().toString(), v.defaultValue() != cxx::Expression());

  if (v.specifiers() != 0)
  {
    std::string specifiers;

    if (v.specifiers() & cxx::VariableSpecifier::Inline)
      specifiers += "inline,";
    if (v.specifiers() & cxx::VariableSpecifier::Static)
      specifiers += "static,";
    if (v.specifiers() & cxx::VariableSpecifier::Constexpr)
      specifiers += "constexpr,";

    specifiers.pop_back();

    object()["specifiers"] = specifiers;
  }
}

void JsonExport::visit_typedef(const cxx::Typedef& t)
{
  ModelVisitor::visit_typedef(t);
  object()["typedef"] = t.type.toString();
}

void JsonExport::visit_entitydocumentation(const EntityDocumentation& edoc)
{
  RAIIJsonExportContext context{ &m_json_stack, path().back() };

  if (edoc.brief().has_value())
    object()["brief"] = edoc.brief().value();

  if (edoc.since().has_value())
    object()["since"] = edoc.since().value().version();

  if (edoc.is<FunctionDocumentation>())
  {
    const auto& fndoc = static_cast<const FunctionDocumentation&>(edoc);

    if (fndoc.returnValue().has_value())
      object()["returns"] = fndoc.returnValue().value();
  }

  ModelVisitor::visit_entitydocumentation(edoc);
}

void JsonExport::visit_manual(const Manual& man)
{
  object()["title"] = man.title;

  ModelVisitor::visit_manual(man);
}

void JsonExport::visit_sectioning(const Sectioning& sec)
{
  object()["name"] = sec.name;
  object()["depth"] = Sectioning::depth2str(sec.depth);

  ModelVisitor::visit_sectioning(sec);
}

void JsonExport::write_location(json::Object& obj, const cxx::SourceLocation& loc)
{
  if (loc.file() == nullptr)
    return;

  json::Object result{};
  result["line"] = loc.line();
  result["col"] = loc.column();
  result["file"] = loc.file()->path();

  obj["loc"] = result;
}

} // namespace dex
