// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex-config.h"

#include "dex/common/json-utils.h"
#include "dex/output/config.h"

#include <QFileInfo>

namespace dex
{

Config parse_config(const QFileInfo& file)
{
  if (!file.exists())
    return {};

  json::Json conf = dex::parse_yaml_config(file.absoluteFilePath().toStdString());

  Config result;
  result.valid = true;

  json::Json in = dex::config::read(conf, "input");

  if (in.isString())
  {
    result.inputs.append(QString::fromStdString(in.toString()));
  }
  else if (in.isArray())
  {
    json::Array list = in.toArray();

    for (int i(0); i < list.length(); ++i)
    {
      const std::string& entry = list.at(i).toString();

      if (!entry.empty())
        result.inputs.append(QString::fromStdString(entry));
    }
  }

  {
    std::string str = dex::config::read(conf, "output", "").toString();
    result.output = QString::fromStdString(str);
  }

  result.variables = conf["variables"].toObject();

  if (result.suffixes.isEmpty())
    result.suffixes << "cxx" << "cpp" << "h" << "hpp";

  // @TODO: detect unused fields in conf

  return result;
}

Config parse_config()
{
  QFileInfo info{ "dex.yml" };
  return parse_config(info);
}

} // namespace dex
