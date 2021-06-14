// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/program-parser.h"

#include "dex/input/parser-machine.h"
#include "dex/input/paragraph-writer.h"
#include "dex/input/cpp-parser.h"
#include "dex/input/enum-parser.h"
#include "dex/input/parser-errors.h"

#include "dex/model/program.h"

#include "dex/common/logging.h"

namespace dex
{

template<typename T>
std::shared_ptr<T> find(const dex::Entity& e, const std::string& name)
{
  if (e.kind() == model::Kind::Class)
  {
    const dex::Class& cla = static_cast<const dex::Class&>(e);

    for (auto m : cla.members)
    {
      if (m->name == name)
        return std::dynamic_pointer_cast<T>(m);
    }
  }
  else if (e.kind() == model::Kind::Namespace)
  {
    const dex::Namespace& ns = static_cast<const dex::Namespace&>(e);

    for (auto child : ns.entities)
    {
      if (child->name == name)
        return std::dynamic_pointer_cast<T>(child);
    }
  }

  return nullptr;
}

ProgramParser::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

ProgramParser::Frame::Frame(FrameType ft, std::shared_ptr<dex::Entity> cxxent)
  : state::Frame<FrameType>(ft)
{
  node = cxxent;

  if(!cxxent->description)
    cxxent->description = std::make_shared<dex::Document>();

  writer = std::make_shared<DocumentWriter>(cxxent->description);
}

ProgramParser::ProgramParser(ParserMachine& m)
  : m_machine(m),
    m_program(m.output()->getOrCreateProgram())
{
  m_state.enter<FrameType::Idle>();
  m_state.current().node = m_program->globalNamespace();
}

ParserMachine& ProgramParser::machine() const
{
  return m_machine;
}

ProgramParser::State& ProgramParser::state()
{
  return m_state;
}

std::shared_ptr<dex::Entity> ProgramParser::currentEntity() const
{
  return m_state.current().node;
}

std::shared_ptr<DocumentWriter> ProgramParser::contentWriter()
{
  return currentFrame().writer;
}

void ProgramParser::class_(std::string name)
{
  exitGhost();

  if (currentFrame().node->is<dex::Function>())
    throw BadCall{ "ProgramParser::class()", "\\class cannot be used inside \\fn" };
  else if (currentFrame().node->is<dex::Enum>())
    throw BadCall{ "ProgramParser::class()", "\\class cannot be used inside \\enum" };

  auto parent = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);

  auto the_class = find<dex::Class>(*static_cast<dex::Entity*>(currentFrame().node.get()), name);

  if (the_class == nullptr)
  {
    the_class = std::make_shared<dex::Class>(std::move(name), parent);
    // TODO: set source location

    appendChild(the_class);
  }

  m_state.enter<FrameType::Class>(the_class);
}

void ProgramParser::endclass()
{
  if (!currentFrame().node->is<dex::Class>())
    throw BadCall{ "ProgramParser::endclass()", "\\endclass but no \\class" };

  exitFrame();
}

void ProgramParser::fn(std::string signature)
{
  exitGhost();

  if (currentFrame().node->is<dex::Function>())
    throw BadCall{ "ProgramParser::fn()", "\\fn cannot be nested" };
  else if (currentFrame().node->is<dex::Enum>())
    throw BadCall{ "ProgramParser::fn()", "\\fn cannot be used inside \\enum" };

  auto parent = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);

  while (signature.back() == ' ')
    signature.pop_back();

  if (signature.back() != ';')
    signature.push_back(';');

  std::shared_ptr<dex::Function> the_fn = [&]() {
    try
    {
      return dex::CppParser::parseFunctionSignature(signature);
    }
    catch (...)
    {
      LOG_INFO << "could not parse function signature '" << signature << "'";
      return std::make_shared<dex::Function>(signature, parent);
    }
  }();

  // TODO: set source location

  appendChild(the_fn);

  m_state.enter<FrameType::Function>(the_fn);
}

void ProgramParser::endfn()
{
  if (!currentFrame().node->is<dex::Function>())
    throw BadCall{ "ProgramParser::endfn()", "\\endfn but no \\fn" };

  exitFrame();
}

void ProgramParser::namespace_(std::string name)
{
  exitGhost();

  if (!currentFrame().node->is<dex::Namespace>())
    throw BadCall{ "ProgramParser::namespace()", "\\namespace can only be used inside \\namespace" };

  auto parent_ns = std::dynamic_pointer_cast<dex::Namespace>(currentFrame().node);

  auto the_namespace = find<dex::Namespace>(*static_cast<dex::Entity*>(currentFrame().node.get()), name);

  if (the_namespace == nullptr)
  {
    the_namespace = std::make_shared<dex::Namespace>(std::move(name), parent_ns);
    // TODO: set source location

    appendChild(parent_ns, the_namespace);
  }

  m_state.enter<FrameType::Namespace>(the_namespace);
}

void ProgramParser::endnamespace()
{
  if (!currentFrame().node->is<dex::Namespace>())
    throw BadCall{ "ProgramParser::endnamespace()", "\\endnamespace but no \\namespace" };

  exitFrame();
}

void ProgramParser::enum_(std::string name)
{
  exitGhost();

  if (!currentFrame().node->is<dex::Namespace>() && !currentFrame().node->is<dex::Class>())
    throw BadCall{ "ProgramParser::enum()", "\\enum must be inside of \\namespace or \\class" };

  auto parent_entity = std::static_pointer_cast<dex::Entity>(currentFrame().node);

  auto new_enum = std::make_shared<dex::Enum>(std::move(name), parent_entity);

  if (parent_entity->is<dex::Namespace>())
  {
    const auto ns = std::static_pointer_cast<dex::Namespace>(parent_entity);
    ns->entities.push_back(new_enum);
  }
  else
  {
    const auto cla = std::static_pointer_cast<dex::Class>(parent_entity);
    cla->members.push_back(new_enum);
  }

  state().enter<FrameType::Enum>(new_enum);

  state().current().block_offset = machine().inputStream().blockPosition().offset;
}

void ProgramParser::endenum()
{
  if (currentFrame().node->is<dex::EnumValue>())
    exitFrame();

  if (!currentFrame().node->is<dex::Enum>())
    throw BadCall{ "ProgramParser::endenum()", "\\endenum but no \\enum" };

  int document_offset = machine().inputStream().blockPosition().offset;

  if (document_offset > currentFrame().block_offset)
  {
    // \enum and \endenum appeared in different blocks.
    // This means that the enum declaration is possibly between the two 
    // blocks.
    // Let's try to parse the values.

    const std::string& document_source = machine().inputStream().currentDocument().content;
    size_t len = static_cast<size_t>(document_offset - currentFrame().block_offset);
    std::string source = document_source.substr(currentFrame().block_offset, len);

    EnumParser eparser{ std::static_pointer_cast<dex::Enum>(currentFrame().node) };
    eparser.parse(source);
  }

  exitFrame();
}

void ProgramParser::value(std::string name)
{
  if (currentFrame().type == FrameType::EnumValue)
    exitFrame();

  if (!currentFrame().node->is<dex::Enum>())
    throw BadCall{ "ProgramParser::value()", "\\value must be near \\enum" };

  const auto en = std::static_pointer_cast<dex::Enum>(currentFrame().node);

  auto enum_value = std::make_shared<dex::EnumValue>(std::move(name), en);

  en->values.push_back(enum_value);

  // TODO: handle optional since clause

  m_state.enter<FrameType::EnumValue>(enum_value);
}

void ProgramParser::endenumvalue()
{
  if (currentFrame().type != FrameType::EnumValue)
    throw BadCall{ "ProgramParser::endenumvalue()", "\\endenumvalue must no \\value" };

  exitFrame();
}

void ProgramParser::variable(std::string decl)
{
  exitGhost();

  if (!currentFrame().node->is<dex::Namespace>() && !currentFrame().node->is<dex::Class>())
    throw BadCall{ "ProgramParser::variable()", "\\variable must be inside \\namespace or \\class" };

  auto parent_entity = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  if (decl.back() != ';')
    decl.push_back(';');

  std::shared_ptr<dex::Variable> the_var = [&]() {
    try
    {
      return dex::CppParser::parseVariable(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse variable declaration '" << decl << "'";
      return std::make_shared<dex::Variable>("auto", decl, parent_entity);
    }
  }();

  if (parent_entity->is<dex::Namespace>())
  {
    const auto ns = std::static_pointer_cast<dex::Namespace>(parent_entity);
    ns->entities.push_back(the_var);
  }
  else
  {
    const auto cla = std::static_pointer_cast<dex::Class>(parent_entity);
    cla->members.push_back(the_var);
  }

  m_state.enter<FrameType::Variable>(the_var);
}

void ProgramParser::endvariable()
{
  if (!currentFrame().node->is<dex::Variable>())
    throw BadCall{ "ProgramParser::endvariable()", "\\endvariable but no \\variable" };

  exitFrame();
}

void ProgramParser::typedef_(std::string decl)
{
  exitGhost();

  if (!currentFrame().node->is<dex::Namespace>() && !currentFrame().node->is<dex::Class>())
    throw BadCall{ "ProgramParser::typedef()", "\\typedef must be inside \\namespace or \\class" };

  auto parent_entity = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  if (decl.back() != ';')
    decl.push_back(';');
  
  decl = "typedef " + decl;

  std::shared_ptr<dex::Typedef> the_typedef = [&]() {
    try
    {
      return dex::CppParser::parseTypedef(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse variable declaration '" << decl << "'";
      return std::make_shared<dex::Typedef>("auto", decl, parent_entity);
    }
  }();

  if (parent_entity->is<dex::Namespace>())
  {
    const auto ns = std::static_pointer_cast<dex::Namespace>(parent_entity);
    ns->entities.push_back(the_typedef);
  }
  else
  {
    const auto cla = std::static_pointer_cast<dex::Class>(parent_entity);
    cla->members.push_back(the_typedef);
  }

  m_state.enter<FrameType::Typedef>(the_typedef);
}

void ProgramParser::endtypedef()
{
  if (!currentFrame().node->is<dex::Typedef>())
    throw BadCall{ "ProgramParser::endtypedef()", "\\endtypedef but no \\typedef" };

  exitFrame();
}

void ProgramParser::macro(std::string decl)
{
  exitGhost();

  if (!currentFrame().node->is<dex::Namespace>())
    throw BadCall{ "ProgramParser::typedef()", "\\macro must be inside \\namespace" };

  auto parent_entity = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  std::shared_ptr<dex::Macro> the_macro = [&]() {
    try
    {
      return dex::CppParser::parseMacro(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse macro declaration '" << decl << "'";
      // failing to parse a macro means its very likely the input is malformed
      throw ParserException{"bad syntax for \\macro"};
    }
  }();

  m_program->macros.push_back(the_macro);

  m_state.enter<FrameType::Macro>(the_macro);
}

void ProgramParser::endmacro()
{
  if (!currentFrame().node->is<dex::Macro>())
    throw BadCall{ "ProgramParser::macro()", "\\endmacro but no \\macro" };

  exitFrame();
}

void ProgramParser::brief(std::string brieftext)
{
  auto entity = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);
  entity->brief = std::move(brieftext);
}

void ProgramParser::since(std::string version)
{
  auto entity = std::dynamic_pointer_cast<dex::Entity>(currentFrame().node);
  entity->since = dex::Since{ version };
}

void ProgramParser::param(std::string des)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::param()", "\\param must inside \\fn" };

  auto fun = std::static_pointer_cast<dex::Function>(currentFrame().node);

  for (size_t i(0); fun->parameters.size(); ++i)
  {
    if (!fun->parameters.at(i)->brief.has_value())
    {
      fun->parameters.at(i)->brief = des;
      return;
    }
  }

  // @TODO: should we throw for this ignored piece of documentation
}

void ProgramParser::returns(std::string des)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::returns()", "\\returns must inside \\fn" };

  auto entity = std::static_pointer_cast<dex::Function>(currentFrame().node);
  entity->return_type.brief = std::move(des);
}

void ProgramParser::nonmember()
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::nonmember()", "\\nonmember must be inside \\fn" };

  auto func = std::static_pointer_cast<dex::Function>(currentFrame().node);

  if(!func->parent()->is<dex::Class>())
    throw BadCall{ "ProgramParser::nonmember()", "\\nonmember cannot be used for \\fn outside of \\class" };

  auto the_class = std::static_pointer_cast<dex::Class>(func->parent());

  // perform the re-parenting
  the_class->members.pop_back();
  appendChild(the_class->parent(), func);

  m_program->related.relates(func, the_class);
}

void ProgramParser::relates(const std::string& class_name)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::relates()", "\\relates must be inside \\fn" };

  auto func = std::static_pointer_cast<dex::Function>(currentFrame().node);

  std::shared_ptr<dex::Entity> parent = func->parent();

  auto the_class = m_program->resolve(class_name, parent);

  if(!the_class || !the_class->is<dex::Class>())
    throw BadCall{ "ProgramParser::relates()", "\\relates must specifiy a known class name" };

  m_program->related.relates(func, std::static_pointer_cast<dex::Class>(the_class));
}

void ProgramParser::beginFile()
{
  /* no-op */
}

void ProgramParser::endFile()
{
  while (m_state.depth() > 1)
  {
    exitFrame();
  }
}

void ProgramParser::beginBlock()
{
  /* no-op */
}

void ProgramParser::endBlock()
{
  auto is_terminal_node = [](const std::shared_ptr<dex::Entity>& node) -> bool{
    return node->kind() == model::Kind::EnumValue 
      || node->kind() == model::Kind::Function
      || node->kind() == model::Kind::Variable
      || node->kind() == model::Kind::Typedef
      || node->kind() == model::Kind::Macro;
  };

  while (is_terminal_node(m_state.current().node))
  {
    exitFrame();
  }

  if (m_state.current().node->is<dex::Enum>())
  {
    m_state.current().ghost = true;
  }
}

ProgramParser::Frame& ProgramParser::currentFrame()
{
  return m_state.current();
}

void ProgramParser::exitGhost()
{
  while (state().current().ghost)
  {
    exitFrame();
  }
}

void ProgramParser::exitFrame()
{
  Frame& f = m_state.current();

  if (f.node->isProgramEntity())
  {
    auto ent = std::static_pointer_cast<dex::Entity>(f.node);
    f.writer->finish();
  }

  m_state.leave();
}

void ProgramParser::appendChild(std::shared_ptr<dex::Entity> e)
{
  appendChild(currentFrame().node, e);
}

void ProgramParser::appendChild(std::shared_ptr<dex::Entity> parent, std::shared_ptr<dex::Entity> child)
{
  switch (parent->kind())
  {
  case model::Kind::Namespace:
    static_cast<dex::Namespace&>(*parent).entities.push_back(child);
    break;
  case model::Kind::Class:
    static_cast<dex::Class&>(*parent).members.push_back(child);
    break;
  case model::Kind::Enum:
    static_cast<dex::Enum&>(*parent).values.push_back(std::static_pointer_cast<dex::EnumValue>(child));
    break;
  default:
    throw std::runtime_error("ProgramParser::appendChild() failed");
  }
  
  child->weak_parent = std::static_pointer_cast<dex::Entity>(parent);
}

} // namespace dex
