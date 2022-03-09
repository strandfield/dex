// Copyright (C) 2019-2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/common/file-utils.h"

#include <QFile>

#include <fstream>
#include <sstream>

namespace dex
{

namespace file_utils
{

bool is_embed_resource(const std::filesystem::path& p)
{
  return p.string().find(":/") == 0;
}

std::string read_all(const std::filesystem::path& p)
{
  if (!is_embed_resource(p))
  {
    std::ifstream file{ p.string() };
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
  else
  {
    QFile file{ QString::fromStdString(p.string()) };
    file.open(QIODevice::ReadOnly);
    return file.readAll().toStdString();
  }
}

void write_file(const std::filesystem::path& p, const std::string& data)
{
  std::ofstream file{ p.string(), std::ios::out | std::ios::trunc };
  file.write(data.c_str(), data.size());
}

void remove(const std::filesystem::path& p)
{
  std::filesystem::remove(p);
}

void crlf2lf(std::string& str)
{
  size_t r = 0;
  size_t w = 0;

  while (r < str.size())
  {
    if (str.at(r) == '\r')
      ++r;
    else
      str[w++] = str[r++];
  }

  str.resize(w);
}

} // namespace file_utils

} // namespace dex
