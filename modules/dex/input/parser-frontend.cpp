// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-frontend.h"

#include "dex/input/document-writer.h"
#include "dex/input/document-writer-frontend.h"
#include "dex/input/functional.h"
#include "dex/input/manual-parser.h"
#include "dex/input/parser-machine.h"
#include "dex/input/parser-errors.h"
#include "dex/input/program-parser.h"

#include "dex/common/logging.h"
#include "dex/common/string-utils.h"

#include <cxx/parsers/restricted-parser.h>

namespace dex
{

ParserFrontend::ParserFrontend(ParserMachine& machine)
  : m_machine(machine),
    m_mode(Mode::Program)
{
  m_prog_parser.reset(new ProgramParser(machine.output()->getOrCreateProgram()));
}

ParserFrontend::~ParserFrontend()
{

}


const std::map<std::string, ParserFrontend::CS>& ParserFrontend::csmap()
{
  static std::map<std::string, CS> static_instance = { 
    /* TeX */
    {Functions::PAR, CS::PAR},
    {Functions::INPUT, CS::INPUT},
    /* Program */
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
    {Functions::TYPEDEF, CS::TYPEDEF},
    {Functions::ENDTYPEDEF, CS::ENDTYPEDEF},
    {Functions::MACRO, CS::MACRO},
    {Functions::ENDMACRO, CS::ENDMACRO},
    {Functions::BRIEF, CS::BRIEF},
    {Functions::SINCE, CS::SINCE},
    {Functions::PARAM, CS::PARAM},
    {Functions::RETURNS, CS::RETURNS},
    {Functions::NONMEMBER, CS::NONMEMBER},
    {Functions::RELATES, CS::RELATES},
    /* Manual */
    {Functions::MANUAL, CS::manual},
    {Functions::PART, CS::part},
    {Functions::CHAPTER, CS::chapter},
    {Functions::SECTION, CS::section},
    /* Grouping */
    {Functions::INGROUP, CS::ingroup},
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

void ParserFrontend::write(char c)
{
  currentWriter().write(c);
}

void ParserFrontend::write_space(char c)
{
  if (m_mode == Mode::Program && m_prog_parser->state().current().type == ProgramParser::FrameType::Idle)
    return;

  auto& w = currentWriter();

  if (w.isIdle())
    return;

  w.write(c);
}

void ParserFrontend::write_active(char /* c */)
{
  // nothing for now
}

void ParserFrontend::bgroup()
{
  currentWriter().bgroup();
}

void ParserFrontend::egroup()
{
  currentWriter().egroup();
}

void ParserFrontend::mathshift() 
{
  currentWriter().mathshift();
}

void ParserFrontend::alignmenttab()
{
  currentWriter().alignmenttab();
}

void ParserFrontend::superscript()
{
  currentWriter().superscript();
}

void ParserFrontend::subscript()
{
  currentWriter().subscript();
}

void ParserFrontend::handle(const FunctionCall& call)
{
  auto it = csmap().find(call.function);

  if (it != csmap().end())
  {
    CS cs = it->second;

    switch (cs)
    {
    case CS::PAR:
      return par(call);
    case CS::INPUT:
      return input(call);
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
    case CS::CLASS:
      return fn_class(call);
    case CS::FN:
      return fn_fn(call);
    case CS::NAMESPACE:
      return fn_namespace(call);
    case CS::ENUM:
      return fn_enum(call);
    case CS::ENUMVALUE:
      return fn_enumvalue(call);
    case CS::VARIABLE:
      return fn_variable(call);
    case CS::TYPEDEF:
      return typedef_(call);
    case CS::ENDTYPEDEF:
      return endtypedef();
    case CS::MACRO:
      return macro(call);
    case CS::ENDMACRO:
      return endmacro();
    case CS::BRIEF:
      return fn_brief(call);
    case CS::SINCE:
      return fn_since(call);
    case CS::PARAM:
      return fn_param(call);
    case CS::RETURNS:
      return fn_returns(call);
    case CS::NONMEMBER:
      return cs_nonmember();
    case CS::RELATES:
      return fn_relates(call);
      /* Documents */
    case CS::manual:
      return fn_manual(call);
    case CS::part:
      return fn_part(call);
    case CS::chapter:
      return fn_chapter(call);
    case CS::section:
      return fn_section(call);
    case CS::ingroup:
      return ingroup(call);
    default:
      throw UnexpectedControlSequence{ call.function };
    }
  }
  else
  {
    if (m_mode == Mode::Program)
    {
      DocumentWriterFrontend writer{ *m_prog_parser->contentWriter() };
      writer.handle(call);
    }
    else
    {
      DocumentWriterFrontend writer{ *m_manual_parser->contentWriter() };
      writer.handle(call);
    }
  }
}

DocumentWriter& ParserFrontend::currentWriter()
{
  if (m_mode == Mode::Program)
  {
    return *m_prog_parser->contentWriter();
  }
  else
  {
    return *m_manual_parser->contentWriter();
  }
}

void ParserFrontend::checkMode(Mode m)
{
  if (m_mode != m)
    throw std::runtime_error{ "Bad mode" };
}

void ParserFrontend::par(const FunctionCall& call)
{
  if (m_mode == Mode::Program &&  m_prog_parser->state().current().type == ProgramParser::FrameType::Idle)
    return;

  currentWriter().par();
}

void ParserFrontend::input(const FunctionCall& call)
{
  const std::string& path = call.arg<std::string>(0);
  m_machine.input(path);
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

void ParserFrontend::typedef_(const FunctionCall& call)
{
  std::string decl = call.arg<std::string>(0);
  m_prog_parser->typedef_(std::move(decl));
}

void ParserFrontend::endtypedef()
{
  m_prog_parser->endtypedef();
}

void ParserFrontend::macro(const FunctionCall& call)
{
  std::string decl = call.arg<std::string>(0);
  m_prog_parser->macro(std::move(decl));
}

void ParserFrontend::endmacro()
{
  m_prog_parser->endmacro();
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

void ParserFrontend::cs_nonmember()
{
  m_prog_parser->nonmember();
}

void ParserFrontend::fn_relates(const FunctionCall& call)
{
  const std::string& class_name = call.arg<std::string>(0);
  m_prog_parser->relates(class_name);
}

void ParserFrontend::fn_manual(const FunctionCall& call)
{
  std::string name = call.arg<std::string>(0);
  auto man = std::make_shared<Manual>(std::move(name));

  m_machine.output()->documents.push_back(man);

  m_mode = Mode::Manual;
  m_manual_parser.reset(new ManualParser(man));
}

void ParserFrontend::fn_part(const FunctionCall& call)
{
  checkMode(Mode::Manual);

  std::string name = call.arg<std::string>(0);

  m_manual_parser->part(std::move(name));
}

void ParserFrontend::fn_chapter(const FunctionCall& call)
{
  checkMode(Mode::Manual);

  std::string name = call.arg<std::string>(0);

  m_manual_parser->chapter(std::move(name));
}

void ParserFrontend::fn_section(const FunctionCall& call)
{
  checkMode(Mode::Manual);

  std::string name = call.arg<std::string>(0);

  m_manual_parser->section(std::move(name));
}

void ParserFrontend::ingroup(const FunctionCall& call)
{
  std::string groupnames = call.arg<std::string>(0);
  std::vector<std::string> groups = str_split(groupnames, ',');

  if (m_mode == Mode::Program)
  {
    std::shared_ptr<cxx::Entity> cxxe = m_prog_parser->currentEntity();
    m_machine.output()->groups.multiInsert(groups, cxxe);
  }
  else if (m_mode == Mode::Manual)
  {
    std::shared_ptr<Document> doc = m_manual_parser->document();
    m_machine.output()->groups.multiInsert(groups, doc);
  }
}

void ParserFrontend::beginFile()
{
  /* no-op */
}

void ParserFrontend::endFile()
{
  if(m_mode == Mode::Program)
    m_prog_parser->endFile();
  else
    m_manual_parser->endFile();
}

void ParserFrontend::beginBlock()
{
  /* no-op */
}

void ParserFrontend::endBlock()
{
  if (m_mode == Mode::Program)
    m_prog_parser->endBlock();
  else
    m_manual_parser->endBlock();
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
