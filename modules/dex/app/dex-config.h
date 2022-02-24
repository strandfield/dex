// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_CONFIG_H
#define DEX_APP_CONFIG_H

#include "dex/dex-app.h"

#include <json-toolkit/json.h>

#include <QStringList>

class QFileInfo;

namespace dex
{

struct Config
{
  bool valid = false;
  QStringList inputs;
  QString output;
  json::Object variables;
};

DEX_APP_API Config parse_config(const QFileInfo& file);
DEX_APP_API Config parse_config();

} // namespace dex

#endif // DEX_APP_CONFIG_H
