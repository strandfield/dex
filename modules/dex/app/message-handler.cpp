// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/message-handler.h"

#include <json-toolkit/stringify.h>

#include <iostream>

namespace dex
{

void app_message_handler(log::Severity type, Logger& logger, const json::Json& value)
{
  switch (type)
  {
  case log::Debug:
  {
    if (!logger.file().empty())
    {
      std::cout << logger.file() << ":" << logger.line() << ": ";
    }

    if (value.isString())
    {
      std::cout << value.toString() << std::endl;
    }
    else
    {
      std::cout << json::stringify(value) << std::endl;
    }
  }
  break;
  case log::Error:
  case log::Fatal:
  {
    if (!logger.file().empty())
    {
      std::cerr << logger.file() << ":" << logger.line() << ": ";
    }

    if (value.isString())
    {
      std::cerr << value.toString() << std::endl;
    }
    else
    {
      std::cout << json::stringify(value) << std::endl;
    }
  }
  break;
  case log::Info:
  case log::Output:
  {
    if (value.isString())
    {
      std::cout << value.toString() << std::endl;
    }
    else
    {
      std::cout << json::stringify(value) << std::endl;
    }
  }
  break;
  default:
    break;
  }
}

} // namespace dex
