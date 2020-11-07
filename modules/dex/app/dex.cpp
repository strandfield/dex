// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/message-handler.h"

#include "dex/common/json-utils.h"
#include "dex/input/parser-machine.h"
#include "dex/output/exporter.h"

#include <cxx/libclang.h>

#include <QDir>

#include <iostream>

namespace dex
{

Dex::Dex(int& argc, char* argv[])
  : QCoreApplication(argc, argv)
{
  setApplicationName("dex");
  setApplicationVersion("0.0.0");

  m_suffixes << "cxx" << "cpp" << "h" << "hpp";

  dex::log::install_message_handler(&dex::app_message_handler);
}

int Dex::exec()
{
  CommandLineParser parser;
  m_cli = parser.parse(Dex::arguments());
  auto& result = m_cli;

  if (result.status == CommandLineParserResult::ParseError)
  {
    std::cout << result.error.toStdString() << std::endl;
    return 1;
  }
  
  if (result.status == CommandLineParserResult::HelpRequested)
  {
    std::cout << parser.help().toStdString() << std::endl;
  }
  else if (result.status == CommandLineParserResult::VersionRequested)
  {
    std::cout << Dex::applicationVersion().toStdString() << std::endl;
  }
  else if (result.status == CommandLineParserResult::ClangVersionRequested)
  {
    std::cout << Dex::libClangVersion() << std::endl;
  }
  else if (result.status == CommandLineParserResult::Work)
  {
    work();
  }

  return 0;
}

void Dex::work()
{
  m_ini = parse_ini_config();

  QStringList inputs = m_ini.inputs;

  if (!m_cli.inputs.empty())
    inputs = m_cli.inputs;

  QString output = m_ini.output;

  if (!m_cli.output.isEmpty())
    output = m_cli.output;

  json::Object values = m_ini.variables;

  if (!m_cli.values.data().empty())
    values = m_cli.values;

  process(inputs, output, values);
}

std::string Dex::libClangVersion() const
{
  try
  {
    cxx::LibClang libclang;
    CXVersion ver = libclang.version();

    return std::to_string(ver.Major) + "." + std::to_string(ver.Minor) + "." + std::to_string(ver.Subminor);
  }
  catch (const cxx::LibClangError&)
  {
    return "libclang could not be found";
  }
}

void Dex::process(const QStringList& inputs, QString output, json::Object values)
{
  dex::ParserMachine parser;

  if (!inputs.empty())
  {
    for (const auto& i : inputs)
    {
      try
      {
        feed(parser, i);
      }
      catch (const IOException& ex)
      {
        LOG_ERROR << ex;
      }
    }
  }
  else
  {
    feed(parser, QDir::current());
  }

  if (output.isEmpty())
    output = "dex-output.json";

  write_output(parser.output(), output, values);
}

void Dex::feed(ParserMachine& parser, const QString& input)
{
  QFileInfo info{ input };

  if (!info.exists())
    throw IOException{ input.toStdString(), "input file does not exist" };

  if (info.isDir())
  {
    QDir dir{ info.absoluteFilePath() };
    feed(parser, dir);
  }
  else
  {
    try
    {
      parser.process(info);
    }
    catch (const ParserException& ex)
    {
      LOG_ERROR << ex;

      const bool success = parser.recover();

      if (!success)
        parser.reset();
    }
  }
}

void Dex::feed(ParserMachine& parser, const QDir& input)
{
  QFileInfoList entries = input.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

  for (const auto& e : entries)
  {
    if (e.isDir())
    {
      feed(parser, QDir{ e.absoluteFilePath() });
    }
    else
    {
      if (m_suffixes.contains(e.suffix()))
        feed(parser, e.absoluteFilePath());
    }
  }
}

void Dex::write_output(const std::shared_ptr<Model>& model, const QString& name, json::Object values)
{
  Exporter exporter;
  exporter.copyProfiles();
  exporter.process(model, name, values);
}

} // namespace dex
