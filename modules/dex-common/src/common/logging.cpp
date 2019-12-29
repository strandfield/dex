// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/common/logging.h"

namespace dex
{

namespace log
{

static MessageHandler m_static_message_handler = nullptr;

Logger out()
{
  return Logger{ Output };
}

Logger out(std::string file, int line)
{
  return Logger{ Output, std::move(file), line };
}

Logger error()
{
  return Logger{ Error };
}

Logger error(std::string file, int line)
{
  return Logger{ Error, std::move(file), line };
}

Logger info()
{
  return Logger{ Info };
}

Logger info(std::string file, int line)
{
  return Logger{ Info, std::move(file), line };
}

Logger warning()
{
  return Logger{ Warning };
}

Logger warning(std::string file, int line)
{
  return Logger{ Warning, std::move(file), line };
}

MessageHandler message_handler()
{
  return m_static_message_handler;
}

MessageHandler install_message_handler(MessageHandler handler)
{
  MessageHandler ret = m_static_message_handler;
  m_static_message_handler = handler;
  return ret;
}

} // namespace log

Logger::Logger(log::Severity type, std::string file, int line)
  : m_file(std::move(file)),
    m_line(line),
    m_time(std::chrono::system_clock::now()),
    m_severity(type),
    m_mssg(nullptr)
{

}

Logger::Logger(Logger&& other) noexcept
  : m_file(std::move(other.m_file)),
    m_line(other.m_line),
    m_time(other.m_time),
    m_severity(other.m_severity),
    m_mssg(other.m_mssg)
{
  other.m_mssg = nullptr;
}

Logger::~Logger()
{
  if (m_mssg != nullptr && log::message_handler() != nullptr)
  {
    log::message_handler()(m_severity, *this, m_mssg);
  }
}

const std::string& Logger::file() const noexcept
{
  return m_file;
}

int Logger::line() const noexcept
{
  return m_line;
}

std::chrono::system_clock::time_point Logger::time() const noexcept
{
  return m_time;
}

log::Severity Logger::severity() const noexcept
{
  return m_severity;
}

const json::Json& Logger::message() const noexcept
{
  return m_mssg;
}

Logger& Logger::operator<<(int num)
{
  if (m_mssg == nullptr || !m_mssg.isString())
  {
    m_mssg = std::to_string(num);
  }
  else
  {
    m_mssg = m_mssg.toString() + std::to_string(num);
  }

  return *this;
}

Logger& Logger::operator<<(const char* str)
{
  if (m_mssg == nullptr || !m_mssg.isString())
  {
    m_mssg = std::string(str);
  }
  else
  {
    m_mssg = m_mssg.toString() + std::string(str);
  }

  return *this;
}

Logger& Logger::operator<<(const std::string& str)
{
  if (m_mssg == nullptr || !m_mssg.isString())
  {
    m_mssg = str;
  }
  else
  {
    m_mssg = m_mssg.toString() + str;
  }

  return *this;
}

Logger& Logger::operator<<(const json::Json& mssg)
{
  if (m_mssg == nullptr)
  {
    m_mssg = mssg;
  }
  else if (m_mssg.isArray())
  {
    m_mssg.toArray().push(mssg);
  }
  else
  {
    m_mssg = mssg;
  }

  return *this;
}

} // namespace dex
