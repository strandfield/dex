// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/command-line-parser.h"

namespace dex
{

CommandLineParser::CommandLineParser()
{
  addHelpOption();
  addVersionOption();

  addOption({ "clang-version", "Prints the version of clang if available."});

  addOption({ "i", "Input (file / directory)", "input" });
  addOption({ "o", "Output", "output" });
  addOption({ "value", "Project values for the Liquid-exporter", "variables" });
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
  else if (isSet("clang-version"))
  {
    result.status = CommandLineParserResult::ClangVersionRequested;
  }
  else
  {
    result.status = CommandLineParserResult::Work;
    result.inputs = values("i");
    result.output = value("o");
    result.values = values("value");
  }

  return result;
}

QString CommandLineParser::help() const
{
  return QCommandLineParser::helpText();
}

} // namespace dex
