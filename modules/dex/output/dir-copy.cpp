// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/dir-copy.h"

namespace dex
{

void recursive_copy(const std::filesystem::path& src, const std::filesystem::path& dest)
{
  std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive);
}

} // namespace dex
