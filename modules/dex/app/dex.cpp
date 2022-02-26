// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/message-handler.h"
#include "dex/app/parsing.h"

#include "dex/common/json-utils.h"
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
{
  CommandLineParser parser;
  m_cli = parser.parse(arguments);
  auto& result = m_cli;

  // @TODO: throw if not CommandLineParserResult::Work ?

}

Dex::Dex(const QDir& workdir)
  : m_workdir(workdir)
{

}

int Dex::exec()
{
  if (m_cli.has_value())
  {
    auto& result = *m_cli;

    // @TODO: should action other than work be moved outside of this class
    if (result.status == CommandLineParserResult::ParseError)
    {
      std::cout << result.error.toStdString() << std::endl;
      return 1;
    }

    if (result.status == CommandLineParserResult::HelpRequested)
    {
      CommandLineParser parser;
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
  }
  else if(workingDir().exists())
  {
    work();
  }

  return 0;
}

QDir Dex::workingDir() const
{
  return m_workdir;
}

const Config& Dex::config() const
{
  return m_config;
}

void Dex::readConfig()
{
  m_config = dex::parse_config();
}

void Dex::parseInputs()
{
  m_model = dex::parse_inputs(m_config.inputs, m_config.suffixes);
}

void Dex::writeOutput()
{
  write_output(m_model, m_config.output, m_config.variables);
}

void Dex::work()
{
  if (m_cli.has_value() && m_cli.value().workdir.has_value())
  {
    QString workdir = m_cli.value().workdir.value();
    log::info() << "Changing working dir to '" << workdir.toStdString() << "'";
    
    if (!QDir::setCurrent(workdir))
    {
      log::error() << "Failed to change working dir";
      return;
    }
  }

  readConfig();

  if (!m_config.valid)
  {
    log::info() << "Could not parse dex.yml config";
  }

  parseInputs();
  writeOutput();
}

void Dex::write_output(const std::shared_ptr<Model>& model, const QString& outdir, json::Object values)
{
  assert(!outdir.isEmpty());

  log::info() << "Writing output to '" << outdir.toStdString() << "'";

  dex::run_exporter(model, outdir, values);
}

} // namespace dex
