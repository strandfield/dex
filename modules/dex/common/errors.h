// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_ERRORS_H
#define DEX_COMMON_ERRORS_H

#include "dex/dex-common.h"

#include <json-toolkit/json.h>

#include <stdexcept>

namespace dex
{

class Logger;

#pragma warning(push)
#pragma warning(disable: 4275)

class DEX_COMMON_API Exception : public std::exception
{
protected:
  std::string what_;

public:

  Exception(std::string str);

  const char* what() const noexcept override;

};

#pragma warning(pop)

class DEX_COMMON_API IOException : public Exception
{
public:
  const std::string path;
  const std::string message;

public:
  IOException(std::string path_, std::string mssg_);
};

DEX_COMMON_API Logger& operator<<(Logger& logger, const Exception& ex);
DEX_COMMON_API Logger& operator<<(Logger& logger, const IOException& ex);

} // namespace dex

#endif // DEX_COMMON_ERRORS_H
