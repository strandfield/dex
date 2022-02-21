// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/exporter.h"

#include "dex/output/config.h"
#include "dex/output/dir-copy.h"
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

void Exporter::clearProfiles()
{
  QString dest = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir(dest + "/profiles").removeRecursively();
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
    if(!QFileInfo::exists(dest + "/profiles/" + src))
      recursive_copy(":/templates/" + src, dest + "/profiles/" + src);
  };

  cp("default");
  cp("json");
  cp("md");
  cp("markdown");
  cp("tex");
  cp("latex");
}

static liquid::Value json_to_liquid(const json::Json& js)
{
  if (js.isInteger())
    return js.toInt();
  else if (js.isBoolean())
    return js.toBool();
  else if (js.isNumber())
    return js.toNumber();
  else if (js.isString())
    return js.toString();

  if (js.isObject())
  {
    liquid::Map r = {};

    for (const auto& entry : js.toObject().data())
      r[entry.first] = json_to_liquid(entry.second);

    return r;
  }
  else
  {
    liquid::Array r = {};

    for (const auto& entry : js.toArray().data())
      r.push(json_to_liquid(entry));

    return r;
  }
}

void Exporter::process(const std::shared_ptr<dex::Model>& model, const QString& outdirpath, const json::Object& values)
{
  QDir outdir{ outdirpath };

  if (!outdir.exists())
    throw std::runtime_error("No such directory " + outdirpath.toStdString());

  std::string profile_config_file = outdir.absoluteFilePath("_config.yml").toStdString();
  json::Json config = dex::read_output_config(profile_config_file);
  std::string engine = config["engine"].toString();

  if (engine == "json")
  {
    auto obj = dex::JsonExporter::serialize(*model);

    outdir.mkpath("_output");

    QFile file{ outdirpath + "/_output/dex.json" };

    if (!file.open(QIODevice::WriteOnly))
      throw IOException{ file.fileName().toStdString(), "could not open file for writing" };

    file.write(QByteArray::fromStdString(json::stringify(obj)));

    return;
  }
  else if (engine == "liquid")
  {
    dex::LiquidExporter exporter;

    LiquidExporterProfile prof;
    prof.load(outdir, config);

    exporter.setProfile(std::move(prof));
    exporter.setVariables(json_to_liquid(values).toMap());
    exporter.setModel(model);

    exporter.render();

    return;
  }
  else
  {
    throw std::runtime_error{ "Unknown export type" };
  }
}

} // namespace dex
