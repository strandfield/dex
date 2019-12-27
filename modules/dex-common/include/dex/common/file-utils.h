// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_FILE_UTILS_H
#define DEX_COMMON_FILE_UTILS_H

#include "dex/dex-common.h"

#include <filesystem>

namespace dex
{

namespace file_utils
{

DEX_COMMON_API bool is_embed_resource(const std::filesystem::path& p);
DEX_COMMON_API std::string read_all(const std::filesystem::path& p);

} // namespace file_utils

} // namespace dex

#endif // DEX_COMMON_FILE_UTILS_H
