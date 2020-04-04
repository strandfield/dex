// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_H
#define DEX_OUTPUT_LIQUID_EXPORTER_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <liquid/renderer.h>

#include <QDir>

#include <variant>
#include <vector>

namespace dom
{
class Image;
class Paragraph;
} // namespace dom

namespace cxx
{
class Entity;
class Class;
class Program;
} // namespace cxx

namespace dex
{

class Model;

class DEX_OUTPUT_API LiquidExporter : public liquid::Renderer
{
public:

  typedef std::variant<size_t, std::string> JsonPathElement;
  typedef std::vector<JsonPathElement> JsonPath;

  static Model::Path convertToModelPath(const JsonPath& jspath);

  struct Templates
  {
    liquid::Template class_template;
  };

  void dumpClasses();

  Templates& templates();

  void setOutputDir(const QDir& dir);

  void render();

  void dump(const cxx::Class& cla, const json::Object& obj);

  static void trim_right(std::string& str);
  static void simplify_empty_lines(std::string& str);

protected:

  void setModel(std::shared_ptr<Model> model);
  std::shared_ptr<Model> model() const;
  json::Object serializedModel() const;

protected:
  std::string stringify(const json::Json& val) override;

  virtual std::string stringify_array(const json::Array& list) = 0;
  virtual std::string stringify_paragraph(const dom::Paragraph& par) = 0;
  virtual std::string stringify_image(const dom::Image& img) = 0;

protected:

  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

  static json::Array filter_by_field(const json::Array& list, const std::string& field, const std::string& value);
  static json::Array filter_by_type(const json::Array& list, const std::string& type);
  static json::Array filter_by_accessibility(const json::Array& list, const std::string& as);

protected:

  virtual void postProcess(std::string& output);

private:
  QDir m_output_dir;
  std::shared_ptr<Model> m_model;
  json::Object m_serialized_model;
  Templates m_templates;
};

} // namespace dex

namespace dex
{

inline LiquidExporter::Templates& LiquidExporter::templates()
{
  return m_templates;
}

inline std::shared_ptr<Model> LiquidExporter::model() const
{
  return m_model;
}

inline json::Object LiquidExporter::serializedModel() const
{
  return m_serialized_model;
}

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_H
