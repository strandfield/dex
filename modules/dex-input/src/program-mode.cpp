// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/program-mode.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-machine.h"
#include "dex/input/parser-errors.h"

#include "dex/model/class-documentation.h"
#include "dex/model/enum-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"

#include "dex/common/logging.h"

#include <cxx/documentation.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>

namespace dex
{

static EntityDocumentation& doc(const std::shared_ptr<cxx::Documentation>& d)
{
  return *static_cast<EntityDocumentation*>(d.get());
}

ProgramMode::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

ProgramMode::Frame::Frame(FrameType ft, std::shared_ptr<cxx::Entity> cxxent)
  : state::Frame<FrameType>(ft)
{
  node = cxxent;
  writer = std::make_shared<DocumentWriter>();
}

ProgramMode::ProgramMode(ParserMachine& machine)
  : ParserMode(machine)
{
  m_state.enter<FrameType::Idle>();
  m_state.current().node = machine.output()->getOrCreateProgram()->globalNamespace();
}

ProgramMode::State& ProgramMode::state()
{
  return m_state;
}

const std::map<std::string, ProgramMode::CS>& ProgramMode::csmap()
{
  static std::map<std::string, CS> static_instance = { 
    {Functions::PAR , CS::PAR},
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
    {Functions::BRIEF, CS::BRIEF},
    {Functions::SINCE, CS::SINCE},
    {Functions::PARAM, CS::PARAM},
    {Functions::RETURNS, CS::RETURNS},
  };

  return static_instance;
}

ProgramMode::CS ProgramMode::parseCs(const std::string& str) const
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

bool ProgramMode::write(tex::parsing::Token&& tok)
{
  do_write(std::move(tok));
  return done();
}

void ProgramMode::do_write(tex::parsing::Token&& tok)
{
  if (currentFrame().type == FrameType::Idle && is_discardable(tok))
    return;

  if (tok.isCharacterToken())
  {
    if (currentFrame().type == FrameType::Idle)
    {
      LOG_WARNING << "Non-space character ignored";
      return;
    }

    currentFrame().writer->write(tok.characterToken().value);
  }
  else
  {
    auto it = csmap().find(tok.controlSequence());

    if (it != csmap().end())
    {
      CS cs = it->second;

      switch (cs)
      {
      case CS::PAR:
        return cs_par();
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
      default:
        throw UnexpectedControlSequence{ tok.controlSequence() };
      }
    }
    else
    {
      if (!currentFrame().writer)
        throw UnexpectedControlSequence{ tok.controlSequence() };

      FunctionCall call;
      call.function = tok.controlSequence();

      if (!currentFrame().writer->handle(call))
        throw UnexpectedControlSequence{ tok.controlSequence() };
    }
  }
}

bool ProgramMode::handle(const FunctionCall& call)
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
    Frame& f = currentFrame();
    if (!f.writer || !f.writer->handle(call))
      throw BadControlSequence{ call.function };
  }

  return done();
}

void ProgramMode::cs_par()
{
  Frame& f = currentFrame();

  if (f.writer && f.writer->isWritingParagraph())
    f.writer->endParagraph();
}

void ProgramMode::fn_class(const FunctionCall& call)
{
  if (currentFrame().node->type() == cxx::Function::TypeId)
    throw BadControlSequence{ "class" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  const auto class_name = std::get<std::string>(call.arguments.front());

  auto new_class = std::make_shared<cxx::Class>(class_name, parent);
  new_class->setDocumentation(std::make_shared<ClassDocumentation>());
  // TODO: set source location

  if (currentFrame().node->is<cxx::Class>())
  {
    auto cla = std::static_pointer_cast<cxx::Class>(currentFrame().node);
    cla->members().push_back({ new_class, cxx::AccessSpecifier::PUBLIC });
  }
  else if (currentFrame().node->is<cxx::Namespace>())
  {
    auto ns = std::static_pointer_cast<cxx::Namespace>(currentFrame().node);
    ns->entities().push_back(new_class);
  }
  else
  {
    throw BadControlSequence{ "class" };
  }

  m_state.enter<FrameType::Class>(new_class);
}

void ProgramMode::cs_endclass()
{
  if (!currentFrame().node->is<cxx::Class>())
    throw BadControlSequence{ Functions::ENDCLASS };

  exitFrame();
}

void ProgramMode::fn_fn(const FunctionCall& call)
{
  if (currentFrame().node->type() == cxx::Function::TypeId)
    throw BadControlSequence{ "function" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  const auto fn_signature = std::get<std::string>(call.arguments.front());

  auto new_fn = std::make_shared<cxx::Function>(fn_signature, parent);
  new_fn->setDocumentation(std::make_shared<FunctionDocumentation>());
  // TODO: set source location

  if (currentFrame().node->is<cxx::Class>())
  {
    auto cla = std::static_pointer_cast<cxx::Class>(currentFrame().node);
    cla->members().push_back({ new_fn, cxx::AccessSpecifier::PUBLIC });
  }
  else if (currentFrame().node->is<cxx::Namespace>())
  {
    auto ns = std::static_pointer_cast<cxx::Namespace>(currentFrame().node);
    ns->entities().push_back(new_fn);
  }
  else
  {
    throw BadControlSequence{ "function" };
  }

  m_state.enter<FrameType::Function>(new_fn);
}

void ProgramMode::cs_endfn()
{
  if (!currentFrame().node->is<cxx::Function>())
    throw BadControlSequence{ "endfunction" };

  exitFrame();
}

void ProgramMode::fn_namespace(const FunctionCall& call)
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadControlSequence{ "namespace" };

  auto parent_ns = std::dynamic_pointer_cast<cxx::Namespace>(currentFrame().node);
  const auto ns_name = std::get<std::string>(call.arguments.front());

  auto new_namespace = std::make_shared<cxx::Namespace>(ns_name, parent_ns);
  new_namespace->setDocumentation(std::make_shared<NamespaceDocumentation>());
  // TODO: set source location

  parent_ns->entities().push_back(new_namespace);

  m_state.enter<FrameType::Namespace>(new_namespace);
}

void ProgramMode::cs_endnamespace()
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadControlSequence{ "endnamespace" };

  exitFrame();
}

void ProgramMode::fn_enum(const FunctionCall& call)
{
  if (!currentFrame().node->is<cxx::Namespace>() && !currentFrame().node->is<cxx::Class>())
    throw BadControlSequence{ "enum" };

  auto parent_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
  const std::string enum_name = call.arg<std::string>(0);

  auto new_enum = std::make_shared<cxx::Enum>(enum_name, parent_entity);
  new_enum->setDocumentation(std::make_shared<EnumDocumentation>());

  if (parent_entity->is<cxx::Namespace>())
  {
    const auto ns = std::static_pointer_cast<cxx::Namespace>(parent_entity);
    ns->entities().push_back(new_enum);
  }
  else
  {
    const auto cla = std::static_pointer_cast<cxx::Class>(parent_entity);
    cla->members().push_back({ new_enum, cxx::AccessSpecifier::PUBLIC });
  }

  m_state.enter<FrameType::Enum>(new_enum);
}

void ProgramMode::cs_endenum()
{
  if (!currentFrame().node->is<cxx::Enum>())
    throw BadControlSequence{ "endenum" };

  exitFrame();
}

void ProgramMode::fn_enumvalue(const FunctionCall& call)
{
  if (currentFrame().type == FrameType::EnumValue)
    exitFrame();

  if (!currentFrame().node->is<cxx::Enum>())
    throw BadControlSequence{ "value" };

  const auto en = std::static_pointer_cast<cxx::Enum>(currentFrame().node);

  std::string name = call.arg<std::string>(0);
  auto enum_value = std::make_shared<cxx::EnumValue>(std::move(name), en);
  enum_value->setDocumentation(std::make_shared<EnumValueDocumentation>());

  en->values().push_back(enum_value);

  // TODO: handle optional since clause

  m_state.enter<FrameType::EnumValue>(enum_value);
}

void ProgramMode::cs_endenumvalue()
{
  if (currentFrame().type != FrameType::EnumValue)
    throw BadControlSequence{ "endenumvalue" };

  exitFrame();
}

void ProgramMode::fn_brief(const FunctionCall& call)
{
  std::string text = std::get<std::string>(call.arguments.front());

  auto entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  doc(entity->documentation()).brief() = std::move(text);
}

void ProgramMode::fn_since(const FunctionCall& call)
{
  auto entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  if (call.options.empty())
  {
    // e.g. \since 5.12
    std::string version = std::get<std::string>(call.arguments.front());
    doc(entity->documentation()).since() = dex::Since{ version };
  }
  else
  {
    if (!currentFrame().writer->handle(call))
      throw UnexpectedControlSequence{ call.function };
  }
}

void ProgramMode::fn_param(const FunctionCall& call)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadControlSequence{ "param" };

  std::string text = std::get<std::string>(call.arguments.front());

  auto entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
  auto doc = std::static_pointer_cast<FunctionDocumentation>(entity->documentation());
  doc->parameters().push_back(std::move(text));
}

void ProgramMode::fn_returns(const FunctionCall& call)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadControlSequence{ "returns" };

  std::string text = std::get<std::string>(call.arguments.front());

  auto entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
  auto doc = std::static_pointer_cast<FunctionDocumentation>(entity->documentation());
  doc->returnValue() = std::move(text);

}

void ProgramMode::childFinished(ParserMode& mode)
{
  assert(("Not implemented", false));
}

void ProgramMode::beginFile()
{
  /* no-op */
}

void ProgramMode::endFile()
{
  while (m_state.depth() > 1)
  {
    exitFrame();
  }
}

void ProgramMode::beginBlock()
{
  /* no-op */
}

void ProgramMode::endBlock()
{
  while (m_state.current().node->is<cxx::Function>())
  {
    exitFrame();
  }
}

ProgramMode::Frame& ProgramMode::currentFrame()
{
  return m_state.current();
}

void ProgramMode::exitFrame()
{
  Frame& f = m_state.current();

  if (f.node->isEntity())
  {
    auto ent = std::static_pointer_cast<cxx::Entity>(f.node);
    f.writer->finish();

    doc(ent->documentation()).description() = std::move(f.writer->output());
  }

  m_state.leave();
}

} // namespace dex
