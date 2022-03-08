// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_CONFIG_H
#define DEX_OUTPUT_CONFIG_H

#include "dex/dex-output.h"

#include "dex/common/yaml-config.h"

namespace dex
{

DEX_OUTPUT_API json::Json read_output_config(const std::filesystem::path& p);

} // namespace dex

#endif // DEX_OUTPUT_CONFIG_H
