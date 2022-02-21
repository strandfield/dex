// Copyright (C) 2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_DIRCOPY_H
#define DEX_OUTPUT_DIRCOPY_H

#include "dex/dex-output.h"

#include <QString>

namespace dex
{

DEX_OUTPUT_API void recursive_copy(const QString& src, const QString& dest);

} // namespace dex

#endif // DEX_OUTPUT_DIRCOPY_H
