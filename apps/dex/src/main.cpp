// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"
#include "dex/app/command-line-parser.h"
#include "dex/app/message-handler.h"

#include <iostream>

int main(int argc, char *argv[])
{
  dex::CommandLineParser parser;
  dex::CommandLineParserResult cli = parser.parse(argc, argv);

  if (cli.status == dex::CommandLineParserResult::ParseError)
  {
    std::cout << cli.error << std::endl;
    return 1;
  }

  if (cli.status == dex::CommandLineParserResult::HelpRequested)
  {
    std::cout << parser.help() << std::endl;
    return 0;
  }
  else if (cli.status == dex::CommandLineParserResult::VersionRequested)
  {
    std::cout << dex::versionstr() << std::endl;
    return 0;
  }
  else if (cli.status == dex::CommandLineParserResult::Work)
  {
    dex::log::install_message_handler(&dex::app_message_handler);
    dex::Dex dexapp{ cli };
    return dexapp.exec();
  }
}
