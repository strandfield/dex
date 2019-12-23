// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_DEFS_H
#define DEX_MODEL_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(DEX_MODEL_LIB_STATIC_LINKING)

#if defined(DEX_MODEL_BUILD_LIB)
#  define DEX_MODEL_API __declspec(dllexport)
#else
#  define DEX_MODEL_API __declspec(dllimport)
#endif

#else

#  define DEX_MODEL_API

#endif

#endif // DEX_MODEL_DEFS_H
