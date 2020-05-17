// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_DEFS_H
#define DEX_OUTPUT_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(DEX_OUTPUT_LIB_STATIC_LINKING)

#if defined(DEX_OUTPUT_BUILD_LIB)
#  define DEX_OUTPUT_API __declspec(dllexport)
#else
#  define DEX_OUTPUT_API __declspec(dllimport)
#endif

#else

#  define DEX_OUTPUT_API

#endif

#endif // DEX_OUTPUT_DEFS_H
