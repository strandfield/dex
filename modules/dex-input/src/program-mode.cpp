// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/program-mode.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-machine.h"
#include "dex/input/parser-errors.h"

#include "dex/model/class-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"

#include "dex/common/logging.h"

#include <cxx/documentation.h>
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
    {Functions::BRIEF, CS::BRIEF},
    {Functions::SINCE, CS::SINCE},
    {Functions::BEGINSINCE, CS::BEGINSINCE},
    {Functions::ENDSINCE, CS::ENDSINCE},
    {Functions::PARAM, CS::PARAM},
    {Functions::RETURNS, CS::RETURNS},
    {Functions::ENDLIST, CS::ENDLIST},
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

bool ProgramMode::write(tex::parsing::Token&& tok)
{
  switch (currentFrame().type)
  {
  case FrameType::Idle:
    write_idle(std::move(tok));
    break;
  case FrameType::Class:
  case FrameType::Function:
  case FrameType::Namespace:
    write_entity(std::move(tok));
    break;
  }

  return done();
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
  else if (call.function == Functions::BEGINSINCE)
  {
    fn_beginsince(call);
  }
  else if (call.function == Functions::RETURNS)
  {
    fn_returns(call);
  }
  else
  {
    Frame& f = currentFrame();
    if (f.writer)
    {
      f.writer->handle(call);
    }
    else
    {
      throw BadControlSequence{ call.function };
    }
  }

  return done();
}

void ProgramMode::write_idle(tex::parsing::Token&& tok)
{
  if (tok.isCharacterToken())
  {
    if (tok.characterToken().category != tex::parsing::CharCategory::Space)
    {
      LOG_WARNING << "Non-space character ignored";
      return;
    }
  }

  auto it = csmap().find(tok.controlSequence());

  if (it == csmap().end())
    throw UnknownControlSequence{ tok.controlSequence() };

  CS cs = it->second;

  switch (cs)
  {
  case CS::PAR:
    return;
  default:
    throw UnexpectedControlSequence{ tok.controlSequence() };
  }
}

void ProgramMode::cs_par()
{
  Frame& f = currentFrame();

  if (f.writer && f.writer->isWritingParagraph())
    f.writer->endParagraph();
}

void ProgramMode::cs_endlist()
{
  Frame& f = currentFrame();

  FunctionCall call;
  call.function = Functions::ENDLIST;
  f.writer->handle(call);
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

void ProgramMode::write_entity(tex::parsing::Token&& tok)
{
  if (tok.isCharacterToken())
  {
    currentFrame().writer->write(tok.characterToken().value);
  }
  else
  {
    CS cs = parseCs(tok.controlSequence());

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
    case CS::ENDSINCE:
      return cs_endsince();
    case CS::ENDLIST:
      return cs_endlist();
    default:
      throw UnexpectedControlSequence{ tok.controlSequence() };
    }
  }
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
    currentFrame().writer->handle(call);
  }
}

void ProgramMode::fn_beginsince(const FunctionCall& call)
{
  Frame& f = currentFrame();
  f.writer->handle(call);
}

void ProgramMode::cs_endsince()
{
  Frame& f = currentFrame();
  f.writer->endSinceBlock();
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
