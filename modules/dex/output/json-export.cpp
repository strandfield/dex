// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/json-export.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/paragraph-annotations.h"
#include "dex/model/since.h"

namespace dex
{

static std::string to_string(dex::AccessSpecifier as)
{
  switch (as)
  {
  case dex::AccessSpecifier::PRIVATE:
    return "private";
  case dex::AccessSpecifier::PROTECTED:
    return "protected";
  default:
    return "public";
  }
}

template<typename T>
void write_if(json::Object& obj, const char* field, T&& val, bool cond)
{
  if (cond)
    obj[field] = std::forward<T>(val);
}

//static void write_location(json::Object& obj, const dex::SourceLocation& loc)
//{
//  if (loc.file() == nullptr)
//    return;
//
//  json::Object result{};
//  result["line"] = loc.line();
//  result["col"] = loc.column();
//  result["file"] = loc.file()->path();
//
//  obj["loc"] = result;
//}

static json::Json serialize_par_metadata(const dex::ParagraphMetaData& pmd)
{
  json::Json result;

  result["type"] = pmd.className();
  result["begin"] = static_cast<int>(pmd.range().begin());
  result["end"] = static_cast<int>(pmd.range().end());

  if (pmd.is<dex::Since>())
  {
    result["version"] = pmd.get<dex::Since>().version();
  }
  else if (pmd.is<dex::TextStyle>())
  {
    result["style"] = static_cast<const dex::TextStyle&>(pmd).style();
  }
  else if (pmd.is<dex::Link>())
  {
    result["url"] = static_cast<const dex::Link&>(pmd).url();
  }
  else if (pmd.is<dex::ParIndexEntry>())
  {
    result["key"] = pmd.get<dex::ParIndexEntry>().key;
  }
  else
  {
    // TODO: log or throw ?
  }

  return result;
}

JsonExporter::JsonExporter(const Model& m)
  : model(m)
{

}

json::Object JsonExporter::serialize(const Model& m)
{
  JsonExporter ex{ m };
  return ex.serialize();
}

json::Object JsonExporter::serialize()
{
  json::Object result;

  if (model.program())
  {
    JsonProgramSerializer progserializer{ mapping };
    result["program"] = progserializer.serialize(*model.program());
  }

  if (!model.documents.empty())
  {
    json::Array docs;

    for (size_t i(0); i < model.documents.size(); ++i)
    {
      JsonDocumentSerializer docserializer{ mapping };
      docs.push(docserializer.serialize(*model.documents.at(i)));
    }

    result["documents"] = docs;
  }

  if (!model.groups.groups.empty())
  {
    json::Array groups;

    for (size_t i(0); i < model.groups.groups.size(); ++i)
    {
      groups.push(serializeGroup(*model.groups.groups.at(i)));
    }

    result["groups"] = groups;
  }

  return result;
}

json::Object JsonExporter::serializeGroup(const Group& group)
{
  json::Object res;

  res["name"] = group.name;

  {
    json::Array ets;

    JsonProgramSerializer progser{ mapping };

    for (auto e : group.content.entities)
    {
      ets.push(progser.path(*e, *model.program()));
    }

    res["entities"] = ets;
  }

  {
    json::Array docs;

    std::string base_path{"$.documents["};

    for (auto d : group.content.documents)
    {
      auto it = std::find(model.documents.begin(), model.documents.end(), d);
      size_t dist = std::distance(model.documents.begin(), it);
      docs.push(base_path + std::to_string(dist) + "]");
    }

    res["documents"] = docs;
  }

  mapping.bind(group, res);

  return res;
}

json::Object JsonDocumentSerializer::serialize(dex::Document& doc)
{
  result["type"] = doc.className();
  result["title"] = doc.title;
  result["doctype"] = doc.doctype;
  result["content"] = serializeArray(doc.childNodes());

  mapping.bind(doc, result);

  return result;
}

json::Object JsonDocumentSerializer::serialize(dex::DocumentNode& n)
{
  json::Object ret{};
  std::swap(this->result, ret);
  visitNode(n);
  std::swap(this->result, ret);

  return ret;
}

json::Array JsonDocumentSerializer::serializeArray(const DomNodeList& nodes)
{
  json::Array res;

  for (auto n : nodes)
    res.push(serialize(*n));

  return res;
}


void JsonDocumentSerializer::visitNode(dex::DocumentNode& n)
{
  result["type"] = n.className();
  dispatch(n);
  mapping.bind(n, result);
}


void JsonDocumentSerializer::visit(dex::Image& img)
{
  result["src"] = img.src;

  write_if(result, "height", img.height, img.height != -1);
  write_if(result, "width", img.width, img.width != -1);
}

void JsonDocumentSerializer::visit(dex::List& l)
{
  write_if(result, "marker", l.marker, !l.marker.empty());

  result["ordered"] = l.ordered;

  write_if(result, "reversed", l.reversed, l.ordered);

  result["items"] = serializeArray(l.childNodes());
}

void JsonDocumentSerializer::visit(dex::ListItem& li)
{
  write_if(result, "marker", li.marker, !li.marker.empty());
  write_if(result, "value", li.value, li.value != -1);

  result["content"] = serializeArray(li.childNodes());
}

void JsonDocumentSerializer::visit(dex::Paragraph& par)
{
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
}

void JsonDocumentSerializer::visit(dex::BeginSince& bsince)
{
  result["version"] = bsince.version;
}

void JsonDocumentSerializer::visit(dex::EndSince& esince)
{
  result["version"] = esince.beginsince.lock()->version;
}

void JsonDocumentSerializer::visit(dex::DisplayMath& math)
{
  result["source"] = math.source;
}

void JsonDocumentSerializer::visit(dex::GroupTable& table)
{
  result["groupname"] = table.groupname;
}

void JsonDocumentSerializer::visit(dex::CodeBlock& codeblock)
{
  result["lang"] = codeblock.lang;
  result["code"] = codeblock.code;
}


void JsonDocumentSerializer::visit(dex::FrontMatter& fm)
{
  // no-op
}

void JsonDocumentSerializer::visit(dex::MainMatter& mm)
{
  // no-op
}

void JsonDocumentSerializer::visit(dex::BackMatter& bm)
{
  // no-op
}

void JsonDocumentSerializer::visit(dex::Sectioning& sec)
{
  result["name"] = sec.name;
  result["depth"] = Sectioning::depth2str(sec.depth);

  result["content"] = serializeArray(sec.childNodes());
}

void JsonDocumentSerializer::visit(dex::TableOfContents& toc)
{
  // no-op
}

void JsonDocumentSerializer::visit(dex::Index& idx)
{
  // no-op
}



json::Object JsonProgramSerializer::serialize(dex::Program& prog)
{
  result["global_namespace"] = serialize(*prog.globalNamespace());

  if (!prog.macros.empty())
  {
    result["macros"] = serializeArray(prog.macros);
  }

  if (!prog.related.empty())
    result["related"] = serializeRelatedMembers(prog.related, prog);

  return result;
}

json::Object JsonProgramSerializer::serialize(dex::Entity& e)
{
  json::Object ret{};
  std::swap(this->result, ret);
  visit(e);
  std::swap(this->result, ret);

  return ret;
}

std::string JsonProgramSerializer::path(dex::Entity& e, dex::Program& prog)
{
  if (e.shared_from_this() == prog.globalNamespace())
    return "$.program.global_namespace";

  std::vector<std::string> result;

  auto current_entity = e.shared_from_this();
  auto parent_entity = e.parent();

  while (parent_entity != nullptr)
  {
    if (parent_entity->is<dex::Class>())
    {
      const dex::Class& parent_class = static_cast<const dex::Class&>(*parent_entity);
      auto it = std::find(parent_class.members.begin(), parent_class.members.end(), current_entity);
      result.push_back(std::string("members[") + std::to_string(std::distance(parent_class.members.begin(), it)) + "]");
    }
    else if (parent_entity->is<dex::Namespace>())
    {
      const auto& parent_namespace = static_cast<const dex::Namespace&>(*parent_entity);
      auto it = std::find(parent_namespace.entities.begin(), parent_namespace.entities.end(), current_entity);
      result.push_back(std::string("entities[") + std::to_string(std::distance(parent_namespace.entities.begin(), it)) + "]");
    }
    else
    {
      throw std::runtime_error{ "Could not compute entitiy's path" };
    }

    current_entity = parent_entity;
    parent_entity = current_entity->parent();
  }

  result.push_back("global_namespace");
  result.push_back("program");

  std::reverse(result.begin(), result.end());

  {
    std::string result_str = "$.";

    for (size_t i(0); i < result.size(); ++i)
    {
      result_str += result.at(i);
      result_str += ".";
    }

    result_str.pop_back();

    return result_str;
  }
}

json::Array JsonProgramSerializer::serializeArray(const std::vector<std::shared_ptr<dex::Entity>>& nodes)
{
  json::Array res;

  for (auto n : nodes)
    res.push(serialize(*n));

  return res;
}

json::Array JsonProgramSerializer::serializeRelatedMembers(dex::RelatedNonMembers& rnm, dex::Program& prog)
{
  json::Array result;

  for (const auto& e : rnm.class_map)
  {
    json::Object json_entry{};
    json_entry["class"] = path(*(e.second->the_class), prog);

    json::Array json_functions;

    for (auto f : e.second->non_members)
    {
      json_functions.push(path(*f, prog));
    }

    json_entry["functions"] = json_functions;

    result.push(json_entry);
  }

  return result;
}

void JsonProgramSerializer::write_documentation(dex::Entity& e)
{
  json::Object jsondoc{};

  if (e.brief.has_value())
    jsondoc["brief"] = e.brief.value();

  if (e.since.has_value())
    jsondoc["since"] = e.since.value().version();

  if (e.is<dex::Function>())
  {
    const auto& fn = static_cast<const dex::Function&>(e);

    if (fn.return_type.brief.has_value())
      jsondoc["returns"] = fn.return_type.brief.value();
  }

  if (e.description && !e.description->childNodes().empty())
  {
    JsonDocumentSerializer jsonserializer{ mapping };
    jsondoc["description"] = jsonserializer.serializeArray(e.description->childNodes());
  }

  result["documentation"] = jsondoc;
}

void JsonProgramSerializer::visit(dex::Entity& e)
{
  result["name"] = e.name;
  result["type"] = to_string(e.kind());

  //write_location(result, e.location);
  write_documentation(e);

  dispatch(e);

  mapping.bind(e, result);
}

void JsonProgramSerializer::visit(dex::Namespace& ns)
{
  if(!ns.entities.empty())
    result["entities"] = serializeArray(ns.entities);
}

void JsonProgramSerializer::visit(dex::Class& cla)
{
  if (!cla.members.empty())
  {
    json::Array members = serializeArray(cla.members);

    for (size_t i(0); i < cla.members.size(); ++i)
    {
      members[static_cast<int>(i)]["accessibility"] = to_string(cla.members.at(i)->getAccessSpecifier());
    }

    result["members"] = members;
  }
}

void JsonProgramSerializer::visit(dex::Enum& en)
{
  result["values"] = serializeArray(en.values);
}

void JsonProgramSerializer::visit(dex::EnumValue& ev)
{
  write_if(result, "value", ev.value(), !ev.value().empty());
}

void JsonProgramSerializer::visit(dex::Function& f)
{
  if (!f.parameters.empty())
  {
    result["parameters"] = serializeArray(f.parameters);
  }

  result["return_type"] = f.return_type.type;

  if (f.specifiers != 0)
  {
    std::string specifiers;

    if (f.specifiers & dex::FunctionSpecifier::Inline)
      specifiers += "inline,";
    if (f.specifiers & dex::FunctionSpecifier::Static)
      specifiers += "static,";
    if (f.specifiers & dex::FunctionSpecifier::Constexpr)
      specifiers += "constexpr,";
    if (f.specifiers & dex::FunctionSpecifier::Virtual)
      specifiers += "virtual,";
    if (f.specifiers & dex::FunctionSpecifier::Override)
      specifiers += "override,";
    if (f.specifiers & dex::FunctionSpecifier::Final)
      specifiers += "final,";
    if (f.specifiers & dex::FunctionSpecifier::Const)
      specifiers += "const,";

    specifiers.pop_back();

    result["specifiers"] = specifiers;
  }
}

void JsonProgramSerializer::visit(dex::FunctionParameter& fp)
{
  result["type"] = fp.type;
  write_if(result, "default_value", fp.default_value, fp.default_value != dex::Expression());

  if (fp.brief.has_value())
    result["documentation"] = fp.brief.value();
}

void JsonProgramSerializer::visit(dex::Variable& v)
{
  result["vartype"] = v.type();
  write_if(result, "default_value", v.defaultValue(), v.defaultValue() != dex::Expression());

  if (v.specifiers() != 0)
  {
    std::string specifiers;

    if (v.specifiers() & dex::VariableSpecifier::Inline)
      specifiers += "inline,";
    if (v.specifiers() & dex::VariableSpecifier::Static)
      specifiers += "static,";
    if (v.specifiers() & dex::VariableSpecifier::Constexpr)
      specifiers += "constexpr,";

    specifiers.pop_back();

    result["specifiers"] = specifiers;
  }
}

void JsonProgramSerializer::visit(dex::Typedef& t)
{
  result["typedef"] = t.type;
}

void JsonProgramSerializer::visit(dex::Macro& m)
{
  json::Array params;

  for (const std::string& p : m.parameters)
    params.push(p);

  result["parameters"] = params;
}

} // namespace dex
