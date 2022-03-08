// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/parsing.h"

#include "dex/app/message-handler.h"

#include "dex/input/parser-machine.h"

#include <json-toolkit/json.h>

#include <QDir>

#include <iostream>

namespace dex
{

struct ParsingContext
{
  dex::ParserMachine machine;
  QStringList suffixes;
};

void feed_machine(ParsingContext& context, const QString& input);
void feed_machine(ParsingContext& context, const QDir& input);

void feed_machine(ParsingContext& context, const QString& input)
{
  QFileInfo info{ input };

  if (!info.exists())
    throw IOException{ input.toStdString(), "input file does not exist" };

  if (info.isDir())
  {
    QDir dir{ info.absoluteFilePath() };
    feed_machine(context, dir);
  }
  else
  {
    try
    {
      log::info() << "Parsing " << info.filePath().toStdString();
      context.machine.process(info);
    }
    catch (const ParserException& ex)
    {
      LOG_ERROR << ex;

      const bool success = context.machine.recover();

      if (!success)
        context.machine.reset();
    }
  }
}

void feed_machine(ParsingContext& context, const QDir& input)
{
  QFileInfoList entries = input.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

  for (const auto& e : entries)
  {
    if (e.isDir())
    {
      feed_machine(context, QDir{ e.absoluteFilePath() });
    }
    else
    {
      if (context.suffixes.contains(e.suffix()))
        feed_machine(context, e.absoluteFilePath());
    }
  }
}

std::shared_ptr<Model> parse_inputs(const QStringList& inputs, const QStringList& suffixes)
{
  ParsingContext context;
  context.suffixes = suffixes;

  if (!inputs.empty())
  {
    log::info() << "Inputs:";
    for (const auto& i : inputs)
    {
      log::info() << i.toStdString();
    }

    for (const auto& i : inputs)
    {
      try
      {
        feed_machine(context, i);
      }
      catch (const IOException& ex)
      {
        LOG_ERROR << ex;
      }
    }
  }
  else
  {
    feed_machine(context, QDir::current());
  }

  return context.machine.output();
}

} // namespace dex
