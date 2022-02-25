// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_VERSION_H
#define DEX_APP_VERSION_H

#define DEX_VERSION_STR "0.0.0"
#define DEX_VERSION_MAJOR 0
#define DEX_VERSION_MINOR 0
#define DEX_VERSION_PATCH 0

#include "dex/dex-app.h"

namespace dex
{

DEX_APP_API const char* versionstr();
DEX_APP_API int version_major();
DEX_APP_API int version_minor();
DEX_APP_API int version_patch();

} // namespace dex

#endif // DEX_APP_VERSION_H
