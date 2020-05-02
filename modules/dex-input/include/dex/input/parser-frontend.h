// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_FRONTEND_H
#define DEX_INPUT_PARSER_FRONTEND_H

#include "dex/dex-input.h"

#include <tex/token.h>

#include <map>
#include <memory>
#include <string>

namespace dex
{

struct FunctionCall;

class ParserMachine;
class ProgramParser;

class DEX_INPUT_API ParserFrontend
{
public:
  explicit ParserFrontend(ParserMachine& machine);
  ~ParserFrontend();

  enum class Mode
  {
    Idle,
    Program,
  };

  enum class CS
  {
    /* Entities */
    CLASS,
    ENDCLASS,
    FN,
    ENDFN,
    NAMESPACE,
    ENDNAMESPACE,
    ENUM,
    ENDENUM,
    ENUMVALUE,
    ENDENUMVALUE,
    VARIABLE,
    ENDVARIABLE,
    /* Metadata */
    BRIEF,
    SINCE,
    PARAM,
    RETURNS,
  };

  static const std::map<std::string, CS>& csmap();
  
  void write(tex::parsing::Token&& tok);
  void handle(const FunctionCall& call);

  void beginFile();
  void endFile();
  void beginBlock();
  void endBlock();

  void recover();
  void reset();

protected:

  CS parseCs(const std::string& str) const;

  void fn_class(const FunctionCall& call);
  void cs_endclass();
  void fn_fn(const FunctionCall& call);
  void cs_endfn();
  void fn_namespace(const FunctionCall& call);
  void cs_endnamespace();
  void fn_enum(const FunctionCall& call);
  void cs_endenum();
  void fn_enumvalue(const FunctionCall& call);
  void cs_endenumvalue();
  void fn_variable(const FunctionCall& call);
  void cs_endvariable();

  void fn_brief(const FunctionCall& call);
  void fn_since(const FunctionCall& call);
  void fn_param(const FunctionCall& call);
  void fn_returns(const FunctionCall& call);

private:
  Mode m_mode;
  std::unique_ptr<ProgramParser> m_prog_parser;
};

} // namespace dex

#endif // DEX_INPUT_PARSER_FRONTEND_H
