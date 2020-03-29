// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/function-caller.h"

#include "dex/input/parser-machine.h"
#include "dex/input/parser-mode.h"
#include "dex/input/parser-errors.h"

#include "dex/common/logging.h"

#include <cassert>
#include <set>
#include <stdexcept>

namespace dex
{

FunctionCaller::FunctionCaller(ParserMachine& machine)
  : m_machine{ machine },
  m_call{ machine.call() },
  m_state{ State::Idle },
  m_clear_results{ false },
  m_pending_call{ false }
{

}

ParserMachine& FunctionCaller::machine() const
{
  return m_machine;
}

FunctionCall& FunctionCaller::call() const
{
  return m_call;
}

void FunctionCaller::write(tex::parsing::Token&& tok)
{
  static const std::set<std::string> recognized_csnames = {
     "c@ll",
     "p@rseoptions",
     "p@rsebool",
     "p@rseint",
     "p@rseword",
     "p@rseline"
  };

  if (state() == State::Idle)
  {
    if (!tok.isControlSequence())
    {
      m_output.push_back(std::move(tok));
    }
    else
    {
      if (recognized_csnames.find(tok.controlSequence()) != recognized_csnames.end())
      {
        m_state = State::GatheringTasks;
        write(std::move(tok));
      }
      else
      {
        m_output.push_back(std::move(tok));
      }
    }
  }
  else if (state() == State::GatheringTasks)
  {
    if (tok.isControlSequence())
    {
      if (tok.controlSequence() == "c@ll")
      {
        m_state = State::WaitingForCallCs;
      }
      else if (tok.controlSequence() == "p@rseoptions")
      {
        addTask(ParseOptions);
      }
      else if (tok.controlSequence() == "p@rsebool")
      {
        addTask(ParseBool);
      }
      else if (tok.controlSequence() == "p@rseint")
      {
        addTask(ParseInt);
      }
      else if (tok.controlSequence() == "p@rseword")
      {
        addTask(ParseWord);
      }
      else if (tok.controlSequence() == "p@rseline")
      {
        addTask(ParseLongWord);
      }
      else
      {
        throw UnexpectedControlSequence{ tok.controlSequence() };
      }
    }
    else
    {
      startWorking();
      write(std::move(tok));
    }
  }
  else if(state() == State::WaitingForCallCs)
  {
    if (!tok.isControlSequence())
      throw ExpectedControlSequence{ "c@ll" };
   
    Task call_task;
    call_task.type = Call;
    call_task.buffer = tok.controlSequence();
    m_tasks.push_back(std::move(call_task));
    startWorking();
  }
  else
  {
    assert(state() == State::Working);
    work(std::move(tok));
  }
}

void FunctionCaller::addTask(TaskType tt)
{
  Task task;
  task.type = tt;
  m_tasks.push_back(std::move(task));
}

FunctionCaller::Task& FunctionCaller::currentTask()
{
  return m_tasks.front();
}

void FunctionCaller::startWorking()
{
  assert(!m_tasks.empty());

  m_state = State::Working;
  startTask(m_tasks.front());

  if (m_clear_results)
  {
    m_call.function.clear();
    m_call.arguments.clear();
    m_call.options.clear();
    m_clear_results = false;
  }
}

void FunctionCaller::startTask(Task& t)
{
  switch (t.type)
  {
  case ParseBool:
  case ParseInt:
  case ParseWord:
  {
    t.progress = TP_GatheringChars;
  }
  break;
  case ParseLongWord:
  {
    t.progress = TP_GatheringChars;
    machine().lexer().catcodes()[static_cast<int>('\n')] = tex::parsing::CharCategory::Active;
  }
  break;
  case ParseOptions:
  {
    t.progress = TP_WaitLeftBracket;
  }
  break;
  case Call:
  {
    m_call.function = t.buffer;
    //m_output.push_back(tex::parsing::Token{ t.buffer });
    m_pending_call = true;
    finishCurrentTask();
  }
  break;
  default:
    break;
  }
}

void FunctionCaller::finishTask(Task& t)
{
  switch (t.type)
  {
  case ParseBool:
  {
    const int n = std::stoi(t.buffer);
    m_call.arguments.emplace_back(static_cast<bool>(n));
  }
  break;
  case ParseInt:
  {
    const int n = std::stoi(t.buffer);
    m_call.arguments.emplace_back(n);
  }
  break;
  case ParseWord:
  {
    m_call.arguments.emplace_back(std::move(t.buffer));
  }
  break;
  case ParseLongWord:
  {
    m_call.arguments.emplace_back(std::move(t.buffer));
    machine().lexer().catcodes()[static_cast<int>('\n')] = tex::parsing::CharCategory::EndOfLine;
  }
  break;
  case ParseOptions:
  {
    return finishParseOptions(t);
  }
  break;
  case Call:
  {
    m_clear_results = true;
  }
  break;
  default:
    break;
  }
}

void FunctionCaller::finishParseOptions(Task& t)
{
  assert(t.type == TaskType::ParseOptions);

  auto& preprocessor = machine().preprocessor();

  const tex::parsing::Macro* macro = preprocessor.find("@fteroptions");

  if (macro)
  {
    preprocessor.input().insert(preprocessor.input().end(), macro->replacementText().begin(), macro->replacementText().end());
  }
}

void FunctionCaller::finishCurrentTask()
{
  finishTask(currentTask());
  m_tasks.erase(m_tasks.begin());
  
  if (m_tasks.empty())
  {
    m_state = State::Idle;
  }
  else
  {
    startTask(currentTask());
  }
}

void FunctionCaller::work(tex::parsing::Token&& tok)
{
  if (tok.isControlSequence())
    throw UnexpectedControlSequence{ tok.controlSequence() };

  switch (m_tasks.front().type)
  {
  case ParseBool:
    return parse_bool(std::move(tok));
  case ParseInt:
    return parse_int(std::move(tok));
  case ParseWord:
    return parse_word(std::move(tok));
  case ParseLongWord:
    return parse_longword(std::move(tok));
  case ParseOptions:
    return parse_options(std::move(tok));
  }
}

void FunctionCaller::parse_bool(tex::parsing::Token&& tok)
{
  if (tok.characterToken().category == tex::parsing::CharCategory::Space)
  {
    return finishCurrentTask();
  }

  char c = tok.characterToken().value;
  
  // TODO: validation

  currentTask().buffer.push_back(c);
}

void FunctionCaller::parse_int(tex::parsing::Token&& tok)
{
  if (tok.characterToken().category == tex::parsing::CharCategory::Space)
  {
    return finishCurrentTask();
  }

  char c = tok.characterToken().value;

  // TODO: validation

  currentTask().buffer.push_back(c);
}

void FunctionCaller::parse_word(tex::parsing::Token&& tok)
{
  // TODO: add punctuator list
  if (tok.characterToken().category == tex::parsing::CharCategory::Space
    || tok.characterToken().value == '.' || tok.characterToken().value == ',' 
    || tok.characterToken().value == ':')
  {
    return finishCurrentTask();
  }

  char c = tok.characterToken().value;
  currentTask().buffer.push_back(c);
}

void FunctionCaller::parse_longword(tex::parsing::Token&& tok)
{
  if (tok.characterToken().category == tex::parsing::CharCategory::Active
    && tok.characterToken().value == '\n')
  {
    return finishCurrentTask();
  }

  char c = tok.characterToken().value;
  currentTask().buffer.push_back(c);
}

void FunctionCaller::parse_options(tex::parsing::Token&& tok)
{
  auto& t = currentTask();
  char c = tok.characterToken().value;

  auto append_key = [&t](char c) -> void
  {
    t.key_buffer.push_back(c);

    if (c == ' ' && t.key_buffer.size() == 1)
      t.key_buffer.pop_back();
  };

  switch (t.progress)
  {
  case TP_WaitLeftBracket:
  {
    if (c == '[')
    {
      t.progress = TP_WaitKeyOrRightBracket;
    }
    else
    {
      finishCurrentTask();
    }
  }
  break;
  case TP_WaitKeyOrRightBracket:
  {
    if (c == ']')
    {
      finishCurrentTask();
    }
    else
    {
      append_key(c);
      t.progress = TP_ParsingKey;
    }
  }
  break;
  case TP_ParsingKey:
  {
    if (c == ',')
    {
      m_call.options[""] = parse(std::move(t.key_buffer));
      t.progress = TP_WaitKeyOrRightBracket;
    }
    else if (c == ']')
    {
      m_call.options[""] = parse(std::move(t.key_buffer));
      finishCurrentTask();
    }
    else if (c == '=')
    {
      t.progress = TP_ParsingValue;
    }
    else
    {
      append_key(c);
    }
  }
  break;
  case TP_ParsingValue:
  {
    if (c == ',')
    {
      m_call.options[t.key_buffer] = parse(std::move(t.buffer));
      t.progress = TP_WaitKeyOrRightBracket;
    }
    else if (c == ']')
    {
      m_call.options[t.key_buffer] = parse(std::move(t.buffer));
      finishCurrentTask();
    }
    else
    {
      t.buffer.push_back(c);
    }
  }
  break;
  }
}

FunctionCaller::Argument FunctionCaller::parse(std::string&& str)
{
  if (str.empty())
  {
    LOG_WARNING << "Empty value of key in command option";
    return std::move(str);
  }

  const bool all_digits = std::all_of(str.begin(), str.end(), [](char c) {
    return c >= '0' && c <= '9';
    });
  
  if (all_digits)
    return std::stoi(str);
  else
    return std::move(str);
}

} // namespace dex
