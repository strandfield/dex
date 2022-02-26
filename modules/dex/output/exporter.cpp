// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/exporter.h"

#include "dex/output/config.h"
#include "dex/output/json-export.h"
#include "dex/output/liquid/liquid-exporter.h"

#include "dex/common/errors.h"

#include <json-toolkit/stringify.h>

#include <QDir>

namespace dex
{

void run_exporter(const std::shared_ptr<dex::Model>& model, const QString& outdirpath, const json::Object& values)
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
    dex::LiquidExporter exporter{ outdir.absolutePath().toStdString(), config };

    exporter.setVariables(values);
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
