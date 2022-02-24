// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/command-line-parser.h"

#include "dex/common/json-utils.h"

namespace dex
{

CommandLineParser::CommandLineParser()
{
  addHelpOption();
  addVersionOption();

  addOption({ "w", "Working directory", "workdir" });
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
    result.workdir= value("w");
  }

  return result;
}

QString CommandLineParser::help() const
{
  return QCommandLineParser::helpText();
}

} // namespace dex
