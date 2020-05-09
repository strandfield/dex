// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_INICONFIGPARSER_H
#define DEX_APP_INICONFIGPARSER_H

#include "dex/dex-app.h"

#include <json-toolkit/json.h>

#include <QStringList>

class QFileInfo;

namespace dex
{

struct IniOptions
{
  bool valid = false;
  QStringList inputs;
  QString output;
  json::Object variables;
};

DEX_APP_API IniOptions parse_ini_config(const QFileInfo& file);
DEX_APP_API IniOptions parse_ini_config();

} // namespace dex

#endif // DEX_APP_INICONFIGPARSER_H
