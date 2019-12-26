// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_H
#define DEX_OUTPUT_LIQUID_EXPORTER_H

#include "dex/dex-output.h"

#include <liquid/renderer.h>

#include <QDir>

#include <variant>
#include <vector>

namespace cxx
{
class Entity;
class Class;
class Program;
} // namespace cxx

namespace dex
{

class DEX_OUTPUT_API LiquidExporter : public liquid::Renderer
{
public:

  typedef std::variant<size_t, std::string> JsonPathElement;
  typedef std::vector<JsonPathElement> JsonPath;

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

  const cxx::Program& program() const;
  const json::Object& jsonProgram() const;
  json::Object& jsonProgram();

  void setProgram(const std::shared_ptr<cxx::Program>& prog);

protected:

  std::shared_ptr<cxx::Entity> get(const JsonPath& path) const;
  static json::Json get(const JsonPath& path, const json::Json& val);

protected:

  virtual void postProcess(std::string& output);

protected:

  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

  static json::Array filter_by_field(const json::Array& list, const std::string& field, const std::string& value);
  static json::Array filter_by_type(const json::Array& list, const std::string& type);
  static json::Array filter_by_accessibility(const json::Array& list, const std::string& as);

private:
  QDir m_output_dir;
  std::shared_ptr<cxx::Program> m_program;
  json::Object m_json_program;
  Templates m_templates;
};

} // namespace dex

namespace dex
{

inline LiquidExporter::Templates& LiquidExporter::templates()
{
  return m_templates;
}

inline const cxx::Program& LiquidExporter::program() const
{
  return *m_program;
}

inline const json::Object& LiquidExporter::jsonProgram() const
{
  return m_json_program;
}

inline json::Object& LiquidExporter::jsonProgram()
{
  return m_json_program;
}

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_H
