// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/common/errors.h"

#include "dex/common/logging.h"

namespace dex
{

Exception::Exception(std::string str)
  : what_(std::move(str))
{

}

const char* Exception::what() const noexcept
{
  return what_.data();
}

IOException::IOException(std::string path_, std::string mssg_)
  : Exception(mssg_ + ":" + path_),
    path(std::move(path_)),
    message(std::move(mssg_))
{

}

Logger& operator<<(Logger& logger, const Exception& ex)
{
  json::Object obj;

  obj["type"] = "Exception";
  obj["what"] = ex.what();

  logger << obj;

  return logger;
}

Logger& operator<<(Logger& logger, const IOException& ex)
{
  json::Object obj;

  obj["type"] = "IOException";
  obj["what"] = ex.what();

  obj["path"] = ex.path;
  obj["message"] = ex.message;

  logger << obj;

  return logger;
}

} // namespace dex
