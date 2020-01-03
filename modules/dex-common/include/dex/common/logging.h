// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_COMMON_LOGGING_H
#define DEX_COMMON_LOGGING_H

#include "dex/dex-common.h"

#include <json-toolkit/json.h>

#include <chrono>
#include <memory>
#include <string>

namespace dex
{

namespace log
{

enum Severity
{
  Output,
  Debug,
  Info,
  Warning,
  Error,
  Fatal,
};

} // namespace log

class DEX_COMMON_API Logger
{
public:
  Logger(log::Severity type, std::string file = {}, int line = -1);
  Logger(const Logger&) = delete;
  Logger(Logger&& other) noexcept;
  ~Logger();

  const std::string& file() const noexcept;
  int line() const noexcept;
  std::chrono::system_clock::time_point time() const noexcept;
  log::Severity severity() const noexcept;

  const json::Json& message() const noexcept;

  Logger& operator=(const Logger&) = delete;

  Logger& operator<<(int num);
  Logger& operator<<(const char* str);
  Logger& operator<<(const std::string& str);
  Logger& operator<<(const json::Json& mssg);

private:
  std::string m_file;
  int m_line;
  std::chrono::system_clock::time_point m_time;
  log::Severity m_severity;
  json::Json m_mssg;
};

typedef std::shared_ptr<Logger> LoggerInstance;

template<typename T>
Logger& operator<<(const LoggerInstance& logger, T&& data)
{
    (*logger) << std::forward<T>(data);
    return *logger;
}

namespace log
{

DEX_COMMON_API LoggerInstance out();
DEX_COMMON_API LoggerInstance out(std::string file, int line);

DEX_COMMON_API LoggerInstance error();
DEX_COMMON_API LoggerInstance error(std::string file, int line);

DEX_COMMON_API LoggerInstance info();
DEX_COMMON_API LoggerInstance info(std::string file, int line);

DEX_COMMON_API LoggerInstance warning();
DEX_COMMON_API LoggerInstance warning(std::string file, int line);

typedef void(*MessageHandler)(Severity, Logger&, const json::Json&);

DEX_COMMON_API MessageHandler message_handler();
DEX_COMMON_API MessageHandler install_message_handler(MessageHandler handler);

} // namespace log

} // namespace dex

#define LOG_INFO dex::log::info(__FILE__, __LINE__)
#define LOG_WARNING dex::log::warning(__FILE__, __LINE__)
#define LOG_ERROR dex::log::error(__FILE__, __LINE__)
#define LOG_OUTPUT dex::log::output(__FILE__, __LINE__)

#endif // DEX_COMMON_LOGGING_H
