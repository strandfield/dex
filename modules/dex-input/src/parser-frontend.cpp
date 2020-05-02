// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-frontend.h"

#include "dex/input/document-writer.h"
#include "dex/input/document-writer-frontend.h"
#include "dex/input/functional.h"
#include "dex/input/parser-machine.h"
#include "dex/input/parser-errors.h"
#include "dex/input/program-parser.h"

#include "dex/common/logging.h"

#include <cxx/parsers/restricted-parser.h>

namespace dex
{

ParserFrontend::ParserFrontend(ParserMachine& machine)
  : m_mode(Mode::Program)
{
  m_prog_parser.reset(new ProgramParser(machine.output()->getOrCreateProgram()->globalNamespace()));
}

ParserFrontend::~ParserFrontend()
{

}


const std::map<std::string, ParserFrontend::CS>& ParserFrontend::csmap()
{
  static std::map<std::string, CS> static_instance = { 
    {Functions::CLASS, CS::CLASS},
    {Functions::ENDCLASS, CS::ENDCLASS},
    {Functions::FUNCTION , CS::FN},
    {Functions::ENDFN, CS::ENDFN},
    {Functions::NAMESPACE, CS::NAMESPACE},
    {Functions::ENDNAMESPACE , CS::ENDNAMESPACE},
    {Functions::ENUM, CS::ENUM},
    {Functions::ENDENUM, CS::ENDENUM},
    {Functions::ENUMVALUE, CS::ENUMVALUE},
    {Functions::ENDENUMVALUE, CS::ENDENUMVALUE},
    {Functions::VARIABLE, CS::VARIABLE},
    {Functions::ENDVARIABLE, CS::ENDVARIABLE},
    {Functions::BRIEF, CS::BRIEF},
    {Functions::SINCE, CS::SINCE},
    {Functions::PARAM, CS::PARAM},
    {Functions::RETURNS, CS::RETURNS},
  };

  return static_instance;
}

ParserFrontend::CS ParserFrontend::parseCs(const std::string& str) const
{
  auto it = csmap().find(str);

  if (it == csmap().end())
    throw UnknownControlSequence{ str };

  return it->second;
}

inline static bool is_discardable(const tex::parsing::Token& tok)
{
  return (tok.isCharacterToken() && tok.characterToken().category == tex::parsing::CharCategory::Space)
    || (tok.isControlSequence() && tok.controlSequence() == Functions::PAR);
}

void ParserFrontend::write(tex::parsing::Token&& tok)
{
  if (m_prog_parser->state().current().type == ProgramParser::FrameType::Idle && is_discardable(tok))
    return;

  if (tok.isCharacterToken())
  {
    if (m_prog_parser->state().current().type == ProgramParser::FrameType::Idle)
    {
      LOG_WARNING << "Non-space character ignored";
      return;
    }

    DocumentWriterFrontend writer{ *m_prog_parser->contentWriter() };
    writer.write(tok.characterToken().value);
  }
  else
  {
    auto it = csmap().find(tok.controlSequence());

    if (it != csmap().end())
    {
      CS cs = it->second;

      switch (cs)
      {
      case CS::ENDFN:
        return cs_endfn();
      case CS::ENDCLASS:
        return cs_endclass();
      case CS::ENDNAMESPACE:
        return cs_endnamespace();
      case CS::ENDENUM:
        return cs_endenum();
      case CS::ENDENUMVALUE:
        return cs_endenumvalue();
      case CS::ENDVARIABLE:
        return cs_endvariable();
      default:
        throw UnexpectedControlSequence{ tok.controlSequence() };
      }
    }
    else
    {
      //if (!currentFrame().writer)
      //  throw UnexpectedControlSequence{ tok.controlSequence() };

      FunctionCall call;
      call.function = tok.controlSequence();

      DocumentWriterFrontend writer{ *m_prog_parser->contentWriter() };
      writer.handle(call);
    }
  }
}

void ParserFrontend::handle(const FunctionCall& call)
{
  if (call.function == Functions::CLASS)
  {
    fn_class(call);
  }
  else if (call.function == Functions::FUNCTION)
  {
    fn_fn(call);
  }
  else if (call.function == Functions::NAMESPACE)
  {
    fn_namespace(call);
  }
  else if (call.function == Functions::ENUM)
  {
    fn_enum(call);
  }
  else if (call.function == Functions::ENUMVALUE)
  {
    fn_enumvalue(call);
  }
  else if (call.function == Functions::VARIABLE)
  {
    fn_variable(call);
  }
  else if (call.function == Functions::BRIEF)
  {
    fn_brief(call);
  }
  else if (call.function == Functions::SINCE)
  {
    fn_since(call);
  }
  else if (call.function == Functions::PARAM)
  {
    fn_param(call);
  }
  else if (call.function == Functions::RETURNS)
  {
    fn_returns(call);
  }
  else
  {
    DocumentWriterFrontend writer{ *m_prog_parser->contentWriter() };
    writer.handle(call);
  }
}

void ParserFrontend::fn_class(const FunctionCall& call)
{
  std::string name = call.arg<std::string>(0);
  m_prog_parser->class_(std::move(name));
}

void ParserFrontend::cs_endclass()
{
  m_prog_parser->endclass();
}

void ParserFrontend::fn_fn(const FunctionCall& call)
{
  std::string signature = call.arg<std::string>(0);
  m_prog_parser->fn(std::move(signature));
}

void ParserFrontend::cs_endfn()
{
  m_prog_parser->endfn();
}

void ParserFrontend::fn_namespace(const FunctionCall& call)
{
  std::string name = call.arg<std::string>(0);
  m_prog_parser->namespace_(std::move(name));
}

void ParserFrontend::cs_endnamespace()
{
  m_prog_parser->endnamespace();
}

void ParserFrontend::fn_enum(const FunctionCall& call)
{
  std::string name = call.arg<std::string>(0);
  m_prog_parser->enum_(std::move(name));
}

void ParserFrontend::cs_endenum()
{
  m_prog_parser->endenum();
}

void ParserFrontend::fn_enumvalue(const FunctionCall& call)
{
  std::string name = call.arg<std::string>(0);
  m_prog_parser->value(std::move(name));
}

void ParserFrontend::cs_endenumvalue()
{
  m_prog_parser->endenumvalue();
}

void ParserFrontend::fn_variable(const FunctionCall& call)
{
  std::string decl = call.arg<std::string>(0);
  m_prog_parser->variable(std::move(decl));
}

void ParserFrontend::cs_endvariable()
{
  m_prog_parser->endvariable();
}

void ParserFrontend::fn_brief(const FunctionCall& call)
{
  std::string text = call.arg<std::string>(0);
  m_prog_parser->brief(text);
}

void ParserFrontend::fn_since(const FunctionCall& call)
{
  if (call.options.empty())
  {
    // e.g. \since 5.12
    std::string version = call.arg<std::string>(0);
    m_prog_parser->since(std::move(version));
  }
  else
  {
    std::string text = call.arg<std::string>(0);
    std::string version = call.opt<std::string>("").value();

    m_prog_parser->contentWriter()->since(version, text);
  }
}

void ParserFrontend::fn_param(const FunctionCall& call)
{
  std::string des = call.arg<std::string>(0);
  m_prog_parser->param(des);
}

void ParserFrontend::fn_returns(const FunctionCall& call)
{
  std::string des = call.arg<std::string>(0);
  m_prog_parser->returns(des);
}

void ParserFrontend::beginFile()
{
  /* no-op */
}

void ParserFrontend::endFile()
{
  m_prog_parser->endFile();
}

void ParserFrontend::beginBlock()
{
  /* no-op */
}

void ParserFrontend::endBlock()
{
  m_prog_parser->endBlock();
}

void ParserFrontend::recover()
{
  // @TODO: perform things to recover from an exception
}

void ParserFrontend::reset()
{
  // @TODO: perform things to reset the parsers
}

} // namespace dex
