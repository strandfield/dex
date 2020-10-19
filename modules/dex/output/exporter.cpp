// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/exporter.h"

#include "dex/output/json-export.h"
#include "dex/output/latex-export.h"
#include "dex/output/markdown-export.h"

#include "dex/common/errors.h"

#include <json-toolkit/stringify.h>

#include <QFileInfo>

namespace dex
{

Exporter::Exporter()
{

}

void Exporter::process(const std::shared_ptr<dex::Model>& model, const QString& name, const json::Object& values)
{
  QFileInfo info{ name };

  if (info.suffix() == "json")
  {
    auto obj = dex::JsonExport::serialize(*model);

    QFile file{ name };

    if (!file.open(QIODevice::WriteOnly))
      throw IOException{ name.toStdString(), "could not open file for writing" };

    file.write(QByteArray::fromStdString(json::stringify(obj)));
  }
  else if (info.suffix() == "md" || info.suffix() == "tex")
  {
    dex::LiquidExporter exporter;

    LiquidExporterProfile prof;

    if (info.suffix() == "md")
      prof.load(QDir{ ":/templates/markdown" });
    else
      prof.load(QDir{ ":/templates/latex" });

    exporter.setProfile(std::move(prof));
    exporter.setVariables(values);
    exporter.setOutputDir(info.dir());
    exporter.setModel(model);

    exporter.render();
  }
  else
  {
    throw std::runtime_error{ "Unknown export type" };
  }
}

} // namespace dex
