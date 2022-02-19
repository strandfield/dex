// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter-profile.h"

#include "dex/common/errors.h"
#include "dex/common/file-utils.h"
#include "dex/common/settings.h"
#include "dex/common/string-utils.h"

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

static json::Json yaml_to_json(const YAML::Node& n)
{
  if (n.IsScalar())
  {
    const std::string& s = n.Scalar();

    if (s == "true")
      return true;
    else if (s == "false")
      return false;
    
    // @TODO: convert int/float ?

    return s;
  }
  else if (n.IsSequence())
  {
    json::Array vec;

    for (size_t i = 0; i < n.size(); ++i) 
    {
      vec.push(yaml_to_json(n[i]));
    }

    return vec;
  }
  else if (n.IsMap())
  {
    json::Object obj;

    for (YAML::const_iterator it = n.begin(); it != n.end(); ++it) 
    {
      obj[it->first.as<std::string>()] = yaml_to_json(it->second);
    }

    return obj;
  }
  else
  {
    return json::null;
  }
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
  std::set<std::string> exclusions;
  SettingsMap settings;

public:
  LiquidExporterProfileLoader(LiquidExporterProfile& pro, const QDir& dir)
    : profile(pro),
      directory(dir)
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
    tmplt.outdir = dex::settings::read(settings, "output/" + name, std::move(default_out));
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

  void list_files()
  {
    QDirIterator diriterator{ directory.absolutePath(), QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories };

    while (diriterator.hasNext())
    {
      std::string path = diriterator.next().toStdString();

      if (excluded(path))
        continue;

      liquid::Template tmplt = open_liquid_template(path);
      tmplt.skipWhitespacesAfterTag();

      path.erase(path.begin(), path.begin() + profile.profile_path.length() + 1);
      profile.files.emplace_back(std::move(path), std::move(tmplt));
    }
  }

public:

  void load()
  {
    if (!directory.exists("config.ini"))
      throw std::runtime_error{ "Bad profile directory" };

    profile.profile_path = directory.absolutePath().toStdString();

    std::string profile_config_file = directory.absoluteFilePath("config.ini").toStdString();
    exclude(profile_config_file);
    settings = dex::settings::load(profile_config_file);

    list_templates();
    list_liquid_includes();
    list_files();
  }
};

void LiquidExporterProfile::load(const QDir& dir)
{
  LiquidExporterProfileLoader loader{ *this, dir };
  loader.load();
}

} // namespace dex
