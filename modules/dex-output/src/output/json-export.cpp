// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-export.h"

#include "dex/model/since.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

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

json::Json JsonExport::serialize_enum(const cxx::Enum& en)
{
  json::Object result{};

  write_entity_info(result, en);

  write_location(result, en.location());

  write_documentation(result, en.documentation());

  if (!en.values().empty())
  {
    json::Array list;

    for (const std::shared_ptr<cxx::EnumValue>& ev : en.values())
    {
      list.push(serialize_enumvalue(*ev));
    }

    result["values"] = list;
  }

  return result;
}

json::Json JsonExport::serialize_enumvalue(const cxx::EnumValue& ev)
{
  json::Object result{};

  write_entity_info(result, ev);

  write_location(result, ev.location());

  write_documentation(result, ev.documentation());

  if (ev.value().empty())
    result["value"] = ev.value();

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

json::Json JsonExport::serialize_documentation(const cxx::Documentation& doc)
{
  if (doc.is<cxx::MultilineComment>())
    return nullptr;

  const auto& entdoc = static_cast<const EntityDocumentation&>(doc);

  if (entdoc.is<ClassDocumentation>())
    return serialize_documentation(static_cast<const ClassDocumentation&>(doc));
  else if(entdoc.is<EnumDocumentation>())
    return serialize_documentation(static_cast<const EnumDocumentation&>(doc));
  else if (entdoc.is<FunctionDocumentation>())
    return serialize_documentation(static_cast<const FunctionDocumentation&>(doc));
  else if (entdoc.is<NamespaceDocumentation>())
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

json::Json JsonExport::serialize_documentation(const EnumDocumentation& doc)
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

json::Array JsonExport::serialize_dom_content(const dom::Content& content)
{
  json::Array result;

  for (const auto& node : content)
  {
    try
    {
      json::Json docnode = serialize_documentation_node(*node);
      result.push(docnode);
    }
    catch (const std::runtime_error&)
    {
      // TODO: log ?
    }
  }

  return result;
}

json::Json JsonExport::serialize_documentation_node(const dom::Node& docnode)
{
  if (docnode.is<dom::Paragraph>())
  {
    return serialize_paragraph(static_cast<const dom::Paragraph&>(docnode));
  }
  else if (docnode.is<dom::Image>())
  {
    return serialize_image(static_cast<const dom::Image&>(docnode));
  }
  else if (docnode.is<dom::List>())
  {
    return serialize_list(static_cast<const dom::List&>(docnode));
  }

  assert(("Not implemented", false));
  return nullptr;
}

json::Json JsonExport::serialize_list(const dom::List& list)
{
  json::Json result;

  if (!list.marker.empty())
    result["marker"] = list.marker;

  result["ordered"] = list.ordered;

  if (list.ordered)
    result["reversed"] = list.reversed;

  json::Array items;

  for (const auto& listitem : list.items)
    items.push(serialize_listitem(*listitem));

  result["items"] = items;

  return result;
}

json::Json JsonExport::serialize_listitem(const dom::ListItem& listitem)
{
  json::Json result;

  if (!listitem.marker.empty())
    result["marker"] = listitem.marker;

  if (listitem.value != -1)
    result["value"] = listitem.value;

  result["content"] = serialize_dom_content(listitem.content);

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

json::Json JsonExport::serialize_image(const dom::Image& img)
{
  json::Json result{};
  result["src"] = img.src;
  
  if (img.height != -1)
    result["height"] = img.height;

  if(img.width != -1)
    result["width"] = img.width;

  return result;
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
  case cxx::NodeKind::TranslationUnit:
    return "translation-unit";
  case cxx::NodeKind::MultilineComment:
    return "multiline-comment";
  case cxx::NodeKind::Documentation:
    return "documentation";
  default:
    assert(false);
    return "";
  }
}

void JsonExport::write_entity_info(json::Object& obj, const cxx::Entity& e)
{
  obj["name"] = e.name();
  obj["type"] = to_string(e.kind());
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
    obj["description"] = serialize_dom_content(doc.description());
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
