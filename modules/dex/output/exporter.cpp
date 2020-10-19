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

namespace dex
{

Exporter::Exporter()
{

}

void Exporter::process(const std::shared_ptr<dex::Model>& model, const QString& name, const json::Object& values)
{
  QFileInfo info{ name };

  QString profile = info.suffix();

  std::string profile_config_file = QDir{ ":/templates/" + profile }.absoluteFilePath("config.ini").toStdString();
  dex::SettingsMap config = dex::settings::load(profile_config_file);
  std::string engine = std::get<std::string>(config["engine"]);

  for (;;)
  {
    if (engine == "delegate")
    {
      profile = QString::fromStdString(std::get<std::string>(config["profile"]));
      profile_config_file = QDir{ ":/templates/" + profile }.absoluteFilePath("config.ini").toStdString();
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
      prof.load(QDir{ ":/templates/" + profile });

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
