// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_FUNCTION_CALLER_H
#define DEX_INPUT_FUNCTION_CALLER_H

#include "dex/dex-input.h"

#include <tex/token.h>

#include <map>
#include <variant>
#include <vector>

namespace dex
{

class ParserMachine;

class DEX_INPUT_API FunctionCaller
{
public:
  explicit FunctionCaller(ParserMachine& machine);

  ParserMachine& machine() const;

  enum TaskType
  {
    TT_Invalid = 0,
    ParseBool,
    ParseInt,
    ParseWord,
    ParseLongWord,
    ParseOptions,
    Call,
  };

  enum TaskProgress
  {
    TP_NotStarted = 0,
    /* Parsing bool, int, word */
    TP_GatheringChars,
    /* Parsing options */
    TP_WaitLeftBracket,
    TP_WaitKeyOrRightBracket,
    TP_ParsingKey,
    TP_ParsingValue,
  };

  struct Task
  {
    TaskType type = TT_Invalid;
    TaskProgress progress = TP_NotStarted;
    size_t brace_depth = 0;
    std::string key_buffer;
    std::string buffer;
  };

  enum class State
  {
    Idle,
    GatheringTasks,
    WaitingForCallCs,
    Working,
  };

  State& state();
  
  void write(tex::parsing::Token&& tok);

  typedef std::variant<bool, int, double, std::string> Argument;
  typedef std::map<std::string, Argument> Options;

  std::vector<Argument>& arguments();
  Options& options();

  std::vector<tex::parsing::Token>& output();

protected:
  void addTask(TaskType tt);
  Task& currentTask();

  void startWorking();
  void startTask(Task& t);
  void finishTask(Task& t);
  void finishCurrentTask();
  void finishParseOptions(Task& t);

  void work(tex::parsing::Token&& tok);

  void parse_bool(tex::parsing::Token&& tok);
  void parse_int(tex::parsing::Token&& tok);
  void parse_word(tex::parsing::Token&& tok);
  void parse_longword(tex::parsing::Token&& tok);
  void parse_options(tex::parsing::Token&& tok);

  static Argument parse(std::string&& str);

private:
  ParserMachine& m_machine;
  State m_state;
  std::vector<Task> m_tasks;
  bool m_clear_results;
  std::vector<Argument> m_arguments;
  Options m_options;
  std::vector<tex::parsing::Token> m_output;
};

} // namespace dex

namespace dex
{

inline FunctionCaller::State& FunctionCaller::state()
{
  return m_state;
}

inline std::vector<FunctionCaller::Argument>& FunctionCaller::arguments()
{
  return m_arguments;
}

inline FunctionCaller::Options& FunctionCaller::options()
{
  return m_options;
}

inline std::vector<tex::parsing::Token>& FunctionCaller::output()
{
  return m_output;
}

} // namespace dex

#endif // DEX_INPUT_FUNCTION_CALLER_H