// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex-config.h"

#include "dex/output/config.h"

namespace dex
{

Config parse_config(const std::filesystem::path& file)
{
  if (!std::filesystem::exists(file))
    return {};

  json::Json conf = dex::parse_yaml_config(file);

  Config result;
  result.valid = true;

  json::Json in = dex::config::read(conf, "input");

  if (in.isString())
  {
    result.inputs.insert(in.toString());
  }
  else if (in.isArray())
  {
    json::Array list = in.toArray();

    for (int i(0); i < list.length(); ++i)
    {
      const std::string& entry = list.at(i).toString();

      if (!entry.empty())
        result.inputs.insert(entry);
    }
  }

  {
    result.output = dex::config::read(conf, "output", "").toString();
  }

  result.variables = conf["variables"].toObject();

  if (result.suffixes.empty())
    result.suffixes.insert({ "cxx", "cpp" , "h" , "hpp" });

  // @TODO: detect unused fields in conf

  return result;
}

Config parse_config()
{
  return parse_config("dex.yml");
}

} // namespace dex
