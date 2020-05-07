// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/command-line-parser.h"
#include "dex/app/message-handler.h"

#include "dex/input/parser-machine.h"
#include "dex/output/json-export.h"
#include "dex/output/latex-export.h"
#include "dex/output/markdown-export.h"

#include <cxx/parsers/libclang-parser.h>

#include <json-toolkit/stringify.h>

#include <QDir>

#include <iostream>

namespace dex
{

Dex::Dex(int argc, char* argv[])
  : QCoreApplication(argc, argv)
{
  setApplicationName("dex");
  setApplicationVersion("0.0.0");

  m_suffixes << "cxx" << "cpp" << "h" << "hpp";

  dex::log::install_message_handler(&dex::app_message_handler);
}

int Dex::exec()
{
  CommandLineParser parser;
  
  auto result = parser.parse(Dex::arguments());


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
    std::cout << Dex::applicationVersion().toStdString() << std::endl;
  }
  else if (result.status == CommandLineParserResult::ClangVersionRequested)
  {
    std::cout << Dex::libClangVersion() << std::endl;
  }
  else if (result.status == CommandLineParserResult::Work)
  {
    process(result.inputs, result.output);
  }

  return 0;
}

std::string Dex::libClangVersion() const
{
  try
  {
    cxx::parsers::LibClangParser parser;
    CXVersion ver = parser.version();

    return std::to_string(ver.Major) + "." + std::to_string(ver.Minor) + "." + std::to_string(ver.Subminor);
  }
  catch (const cxx::parsers::LibClangParserError&)
  {
    return "libclang could not be found";
  }
}

void Dex::process(const QStringList& inputs, QString output)
{
  dex::ParserMachine parser;

  if (!inputs.empty())
  {
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
    output = "dex-output.json";

  write_output(parser.output(), output);
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

void Dex::write_output(const std::shared_ptr<Model>& model, const QString& name)
{
  QFileInfo info{ name };

  if (info.suffix() == "json")
  {
    auto obj = dex::JsonExport::serialize(*model);
    
    QFile file{ name };
    
    if (!file.open(QIODevice::WriteOnly))
      throw IOException{ name.toStdString(), "could not open file for writing" };

    file.write(QByteArray::fromStdString(json::stringify(obj)));
  }
  else if (info.suffix() == "md")
  {
    dex::MarkdownExport md_export;
    md_export.dump(model, info.dir());
  }
  else if (info.suffix() == "tex")
  {
    dex::LatexExport latex_export;
    latex_export.dump(model, info.dir());
  }
  else
  {
    throw std::runtime_error{ "Unknown export type" };
  }
}

} // namespace dex
