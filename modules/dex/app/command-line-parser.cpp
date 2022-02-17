// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/command-line-parser.h"

#include "dex/common/json-utils.h"

namespace dex
{

static json::Object values_to_json(QStringList all_values)
{
  dex::SettingsMap result;

  QStringList values = all_values.join(";").split(";", QString::SkipEmptyParts);

  for (QString key_value_pair : values)
  {
    QStringList key_value = key_value_pair.split('=', QString::SkipEmptyParts);

    if (key_value.size() == 2)
    {
      result[key_value.front().toStdString()] = key_value.back().toStdString();
    }
  }

  return build_json(result);
}

CommandLineParser::CommandLineParser()
{
  addHelpOption();
  addVersionOption();

  addOption({ "i", "Input (file / directory)", "input" });
  addOption({ "o", "Output", "output" });
  addOption({ "w", "Working directory", "workdir" });
  addOption({ "value", "Project values for the Liquid-exporter", "variables" });
  addOption({ "reset-profiles", "Resets the exporter profiles" });
}

CommandLineParserResult CommandLineParser::parse(const QStringList& args)
{
  const bool success = QCommandLineParser::parse(args);

  CommandLineParserResult result;

  if (!success)
  {
    result.status = CommandLineParserResult::ParseError;
    result.error = QCommandLineParser::errorText();
  }
  else if (isSet("help"))
  {
    result.status = CommandLineParserResult::HelpRequested;
  }
  else if (isSet("version"))
  {
    result.status = CommandLineParserResult::VersionRequested;
  }
  else
  {
    result.status = CommandLineParserResult::Work;
    result.inputs = values("i");
    result.output = value("o");
    result.workdir= value("w");
    result.values = values_to_json(values("value"));
  }

  result.reset_profiles = isSet("reset-profiles");

  return result;
}

QString CommandLineParser::help() const
{
  return QCommandLineParser::helpText();
}

} // namespace dex
