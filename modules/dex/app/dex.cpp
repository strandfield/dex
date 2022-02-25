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

void Dex::process(const QStringList& inputs, QString output, json::Object values)
{
  std::shared_ptr<Model> model = dex::parse_inputs(inputs, m_suffixes);

  write_output(model, output, values);
}

void Dex::write_output(const std::shared_ptr<Model>& model, const QString& outdir, json::Object values)
{
  assert(!outdir.isEmpty());

  log::info() << "Writing output to '" << outdir.toStdString() << "'";

  dex::run_exporter(model, outdir, values);
}

} // namespace dex
