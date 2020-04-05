// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/common/settings.h"

#include <QSettings>

namespace dex
{

namespace settings
{

SettingsMap load(const std::filesystem::path& p)
{
  QSettings initsettings{ QString::fromStdString(p.string()), QSettings::IniFormat };

  SettingsMap result;

  for (QString key : initsettings.allKeys())
  {
    QVariant v = initsettings.value(key);

    auto& settingsvalue = result[key.toStdString()];

    if (v.type() == QVariant::Bool)
      settingsvalue = v.toBool();
    else if (v.type() == QVariant::Int)
      settingsvalue = v.toInt();
    else if (v.type() == QVariant::Double)
      settingsvalue = v.toDouble();
    else
      settingsvalue = v.toString().toStdString();
  }

  return result;
}

} // namespace settings

} // namespace dex
