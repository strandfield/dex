// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_H
#define DEX_OUTPUT_LIQUID_EXPORTER_H

#include "dex/dex-output.h"

#include "dex/output/json-export.h"
#include "dex/output/liquid-exporter-profile.h"

#include "dex/model/model.h"

#include <liquid/renderer.h>

#include <QDir>

#include <variant>
#include <vector>

namespace dom
{
class Image;
class List;
class ListItem;
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

class DisplayMath;
class Model;

class DEX_OUTPUT_API LiquidExporter : public liquid::Renderer
{
public:

  typedef std::variant<size_t, std::string> JsonPathElement;
  typedef std::vector<JsonPathElement> JsonPath;

  static Model::Path convertToModelPath(const JsonPath& jspath);

  struct Settings
  {
    liquid::Template class_template;
    liquid::Template namespace_template;
    liquid::Template function_template;
    std::string class_outdir;
    std::string namespace_outdir;
    std::string function_outdir;
    std::vector<std::pair<std::string, liquid::Template>> files;
  };

  typedef LiquidExporterProfile Profile;

  Profile& profile();

  QDir outputDir() const;
  void setOutputDir(const QDir& dir);

  void setVariables(json::Object obj);
  const json::Object& variables() const;

  void render();

  static void trim_right(std::string& str);
  static void simplify_empty_lines(std::string& str);

protected:

  void setModel(std::shared_ptr<Model> model);
  std::shared_ptr<Model> model() const;
  json::Object serializedModel() const;
  const JsonExportMapping& modelMapping() const;

  void annotateModel(const std::string& file_suffix);

protected:
  friend class LiquidExporterModelVisitor;

  void dump(const json::Object& obj, const char* obj_field_name, const Profile::Template& tmplt);

  void dump(const cxx::Class& cla, const json::Object& obj);
  void dump(const dex::Manual& man, const json::Object& obj);

protected:
  std::string stringify(const json::Json& val) override;

  virtual std::string stringify_domnode(const dom::Node& node);
  virtual std::string stringify_domcontent(const dom::Content& content);

  virtual std::string stringify_array(const json::Array& list);
  virtual std::string stringify_list(const dom::List& list) = 0;
  virtual std::string stringify_listitem(const dom::ListItem& li) = 0;
  virtual std::string stringify_paragraph(const dom::Paragraph& par) = 0;
  virtual std::string stringify_image(const dom::Image& img) = 0;
  virtual std::string stringify_math(const dex::DisplayMath& math) = 0;

  virtual std::string stringify_section(const dex::Sectioning& sec) = 0;

protected:

  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

  static json::Array filter_by_field(const json::Array& list, const std::string& field, const std::string& value);
  static json::Array filter_by_type(const json::Array& list, const std::string& type);
  static json::Array filter_by_accessibility(const json::Array& list, const std::string& as);
  json::Array related_non_members(const json::Object& json_class);

protected:

  void setupContext(json::Object& context);
  virtual void postProcess(std::string& output);
  void write(const std::string& data, const std::string& filepath);

private:
  QDir m_output_dir;
  std::shared_ptr<Model> m_model;
  json::Object m_serialized_model;
  JsonExportMapping m_model_mapping;
  Profile m_profile;
  json::Object m_user_variables;
};

} // namespace dex

namespace dex
{

inline LiquidExporterProfile& LiquidExporter::profile()
{
  return m_profile;
}

inline std::shared_ptr<Model> LiquidExporter::model() const
{
  return m_model;
}

inline json::Object LiquidExporter::serializedModel() const
{
  return m_serialized_model;
}

inline const JsonExportMapping& LiquidExporter::modelMapping() const
{
  return m_model_mapping;
}

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_H
