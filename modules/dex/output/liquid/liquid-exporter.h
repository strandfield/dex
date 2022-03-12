// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_H
#define DEX_OUTPUT_LIQUID_EXPORTER_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <liquid/renderer.h>

#include <json-toolkit/json.h>

#include <filesystem>
#include <map>
#include <variant>
#include <vector>

namespace dex
{

class Model;

class LiquidFilters;
class LiquidStringifier;

liquid::Value json_to_liquid(const json::Json& js);

liquid::Template open_liquid_template(const std::filesystem::path& path);

struct TemplateWithFrontMatter
{
  json::Object frontmatter;
  liquid::Template model;
};

TemplateWithFrontMatter open_template_with_front_matter(const std::filesystem::path& path);

struct LiquidLayout
{
  liquid::Template model;
  std::string outdir;
  std::string filesuffix;
};

class DEX_OUTPUT_API LiquidExporter : public liquid::Renderer
{
public:

  explicit LiquidExporter(std::string folder_path, const json::Json config = json::null);
  ~LiquidExporter();

  struct Layouts
  {
    LiquidLayout class_template;
    LiquidLayout namespace_template;
    LiquidLayout document_template;
  };

  const std::string& folderPath() const;

  const std::string& outputPath() const;
  std::filesystem::path outputDir() const;

  const Layouts& layouts() const;

  void setVariables(const json::Object& obj);
  void setVariables(liquid::Map obj);
  const liquid::Map& variables() const;

  void render();

  static void trim_right(std::string& str);
  static void simplify_empty_lines(std::string& str);

  void setModel(std::shared_ptr<Model> model);
  std::shared_ptr<Model> model() const;

  std::string get_url(const dex::Entity& e) const;
  std::string get_url(const dex::Document& doc) const;
  std::string get_url(const std::shared_ptr<model::Object>& obj) const;

protected:
  friend class LiquidExporterModelVisitor;

  void dump(const std::shared_ptr<model::Object>& obj, const char* obj_field_name, const LiquidLayout& layout);

  void dump(dex::Class& cla);
  void dump(dex::Namespace& ns);
  void dump(dex::Document& doc);

protected:
  std::string stringify(const liquid::Value& val) override;
  liquid::Value applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args) override;

protected:

  void listLayouts();
  LiquidLayout parseLayout(const std::filesystem::path& fileinfo, const std::string& name, std::string default_out);
  void listIncludes();

  void renderDirectory(const std::filesystem::path& path);
  void renderFile(const std::filesystem::path& filepath);
  bool isSpecialFile(const std::filesystem::path& fileinfo) const;
  void selectStringifier(const std::string& filesuffix);
  void setupContext(liquid::Map& context);
  void postProcess(std::string& output);
  void checkWriteDirectory(const std::filesystem::path& filepath);
  void write(const std::string& data, const std::filesystem::path& filepath);

private:
  std::string m_folder_path;
  std::string m_output_path;
  json::Json m_config;
  std::shared_ptr<Model> m_model;
  Layouts m_layouts;
  liquid::Map m_user_variables;
  std::map<std::string, std::shared_ptr<LiquidStringifier>> m_stringifiers;
  std::shared_ptr<LiquidStringifier> m_stringifier;
  std::unique_ptr<LiquidFilters> m_filters;
};

} // namespace dex

namespace dex
{

inline std::shared_ptr<Model> LiquidExporter::model() const
{
  return m_model;
}

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_H
