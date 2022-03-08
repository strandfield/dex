// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/message-handler.h"
#include "dex/app/parsing.h"

#include "dex/output/exporter.h"

#include <json-toolkit/json.h>

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


Dex::Dex(const CommandLineParserResult& arguments)
  : m_workdir(QDir::current())
{
  if (arguments.workdir.has_value())
    m_workdir = QDir(arguments.workdir.value());
}

Dex::Dex(const QDir& workdir)
  : m_workdir(workdir)
{

}

int Dex::exec()
{
  if(workingDir().exists())
  {
    work();
    return 0;
  }
  else
  {
    log::error() << "Working directory " << workingDir().absolutePath().toStdString() << " does not exist";
    return 1;
  }
}

QDir Dex::workingDir() const
{
  return m_workdir;
}

const Config& Dex::config() const
{
  return m_config;
}

std::shared_ptr<Model> Dex::model() const
{
  return m_model;
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
  if (QDir::current() != workingDir())
  {
    log::info() << "Changing working dir to '" << workingDir().absolutePath().toStdString() << "'";

    if (!QDir::setCurrent(workingDir().absolutePath()))
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
