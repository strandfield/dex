// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/format.h"

#include "dex/common/file-utils.h"

#include <tex/lexer.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

std::vector<tex::parsing::Macro> DexFormat::load()
{
  std::string source = file_utils::read_all(":/dex.fmt");
  file_utils::crlf2lf(source);
  return tex::parsing::Format::parse(source);
}

} // namespace dex
