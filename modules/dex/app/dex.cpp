// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/message-handler.h"

#include "dex/common/json-utils.h"
#include "dex/input/parser-machine.h"
#include "dex/output/exporter.h"

#include <QDir>

#include <iostream>

namespace dex
{

const char* versionstr()
{
  return DEX_VERSION_STR;
}

int version_major()
{
  return DEX_VERSION_MAJOR;
}

int version_minor()
{
  return DEX_VERSION_MINOR;
}

int version_patch()
{
  return DEX_VERSION_PATCH;
}


Dex::Dex(const QStringList& arguments)
  : m_arguments(arguments)
{
  m_suffixes << "cxx" << "cpp" << "h" << "hpp";

  dex::log::install_message_handler(&dex::app_message_handler);
}

int Dex::exec()
{
  CommandLineParser parser;
  m_cli = parser.parse(m_arguments);
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
    std::cout << dex::versionstr() << std::endl;
  }
  else if (result.status == CommandLineParserResult::Work)
  {
    work();
  }

  return 0;
}

void Dex::work()
{
  if (m_cli.workdir.has_value())
  {
    QString workdir = m_cli.workdir.value();
    log::info() << "Changing working dir to '" << workdir.toStdString() << "'";
    
    if (!QDir::setCurrent(workdir))
    {
      log::error() << "Failed to change working dir";
      return;
    }
  }

  m_config = dex::parse_config();

  if (!m_config.valid)
  {
    log::info() << "Could not parse dex.yml config";
  }

  process(m_config.inputs, m_config.output, m_config.variables);
}

// @TODO: split into two functions:
// one that takes the 'inputs' and produce a Model
// one that takes the model and the output directory
void Dex::process(const QStringList& inputs, QString output, json::Object values)
{
  dex::ParserMachine parser;

  if (!inputs.empty())
  {
    log::info() << "Inputs:";
    for (const auto& i : inputs)
    {
      log::info() << i.toStdString();
    }

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
  {
    log::info() << "No output specified";
    return;
  }

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
      log::info() << "Parsing " << info.filePath().toStdString();
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

void Dex::write_output(const std::shared_ptr<Model>& model, const QString& outdir, json::Object values)
{
  assert(!outdir.isEmpty());

  log::info() << "Writing output to '" << outdir.toStdString() << "'";

  dex::run_exporter(model, outdir, values);
}

} // namespace dex
