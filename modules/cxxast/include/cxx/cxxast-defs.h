// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_DEFS_H
#define CXXAST_DEFS_H

#if (defined(WIN32) || defined(_WIN32)) && !defined(CXXAST_LIB_STATIC_LINKING)

#if defined(CXXAST_BUILD_LIB)
#  define CXXAST_API __declspec(dllexport)
#else
#  define CXXAST_API __declspec(dllimport)
#endif

#else

#  define CXXAST_API

#endif

#endif // CXXAST_DEFS_H
