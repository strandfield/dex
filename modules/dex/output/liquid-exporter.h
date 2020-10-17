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

class LiquidStringifier;

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

  const Profile& profile() const;
  void setProfile(Profile pro);

  QDir outputDir() const;
  void setOutputDir(const QDir& dir);

  void setVariables(json::Object obj);
  const json::Object& variables() const;

  void render();

  static void trim_right(std::string& str);
  static void simplify_empty_lines(std::string& str);

  void setModel(std::shared_ptr<Model> model);
  std::shared_ptr<Model> model() const;
  json::Object serializedModel() const;
  const JsonExportMapping& modelMapping() const;

  std::string capture(const liquid::Template& tmplt, const json::Object& data);

protected:

  void annotateModel();

protected:
  friend class LiquidExporterModelVisitor;

  void dump(const json::Object& obj, const char* obj_field_name, const Profile::Template& tmplt);

  void dump(const cxx::Class& cla, const json::Object& obj);
  void dump(const dex::Manual& man, const json::Object& obj);

protected:
  std::string stringify(const json::Json& val) override;

protected:

  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

  static json::Array filter_by_field(const json::Array& list, const std::string& field, const std::string& value);
  static json::Array filter_by_type(const json::Array& list, const std::string& type);
  static json::Array filter_by_accessibility(const json::Array& list, const std::string& as);
  json::Array related_non_members(const json::Object& json_class);
  json::Array group_get_entities(const json::Object& json_group);
  json::Array group_get_manuals(const json::Object& json_group);
  json::Array group_get_groups(const json::Object& json_group);

protected:

  void setupContext(json::Object& context);
  void postProcess(std::string& output);
  void write(const std::string& data, const std::string& filepath);

private:
  QDir m_output_dir;
  std::shared_ptr<Model> m_model;
  json::Object m_serialized_model;
  JsonExportMapping m_model_mapping;
  Profile m_profile;
  json::Object m_user_variables;
protected:
  std::shared_ptr<LiquidStringifier> m_stringifier;
};

} // namespace dex

namespace dex
{

inline const LiquidExporterProfile& LiquidExporter::profile() const
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
