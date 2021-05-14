// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/ini-config-parser.h"

#include "dex/common/json-utils.h"
#include "dex/common/logging.h"
#include "dex/common/settings.h"
#include "dex/common/string-utils.h"

#include <QFileInfo>

namespace dex
{

IniOptions parse_ini_config(const QFileInfo& file)
{
  if (!file.exists())
    return {};

  SettingsMap settings = dex::settings::load(file.absoluteFilePath().toStdString());

  IniOptions result;
  result.valid = true;

  {
    QString inputlist = QString::fromStdString(dex::settings::read<std::string>(settings, "input", ""));
    result.inputs = inputlist.split(';', Qt::SkipEmptyParts);
  }

  {
    std::string str = dex::settings::read<std::string>(settings, "output", "");
    result.output = QString::fromStdString(str);
  }

  SettingsMap variables;

  for (auto it = settings.begin(); it != settings.end();)
  {
    const auto& e = *it;

    if (StdString(e.first).starts_with("variables/"))
    {
      std::string key = e.first;
      SettingsValue val = e.second;

      it = settings.erase(it);

      key.erase(0, 10);
      variables[key] = val;
    }
    else
    {
      ++it;
    }
  }

  result.variables = build_json(variables);

  if (!settings.empty())
  {
    LOG_INFO << "some settings from " << file.filePath().toStdString() << " have not been processed";
  }

  return result;
}

IniOptions parse_ini_config()
{
  QFileInfo info{ "dex.ini" };
  return parse_ini_config(info);
}

} // namespace dex
