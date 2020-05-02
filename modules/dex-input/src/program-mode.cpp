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
#include "dex/model/variable-documentation.h"

#include "dex/common/logging.h"

#include <cxx/documentation.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/variable.h>

#include <cxx/parsers/restricted-parser.h>

namespace dex
{

static EntityDocumentation& doc(const std::shared_ptr<cxx::Documentation>& d)
{
  return *static_cast<EntityDocumentation*>(d.get());
}

template<typename T>
std::shared_ptr<T> find(const cxx::Entity& e, const std::string& name)
{
  if (e.kind() == cxx::NodeKind::Class)
  {
    const cxx::Class& cla = static_cast<const cxx::Class&>(e);

    for (auto m : cla.members())
    {
      if (m.first->name() == name)
        return std::dynamic_pointer_cast<T>(m.first);
    }
  }
  else if (e.kind() == cxx::NodeKind::Namespace)
  {
    const cxx::Namespace& ns = static_cast<const cxx::Namespace&>(e);

    for (auto child : ns.entities())
    {
      if (child->name() == name)
        return std::dynamic_pointer_cast<T>(child);
    }
  }

  return nullptr;
}

ProgramMode::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

ProgramMode::Frame::Frame(FrameType ft, std::shared_ptr<cxx::Entity> cxxent)
  : state::Frame<FrameType>(ft)
{
  node = cxxent;
  writer = std::make_shared<DocumentWriterToolchain>();
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
    Frame& f = currentFrame();
    if (!f.writer || !f.writer->handle(call))
      throw BadControlSequence{ call.function };
  }

  return done();
}

void ProgramMode::fn_class(const FunctionCall& call)
{
  if (currentFrame().node->is<cxx::Function>())
    throw BadControlSequence{ "class" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  const auto class_name = std::get<std::string>(call.arguments.front());

  auto the_class = find<cxx::Class>(*static_cast<cxx::Entity*>(currentFrame().node.get()), class_name);

  if (the_class == nullptr)
  {
    the_class = std::make_shared<cxx::Class>(class_name, parent);
    the_class->setDocumentation(std::make_shared<ClassDocumentation>());
    // TODO: set source location

    if (currentFrame().node->is<cxx::Class>())
    {
      auto cla = std::static_pointer_cast<cxx::Class>(currentFrame().node);
      cla->members().push_back({ the_class, cxx::AccessSpecifier::PUBLIC });
    }
    else if (currentFrame().node->is<cxx::Namespace>())
    {
      auto ns = std::static_pointer_cast<cxx::Namespace>(currentFrame().node);
      ns->entities().push_back(the_class);
    }
    else
    {
      throw BadControlSequence{ "class" };
    }
  }

  m_state.enter<FrameType::Class>(the_class);
  m_lastblock_entity = the_class;
}

void ProgramMode::cs_endclass()
{
  if (!currentFrame().node->is<cxx::Class>())
    throw BadControlSequence{ Functions::ENDCLASS };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramMode::fn_fn(const FunctionCall& call)
{
  if (currentFrame().node->is<cxx::Function>())
    throw BadControlSequence{ "function" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  std::string fn_signature = std::get<std::string>(call.arguments.front());

  while (fn_signature.back() == ' ')
    fn_signature.pop_back();

  if (fn_signature.back() != ';')
    fn_signature.push_back(';');

  std::shared_ptr<cxx::Function> the_fn = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseFunctionSignature(fn_signature);
    }
    catch (...)
    {
      LOG_INFO << "could not parse function signature '" << fn_signature << "'";
      return std::make_shared<cxx::Function>(fn_signature, parent);
    }
  }();

  the_fn->setDocumentation(std::make_shared<FunctionDocumentation>());
  // TODO: set source location

  if (currentFrame().node->is<cxx::Class>())
  {
    auto cla = std::static_pointer_cast<cxx::Class>(currentFrame().node);
    cla->members().push_back({ the_fn, cxx::AccessSpecifier::PUBLIC });
  }
  else if (currentFrame().node->is<cxx::Namespace>())
  {
    auto ns = std::static_pointer_cast<cxx::Namespace>(currentFrame().node);
    ns->entities().push_back(the_fn);
  }
  else
  {
    throw BadControlSequence{ "function" };
  }

  m_state.enter<FrameType::Function>(the_fn);
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

  auto the_namespace = find<cxx::Namespace>(*static_cast<cxx::Entity*>(currentFrame().node.get()), ns_name);

  if (the_namespace == nullptr)
  {
    the_namespace = std::make_shared<cxx::Namespace>(ns_name, parent_ns);
    the_namespace->setDocumentation(std::make_shared<NamespaceDocumentation>());
    // TODO: set source location

    parent_ns->entities().push_back(the_namespace);
  }

  m_state.enter<FrameType::Namespace>(the_namespace);
  m_lastblock_entity = the_namespace;
}

void ProgramMode::cs_endnamespace()
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadControlSequence{ "endnamespace" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
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
  m_lastblock_entity = new_enum;
}

void ProgramMode::cs_endenum()
{
  if (!currentFrame().node->is<cxx::Enum>())
    throw BadControlSequence{ "endenum" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramMode::fn_enumvalue(const FunctionCall& call)
{
  if (currentFrame().type == FrameType::EnumValue)
    exitFrame();

  if (!currentFrame().node->is<cxx::Enum>())
  {
    if(m_lastblock_entity->node_kind() != cxx::NodeKind::Enum)
      throw BadControlSequence{ "value" };

    m_state.enter<FrameType::Enum>(m_lastblock_entity);
  }

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

void ProgramMode::fn_variable(const FunctionCall& call)
{
  if (!currentFrame().node->is<cxx::Namespace>() && !currentFrame().node->is<cxx::Class>())
    throw BadControlSequence{ "variable" };

  auto parent_entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  std::string var_decl = std::get<std::string>(call.arguments.front());

  while (var_decl.back() == ' ')
    var_decl.pop_back();

  if (var_decl.back() != ';')
    var_decl.push_back(';');

  std::shared_ptr<cxx::Variable> the_var = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseVariable(var_decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse variable declaration '" << var_decl << "'";
      return std::make_shared<cxx::Variable>(cxx::Type::Auto, var_decl, parent_entity);
    }
  }();

  the_var->setDocumentation(std::make_shared<VariableDocumentation>());

  if (parent_entity->is<cxx::Namespace>())
  {
    const auto ns = std::static_pointer_cast<cxx::Namespace>(parent_entity);
    ns->entities().push_back(the_var);
  }
  else
  {
    const auto cla = std::static_pointer_cast<cxx::Class>(parent_entity);
    cla->members().push_back({ the_var, cxx::AccessSpecifier::PUBLIC });
  }

  m_state.enter<FrameType::Variable>(the_var);
  m_lastblock_entity = the_var;
}

void ProgramMode::cs_endvariable()
{
  if (!currentFrame().node->is<cxx::Variable>())
    throw BadControlSequence{ "endvariable" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
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
  auto is_func_or_enum = [](const std::shared_ptr<cxx::Node>& node) -> bool{
    return node->node_kind() == cxx::NodeKind::Enum || node->node_kind() == cxx::NodeKind::Function
      || node->node_kind() == cxx::NodeKind::Variable;
  };

  while (is_func_or_enum(m_state.current().node))
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

    if(!f.writer->output().empty())
      doc(ent->documentation()).description() = std::move(f.writer->output());
  }

  m_state.leave();
}

} // namespace dex
