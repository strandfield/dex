// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/exporter.h"

#include "dex/output/json-export.h"
#include "dex/output/latex-export.h"
#include "dex/output/markdown-export.h"

#include "dex/common/errors.h"
#include "dex/common/settings.h"

#include <json-toolkit/stringify.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace dex
{

Exporter::Exporter()
{

}

static void recursive_copy(const QString& src, const QString& dest)
{
  QFileInfo src_file_info{ src };

  if (src_file_info.isDir())
  {
    QDir target_dir{ dest };
    target_dir.cdUp();
    target_dir.mkdir(QFileInfo(dest).fileName());

    QDir src_dir{ src };

    QStringList filenames = src_dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& filename : filenames)
    {
      const QString nested_src = src + "/" + filename;
      const QString nested_dest = dest + "/" + filename;
      recursive_copy(nested_src, nested_dest);
    }
  }
  else 
  {
    QFile::copy(src, dest);
  }
}

void Exporter::copyProfiles()
{
  QString dest = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir dest_dir{ dest };

  if (!dest_dir.exists())
  {
    QDir upper_dir = dest_dir;
    upper_dir.cdUp();
    upper_dir.mkpath(dest);
  }

  dest_dir = QDir(dest + "/profiles");

  if (!dest_dir.exists())
  {
    QDir upper_dir = dest_dir;
    upper_dir.cdUp();
    upper_dir.mkpath(dest + "/profiles");
  }

  auto cp = [dest](const QString& src) {
    if(!QFileInfo(dest + "/profiles/" + src).exists())
      recursive_copy(":/templates/" + src, dest + "/profiles/" + src);
  };

  cp("json");
  cp("md");
  cp("markdown");
  cp("tex");
  cp("latex");
}

void Exporter::process(const std::shared_ptr<dex::Model>& model, const QString& name, const json::Object& values)
{
  QFileInfo info{ name };

  QString profile = info.suffix();
  const QString profiles_dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/profiles";
  QString profile_dir = profiles_dir + "/" + profile;

  std::string profile_config_file = QDir{ profile_dir }.absoluteFilePath("config.ini").toStdString();
  dex::SettingsMap config = dex::settings::load(profile_config_file);
  std::string engine = std::get<std::string>(config["engine"]);

  for (;;)
  {
    if (engine == "delegate")
    {
      profile = QString::fromStdString(std::get<std::string>(config["profile"]));
      profile_dir = profiles_dir + "/" + profile;
      profile_config_file = QDir{ profile_dir }.absoluteFilePath("config.ini").toStdString();
      config = dex::settings::load(profile_config_file);
      engine = std::get<std::string>(config["engine"]);
    }
    else if (engine == "json")
    {
      auto obj = dex::JsonExport::serialize(*model);

      QFile file{ name };

      if (!file.open(QIODevice::WriteOnly))
        throw IOException{ name.toStdString(), "could not open file for writing" };

      file.write(QByteArray::fromStdString(json::stringify(obj)));

      return;
    }
    else if (engine == "liquid")
    {
      dex::LiquidExporter exporter;

      LiquidExporterProfile prof;
      prof.load(QDir{ profile_dir });

      exporter.setProfile(std::move(prof));
      exporter.setVariables(values);
      exporter.setOutputDir(info.dir());
      exporter.setModel(model);

      exporter.render();

      return;
    }
    else
    {
      throw std::runtime_error{ "Unknown export type" };
    }
  }
}

} // namespace dex
