// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-export.h"

#include "dex/model/since.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

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

json::Json JsonExport::serialize(const cxx::Program& prog)
{
  return serialize_namespace(*prog.globalNamespace());
}

json::Json JsonExport::serialize_entity(const cxx::Entity& e)
{
  if (e.is<cxx::Namespace>())
    return serialize_namespace(static_cast<const cxx::Namespace&>(e));
  else if (e.is<cxx::Class>())
    return serialize_class(static_cast<const cxx::Class&>(e));
  else if (e.is<cxx::Function>())
    return serialize_function(static_cast<const cxx::Function&>(e));
  
  assert(("Not implemented", false));
  return nullptr;
}

json::Json JsonExport::serialize_namespace(const cxx::Namespace& ns)
{
  json::Object result{};
  
  write_entity_info(result, ns);

  write_location(result, ns.location());

  write_documentation(result, ns.documentation());

  write_entities(result, ns.entities());

  return result;
}

json::Json JsonExport::serialize_class(const cxx::Class& cla)
{
  json::Object result{};

  write_entity_info(result, cla);

  write_location(result, cla.location());

  write_documentation(result, cla.documentation());

  if (!cla.members().empty())
  {
    json::Array list;
    for (const auto& m : cla.members())
    {
      json::Object member = serialize_entity(*m.first).toObject();
      member["accessibility"] = to_string(m.second);
      list.push(member);
    }

    result["members"] = list;
  }

  return result;
}

json::Json JsonExport::serialize_function(const cxx::Function& f)
{
  json::Object result{};

  write_entity_info(result, f);

  write_location(result, f.location());

  write_documentation(result, f.documentation());

  return result;
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
  else
  {
    // TODO: log or throw ?
  }

  return result;
}

json::Json JsonExport::serialize_paragraph(const dom::Paragraph& par)
{
  json::Json result;

  result["type"] = par.type();

  result["text"] = par.text();

  if (!par.metadata().empty())
  {
    json::Array metadatas;

    for (const auto& md : par.metadata())
    {
      metadatas.push(serialize_par_metadata(*md));
    }

    result["metadata"] = metadatas;
  }

  return result;
}

json::Json JsonExport::serialize_documentation(const cxx::Documentation& doc)
{
  if (doc.is<ClassDocumentation>())
    return serialize_documentation(static_cast<const ClassDocumentation&>(doc));
  else if (doc.is<FunctionDocumentation>())
    return serialize_documentation(static_cast<const FunctionDocumentation&>(doc));
  else if (doc.is<NamespaceDocumentation>())
    return serialize_documentation(static_cast<const NamespaceDocumentation&>(doc));

  assert(("Not implemented", false));
  return nullptr;
}

json::Json JsonExport::serialize_documentation(const ClassDocumentation& doc)
{
  json::Object result{};

  write_entity_documentation(result, doc);

  return result;
}

json::Json JsonExport::serialize_documentation(const FunctionDocumentation& doc)
{
  json::Object result{};

  write_entity_documentation(result, doc);

  if (!doc.parameters().empty())
  {
    json::Array params;

    for (const std::string& p : doc.parameters())
    {
      params.push(p);
    }

    result["parameters"] = params;
  }

  if (doc.returnValue().has_value())
  {
    result["returns"] = doc.returnValue().value();
  }

  return result;
}

json::Json JsonExport::serialize_documentation(const NamespaceDocumentation& doc)
{
  json::Object result{};

  write_entity_documentation(result, doc);

  return result;
}

json::Json JsonExport::serialize_documentation_node(const dom::Node& docnode)
{
  if (docnode.is<dom::Paragraph>())
  {
    return serialize_paragraph(static_cast<const dom::Paragraph&>(docnode));
  }

  assert(("Not implemented", false));
  return nullptr;
}

void JsonExport::write_entity_info(json::Object& obj, const cxx::Entity& e)
{
  obj["name"] = e.name();
  obj["type"] = e.type();
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

void JsonExport::write_documentation(json::Object& obj, const std::shared_ptr<cxx::Documentation>& doc)
{
  if (doc)
    write_documentation(obj, *doc);
}

void JsonExport::write_documentation(json::Object& obj, const cxx::Documentation& doc)
{
  obj["documentation"] = serialize_documentation(doc);
}

void JsonExport::write_entity_documentation(json::Object& obj, const EntityDocumentation& doc)
{
  if (doc.brief().has_value())
    obj["brief"] = doc.brief().value();

  if (doc.since().has_value())
    obj["since"] = doc.since().value().version();

  if (!doc.description().empty())
  {
    json::Array docnodes;

    for (const auto n : doc.description())
    {
      try
      {
        json::Json docnode = serialize_documentation_node(*n);
        docnodes.push(docnode);
      }
      catch (const std::runtime_error&)
      {
        // TODO: log ?
      }
    }

    obj["description"] = docnodes;
  }
}

void JsonExport::write_entities(json::Object& obj, const std::vector<std::shared_ptr<cxx::Entity>>& list)
{
  if (!list.empty())
  {
    json::Array entities;
    for (const auto& e : list)
    {
      entities.push(serialize_entity(*e));
    }

    obj["entities"] = entities;
  }
}

} // namespace dex
