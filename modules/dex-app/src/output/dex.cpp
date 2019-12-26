// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include "dex/app/command-line-parser.h"

#include "dex/input/parser-machine.h"
#include "dex/output/json-export.h"
#include "dex/output/markdown-export.h"

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
  else if (result.status == CommandLineParserResult::Work)
  {
    process(result.inputs, result.output);
  }

  return 0;
}

void Dex::process(const QStringList& inputs, QString output)
{
  dex::ParserMachine parser;

  if (!inputs.empty())
  {
    for (const auto& i : inputs)
      feed(parser, i);
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
    throw std::runtime_error{ "Invalid input" };

  if (info.isDir())
  {
    QDir dir{ info.absoluteFilePath() };
    feed(parser, dir);
  }
  else
  {
    parser.process(info);
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

void Dex::write_output(const std::shared_ptr<cxx::Program>& prog, const QString& name)
{
  QFileInfo info{ name };

  if (info.suffix() == "json")
  {
    auto obj = dex::JsonExport::serialize(*prog);
    
    QFile file{ name };
    
    if (!file.open(QIODevice::WriteOnly))
      throw std::runtime_error{ "Could not open output file" };

    file.write(QByteArray::fromStdString(json::stringify(obj)));
  }
  else if (info.suffix() == "md")
  {
    dex::MarkdownExport md_export;
    md_export.dump(prog, info.dir());
  }
  else
  {
    throw std::runtime_error{ "Unknown export type" };
  }
}

} // namespace dex
