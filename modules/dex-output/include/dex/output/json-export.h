// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_JSON_EXPORT_H
#define DEX_OUTPUT_JSON_EXPORT_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <dom/paragraph.h>

#include <json-toolkit/json.h>

namespace dom
{
class Image;
class List;
class ListItem;
} // namespace dom

namespace cxx
{
class Entity;
class Class;
class Documentation;
class Enum;
class Function;
class Namespace;
class Program;
class SourceLocation;
} // namespace cxx

namespace dex
{

class DEX_OUTPUT_API JsonExport
{
public:

  static json::Json serialize(const cxx::Program& prog);

protected:
  static json::Json serialize_entity(const cxx::Entity& e);
  static json::Json serialize_namespace(const cxx::Namespace& ns);
  static json::Json serialize_class(const cxx::Class& cla);
  static json::Json serialize_enum(const cxx::Enum& en);
  static json::Json serialize_function(const cxx::Function& f);
  static json::Json serialize_documentation(const cxx::Documentation& doc);
  static json::Json serialize_documentation(const ClassDocumentation& doc);
  static json::Json serialize_documentation(const EnumDocumentation& doc);
  static json::Json serialize_documentation(const FunctionDocumentation& doc);
  static json::Json serialize_documentation(const NamespaceDocumentation& doc);
  static json::Array serialize_dom_content(const dom::Content& content);
  static json::Json serialize_documentation_node(const dom::Node& docnode);
  static json::Json serialize_list(const dom::List& list);
  static json::Json serialize_listitem(const dom::ListItem& listitem);
  static json::Json serialize_paragraph(const dom::Paragraph& par);
  static json::Json serialize_image(const dom::Image& img);

protected:
  static void write_entity_info(json::Object& obj, const cxx::Entity& e);
  static void write_location(json::Object& obj, const cxx::SourceLocation& loc);
  static void write_documentation(json::Object& obj, const std::shared_ptr<cxx::Documentation>& doc);
  static void write_documentation(json::Object& obj, const cxx::Documentation& doc);
  static void write_entity_documentation(json::Object& obj, const EntityDocumentation& doc);
  static void write_entities(json::Object& obj, const std::vector<std::shared_ptr<cxx::Entity>>& list);
};

} // namespace dex

#endif // DEX_OUTPUT_JSON_EXPORT_H
