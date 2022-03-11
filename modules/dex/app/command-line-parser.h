// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_COMMANDLINEPARSER_H
#define DEX_APP_COMMANDLINEPARSER_H

#include "dex/dex-app.h"

#include <json-toolkit/json.h>

#include <optional>
#include <string>

namespace dex
{

struct CommandLineParserResult
{
  enum Status
  {
    HelpRequested,
    VersionRequested,
    Work,
    ParseError,
  };

  Status status = HelpRequested;
  std::string error;
  std::optional<std::string> workdir;
};

class DEX_APP_API CommandLineParser
{
public:
  CommandLineParser();

  CommandLineParserResult parse(int argc, char* argv[]);

  std::string help() const;

};

} // namespace dex

#endif // DEX_APP_COMMANDLINEPARSER_H
