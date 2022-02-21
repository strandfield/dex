// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/config.h"

#include <yaml-cpp/yaml.h>

namespace dex
{

json::Json yaml_to_json(const YAML::Node& n)
{
  if (n.IsScalar())
  {
    const std::string& s = n.Scalar();

    if (s == "true")
      return true;
    else if (s == "false")
      return false;

    // @TODO: convert int/float ?

    return s;
  }
  else if (n.IsSequence())
  {
    json::Array vec;

    for (size_t i = 0; i < n.size(); ++i)
    {
      vec.push(yaml_to_json(n[i]));
    }

    return vec;
  }
  else if (n.IsMap())
  {
    json::Object obj;

    for (YAML::const_iterator it = n.begin(); it != n.end(); ++it)
    {
      obj[it->first.as<std::string>()] = yaml_to_json(it->second);
    }

    return obj;
  }
  else
  {
    return json::null;
  }
}

json::Json read_output_config(const std::filesystem::path& p)
{
  YAML::Node n = YAML::LoadFile(p.string());
  return yaml_to_json(n);
}

} // namespace dex
