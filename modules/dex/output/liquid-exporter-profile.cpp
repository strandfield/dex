// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter-profile.h"

#include "dex/common/errors.h"
#include "dex/common/file-utils.h"
#include "dex/common/settings.h"
#include "dex/common/string-utils.h"
#include "dex/output/config.h"

#include <yaml-cpp/yaml.h>

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

#include <set>

namespace dex
{

liquid::Template open_liquid_template(const std::string& path)
{
  std::string tmplt = file_utils::read_all(path);
  return liquid::parse(tmplt);
}

TemplateWithFrontMatter open_template_with_front_matter(const std::string& path)
{
  QFile file{ path.c_str() };

  file.open(QIODevice::ReadOnly);

  file.readLine();

  QByteArray frontmatter;
  QByteArray l = file.readLine();

  while (!l.startsWith("---"))
  {
    frontmatter += l;
    frontmatter += "\n";
    l = file.readLine();
  }

  YAML::Node yam = YAML::Load(frontmatter.data());
  
  std::string tmplt_content = file.readAll().toStdString();

  TemplateWithFrontMatter result;
  result.frontmatter = yaml_to_json(yam).toObject();
  result.model = liquid::parse(tmplt_content);

  return result;
}

class LiquidExporterProfileLoader
{
public:
  LiquidExporterProfile& profile;
  QDir directory;
  json::Json config;
  std::set<std::string> exclusions;

public:
  LiquidExporterProfileLoader(LiquidExporterProfile& pro, const QDir& dir, const json::Json& conf)
    : profile(pro),
      directory(dir),
      config(conf)
  {

  }

protected:
  bool excluded(const std::string& filepath)
  {
    return exclusions.find(filepath) != exclusions.end();
  }

  void exclude(std::string filepath)
  {
    exclusions.insert(std::move(filepath));
  }

  void exclude(const QString& filepath)
  {
    exclude(filepath.toStdString());
  }

  void read_template(const QFileInfo& fileinfo, const std::string& name, LiquidExporterProfile::Template& tmplt, std::string default_out)
  {
    std::string path = fileinfo.absoluteFilePath().toStdString();
    tmplt.model = open_liquid_template(path);
    tmplt.model.skipWhitespacesAfterTag();
    tmplt.outdir = dex::config::read(config["output"], name, std::move(default_out)).toString();
    tmplt.filesuffix = fileinfo.suffix().toStdString();
  }

  void list_templates()
  {
    QDir dir{ directory.absolutePath() + "/_layouts" };

    if (!dir.exists())
      return;

    exclude(dir.absolutePath());

    QDirIterator diriterator{ dir.absolutePath(), QDir::NoDotAndDotDot | QDir::Files };

    while (diriterator.hasNext())
    {
      QFileInfo fileinfo{ diriterator.next() };
      exclude(fileinfo.absoluteFilePath());

      if (fileinfo.baseName() == "class")
        read_template(fileinfo, "class", profile.class_template, "classes");
      else if (fileinfo.baseName() == "namespace")
        read_template(fileinfo, "namespace", profile.namespace_template, "namespaces");
      else if (fileinfo.baseName() == "document")
        read_template(fileinfo, "document", profile.document_template, "documents");
    }
  }

  void list_liquid_includes()
  {
    QDir dir{ directory.absolutePath() + "/_includes" };

    if (!dir.exists())
      return;

    exclude(dir.absolutePath());

    QDirIterator diriterator{ dir.absolutePath(), QDir::NoDotAndDotDot | QDir::Files };

    while (diriterator.hasNext())
    {
      std::string p = diriterator.next().toStdString();
      exclude(p);
      liquid::Template tmplt = open_liquid_template(p);
      tmplt.skipWhitespacesAfterTag();
      p.erase(p.begin(), p.begin() + profile.profile_path.length() + 11);
      profile.liquid_templates.emplace_back(std::move(p), std::move(tmplt));
    }
  }

public:

  void load()
  {
    assert(directory.exists("_config.yml"));

    profile.profile_path = directory.absolutePath().toStdString();

    std::string profile_config_file = directory.absoluteFilePath("_config.yml").toStdString();
    exclude(profile_config_file);

    list_templates();
    list_liquid_includes();
  }
};

void LiquidExporterProfile::load(const QDir& dir, const json::Json& config)
{
  LiquidExporterProfileLoader loader{ *this, dir, config };
  loader.load();
}

} // namespace dex
