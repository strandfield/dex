// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/parsing.h"

#include "dex/app/message-handler.h"

#include "dex/input/parser-machine.h"

#include <json-toolkit/json.h>

#include <iostream>

namespace dex
{

struct ParsingContext
{
  dex::ParserMachine machine;
  std::set<std::string> suffixes;
};

void feed_machine(ParsingContext& context, const std::filesystem::path& path)
{
  if (!std::filesystem::exists(path))
    throw IOException{ path.string(), "input file does not exist" };

  if (std::filesystem::is_directory(path))
  {
    for (const std::filesystem::directory_entry& e : std::filesystem::directory_iterator(path))
    {
      if (e.is_directory())
      {
        feed_machine(context, e.path());
      }
      else if(e.is_regular_file())
      {
        auto it = context.suffixes.find(e.path().extension().string().substr(1));

        if (it != context.suffixes.end())
          feed_machine(context, e.path());
      }
    }
  }
  else
  {
    try
    {
      log::info() << "Parsing " << path.string();
      context.machine.process(path);
    }
    catch (const ParserException& ex)
    {
      LOG_ERROR << ex;

      const bool success = context.machine.recover();

      if (!success)
        context.machine.reset();
    }
    catch (const std::runtime_error& ex)
    {
      LOG_ERROR << ex.what();

      const bool success = context.machine.recover();

      if (!success)
        context.machine.reset();
    }
  }
}

std::shared_ptr<Model> parse_inputs(const std::set<std::string>& inputs, const std::set<std::string>& suffixes)
{
  ParsingContext context;
  context.suffixes = suffixes;

  if (!inputs.empty())
  {
    log::info() << "Inputs:";
    for (const auto& i : inputs)
    {
      log::info() << i;
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
    feed_machine(context, std::filesystem::current_path());
  }

  return context.machine.output();
}

} // namespace dex
