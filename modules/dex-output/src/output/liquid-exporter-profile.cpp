// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter-profile.h"

#include "dex/common/errors.h"
#include "dex/common/file-utils.h"
#include "dex/common/settings.h"

#include <QDirIterator>

#include <set>

namespace dex
{

static liquid::Template open_liquid_template(const std::string& path)
{
  std::string tmplt = file_utils::read_all(path);
  return liquid::parse(tmplt);
}

void LiquidExporterProfile::load(const QDir& dir)
{
  if (!dir.exists("config.ini"))
    throw std::runtime_error{ "Bad profile directory" };

  std::set<std::string> exclusions;
  exclusions.insert(dir.absoluteFilePath("config.ini").toStdString());

  SettingsMap settings = dex::settings::load(dir.absoluteFilePath("config.ini").toStdString());

  std::string path = dex::settings::read(settings, "templates/class", std::string());

  if (!path.empty())
  {
    path = dir.absolutePath().toStdString() + "/" + path;
    this->class_template = open_liquid_template(path);
    exclusions.insert(path);

    this->class_outdir = dex::settings::read(settings, "output/class", std::string("classes"));
  }

  QDirIterator diriterator{ dir.absolutePath(), QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories };

  while (diriterator.hasNext()) 
  {
    std::string path = diriterator.next().toStdString();

    if (exclusions.find(path) != exclusions.end())
      continue;

    liquid::Template tmplt = open_liquid_template(path);
    this->files.emplace_back(std::move(path), std::move(tmplt));
  }
}

} // namespace dex
