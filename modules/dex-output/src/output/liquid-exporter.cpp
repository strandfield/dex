// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter.h"

#include "dex/output/json-annotator.h"
#include "dex/output/json-export.h"
#include "dex/output/json-visitor.h"

#include "dex/common/errors.h"

#include "dex/model/model.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

namespace dex
{

using TreeNode = std::variant<
  std::shared_ptr<cxx::Entity>,
  const std::vector<std::shared_ptr<cxx::Entity>>*,
  const std::vector<std::pair<std::shared_ptr<cxx::Entity>, cxx::AccessSpecifier>>*>;


static TreeNode get_child_node(const TreeNode& node, const std::string& child_name)
{
  if (!std::holds_alternative<std::shared_ptr<cxx::Entity>>(node))
    throw std::runtime_error{ "Invalid json path" };

  auto e = std::get<std::shared_ptr<cxx::Entity>>(node);

  if (e->is<cxx::Namespace>())
  {
    auto ns = std::static_pointer_cast<cxx::Namespace>(e);

    if (child_name == "entities")
    {
      return &(ns->entities());
    }
  }
  else if (e->is<cxx::Class>())
  {
    auto cla = std::static_pointer_cast<cxx::Class>(e);

    if (child_name == "members")
    {
      return &(cla->members());
    }
  }
  else if (e->is<cxx::Function>())
  {
    /* no-op */
  }
  
  throw std::runtime_error{ "Invalid json path" };
}

static TreeNode get_child_node(const TreeNode& node, size_t index)
{
  if (std::holds_alternative<std::shared_ptr<cxx::Entity>>(node))
    throw std::runtime_error{ "Invalid json path" };
 
  if (std::holds_alternative<const std::vector<std::shared_ptr<cxx::Entity>>*>(node))
  {
    const std::vector<std::shared_ptr<cxx::Entity>>* vec =
      std::get<const std::vector<std::shared_ptr<cxx::Entity>>*>(node);

    return vec->at(index);
  }
  else
  {
    const std::vector<std::pair<std::shared_ptr<cxx::Entity>, cxx::AccessSpecifier>>* vec =
      std::get<const std::vector<std::pair<std::shared_ptr<cxx::Entity>, cxx::AccessSpecifier>>*>(node);

    return vec->at(index).first;
  }
}

static json::Json get_child_node(const json::Json& node, const std::string& child_name)
{
  if (!node.isObject())
    throw std::runtime_error{ "Invalid json path" };

  return node[child_name];
}

static json::Json get_child_node(const json::Json& node, size_t index)
{
  if(!node.isArray())
    throw std::runtime_error{ "Invalid json path" };

  return node.at(static_cast<int>(index));
}


struct ClassDumper : JsonVisitor
{
  LiquidExporter& exporter;

  ClassDumper(LiquidExporter& e)
    : exporter{ e }
  {

  }

  void visit_class(const cxx::Class& cla, json::Object& obj) override
  {
    exporter.dump(cla, obj);

    JsonVisitor::visit_class(cla, obj);
  }
};

void LiquidExporter::setOutputDir(const QDir& dir)
{
  m_output_dir = dir;
}

void LiquidExporter::render()
{
  dumpClasses();
}

void LiquidExporter::dumpClasses()
{
  if (m_templates.class_template.nodes().empty())
    return;

  if (!model()->program())
    return;

  json::Object prog = m_serialized_model["program"].toObject();

  ClassDumper dumper{ *this };
  dumper.visit(*model()->program(), prog);
}

void LiquidExporter::dump(const cxx::Class& cla, const json::Object& obj)
{
  if (obj["url"] == nullptr)
  {
    return;
  }

  const std::string url = obj["url"].toString();

  json::Object context;
  context["prog"] = m_serialized_model["program"];
  context["class"] = obj;

  std::string output = liquid::Renderer::render(m_templates.class_template, context);

  postProcess(output);

  QFileInfo fileinfo{ m_output_dir.absolutePath() + "/" + QString::fromStdString(url) };

  if (!fileinfo.dir().exists())
  {
    const bool success = QDir().mkpath(fileinfo.dir().absolutePath());

    if (!success)
      throw IOException{ fileinfo.dir().absolutePath().toStdString(), "could not create directory" };
  }

  QFile file{ fileinfo.absoluteFilePath() };

  if (!file.open(QIODevice::WriteOnly))
    throw IOException{ fileinfo.absoluteFilePath().toStdString(), "could not open file for writing" };

  file.write(output.data());

  file.close();
}

void LiquidExporter::setModel(std::shared_ptr<Model> model)
{
  m_model = model;
  
  m_serialized_model = JsonExport::serialize(*model).toObject();

  JsonPathAnnotator path_annotator;
  path_annotator.annotate(*model, m_serialized_model);
}

std::shared_ptr<cxx::Entity> LiquidExporter::get(const JsonPath& path) const
{
  TreeNode node = model()->program()->globalNamespace();

  for (const auto& p : path)
  {
    if (std::holds_alternative<size_t>(p))
      node = get_child_node(node, std::get<size_t>(p));
    else
      node = get_child_node(node, std::get<std::string>(p));
  }

  if (!std::holds_alternative<std::shared_ptr<cxx::Entity>>(node))
    throw std::runtime_error{ "Invalid json path" };

  return std::get<std::shared_ptr<cxx::Entity>>(node);
}

json::Json LiquidExporter::get(const JsonPath& path, const json::Json& val)
{
  auto result = val;

  for (const auto& p : path)
  {
    if (std::holds_alternative<size_t>(p))
      result = get_child_node(result, std::get<size_t>(p));
    else
      result = get_child_node(result, std::get<std::string>(p));
  }

  return result;
}

void LiquidExporter::postProcess(std::string& output)
{
  /* no-op */
}

void LiquidExporter::trim_right(std::string& str)
{
  // Remove spaces before end of line '\n'
  {
    size_t w = 0;

    for (size_t r = 0; r < str.size();)
    {
      if (str.at(r) == ' ')
      {
        // Find next non space char
        size_t rr = r;

        while (rr < str.size() && str.at(rr) == ' ') ++rr;

        if (str.at(rr) == '\n') // discard the spaces
        {
          r = rr;
        }
        else // keep the spaces
        {
          while (r < rr)
            str[w++] = str[r++];
        }
      }
      else
      {
        str[w++] = str[r++];
      }
    }

    str.resize(w);
  }
}

void LiquidExporter::simplify_empty_lines(std::string& str)
{
  size_t w = 0;

  for (size_t r = 0; r < str.size();)
  {
    if (str.at(r) == '\n')
    {
      // Find next non new-line char
      size_t rr = r;

      while (rr < str.size() && str.at(rr) == '\n') ++rr;

      if (rr - r >= 3) // discard the extra '\n'
      {
        r = rr;
        str[w++] = '\n';
        str[w++] = '\n';
      }
      else // keep the newlines
      {
        while (r < rr)
          str[w++] = str[r++];
      }
    }
    else
    {
      str[w++] = str[r++];
    }
  }

  str.resize(w);
}

static json::Array array_arg(const json::Json& object)
{
  if (object.isNull())
    return json::Array();
  else if (object.isArray())
    return object.toArray();
  else
    throw std::runtime_error{ "Object is not an array" };
}

json::Json LiquidExporter::applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args)
{
  if (name == "filter_by_type")
  {
    return filter_by_type(array_arg(object), args.front().toString());
  }
  else if (name == "filter_by_accessibility")
  {
    return filter_by_accessibility(array_arg(object), args.front().toString());
  }
  else if (name == "filter_by_field")
  {
    return filter_by_field(array_arg(object), args.front().toString(), args.back().toString());
  }

  return liquid::Renderer::applyFilter(name, object, args);
}

json::Array LiquidExporter::filter_by_field(const json::Array& list, const std::string& field, const std::string& value)
{
  json::Array result;

  for (const auto& obj : list.data())
  {
    if (obj[field] == value)
      result.push(obj);
  }

  return result;
}

json::Array LiquidExporter::filter_by_type(const json::Array& list, const std::string& type)
{
  static const std::string field = "type";
  return filter_by_field(list, field, type);
}

json::Array LiquidExporter::filter_by_accessibility(const json::Array& list, const std::string& as)
{
  static const std::string field = "accessibility";
  return filter_by_field(list, field, as);
}

} // namespace dex
