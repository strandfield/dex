// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/config.h"

namespace dex
{

json::Json read_output_config(const std::filesystem::path& p)
{
  return parse_yaml_config(p);
}

} // namespace dex
