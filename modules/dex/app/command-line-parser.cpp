// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/command-line-parser.h"

#include <json-toolkit/json.h>

namespace dex
{

CommandLineParser::CommandLineParser()
{

}

CommandLineParserResult CommandLineParser::parse(int argc, char* argv[])
{
  CommandLineParserResult result;

  for (int i(1); i < argc; )
  {
    std::string opt = std::string(argv[i]);

    if (opt == "-?" || opt == "--help" || opt == "-h")
    {
      result.status = CommandLineParserResult::HelpRequested;
      ++i;
    }
    else if (opt == "-w")
    {
      result.status = CommandLineParserResult::Work;
      ++i;
      result.workdir = std::string(argv[i]);
      ++i;
    }
    else if (opt == "-v" || opt == "--version")
    {
      result.status = CommandLineParserResult::VersionRequested;
      ++i;
    }
    else
    {
      result.status = CommandLineParserResult::ParseError;
      result.error = "Unknown option: " + opt;
      return result;
    }
  }

  if (argc == 1)
  {
    result.status = CommandLineParserResult::Work;
  }

  return result;
}

std::string CommandLineParser::help() const
{
  std::string help;

  help += "Usage: dex [options]\n";
  help += "\n";
  help += "Options:\n";
  help += "  -?, -h, --help  Displays help on commandline options.\n";
  help += "  -v, --version   Displays version information.\n";
  help += "  -w <workdir>    Working directory\n";

  return help;
}

} // namespace dex
