// Copyright (C) 2019-2021 Vincent Chambrin
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

#include <map>
#include <variant>
#include <vector>

namespace dex
{

class Model;

class LiquidFilters;
class LiquidStringifier;

class DEX_OUTPUT_API LiquidExporter : public liquid::Renderer
{
public:

  LiquidExporter();
  ~LiquidExporter();

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

  void setVariables(liquid::Map obj);
  const liquid::Map& variables() const;

  void render();

  static void trim_right(std::string& str);
  static void simplify_empty_lines(std::string& str);

  void setModel(std::shared_ptr<Model> model);
  std::shared_ptr<Model> model() const;
  json::Object serializedModel() const;
  const JsonExportMapping& modelMapping() const;

protected:

  void annotateModel();
  std::string get_url(const dex::Entity& e) const;
  std::string get_url(const dex::Document& doc) const;
  std::string get_url(const std::shared_ptr<model::Object>& obj) const;

protected:
  friend class LiquidExporterModelVisitor;

  void dump(const std::shared_ptr<model::Object>& obj, const char* obj_field_name, const Profile::Template& tmplt);

  void dump(dex::Class& cla);
  void dump(dex::Namespace& ns);
  void dump(dex::Document& doc);

protected:
  std::string stringify(const liquid::Value& val) override;
  liquid::Value applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args) override;

protected:

  void selectStringifier(const std::string& filesuffix);
  void setupContext(liquid::Map& context);
  void postProcess(std::string& output);
  void write(const std::string& data, const std::string& filepath);

private:
  QDir m_output_dir;
  std::shared_ptr<Model> m_model;
  json::Object m_serialized_model;
  JsonExportMapping m_model_mapping;
  Profile m_profile;
  liquid::Map m_user_variables;
  std::map<std::string, std::shared_ptr<LiquidStringifier>> m_stringifiers;
  std::shared_ptr<LiquidStringifier> m_stringifier;
  std::unique_ptr<LiquidFilters> m_filters;
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
