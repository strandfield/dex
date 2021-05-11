// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/program-parser.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include "dex/model/class-documentation.h"
#include "dex/model/enum-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"
#include "dex/model/typedef-documentation.h"
#include "dex/model/macro-documentation.h"
#include "dex/model/variable-documentation.h"

#include "dex/common/logging.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/name.h>
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

    for (auto m : cla.members)
    {
      if (m->name == name)
        return std::dynamic_pointer_cast<T>(m);
    }
  }
  else if (e.kind() == cxx::NodeKind::Namespace)
  {
    const cxx::Namespace& ns = static_cast<const cxx::Namespace&>(e);

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

ProgramParser::Frame::Frame(FrameType ft, std::shared_ptr<cxx::Entity> cxxent)
  : state::Frame<FrameType>(ft)
{
  node = cxxent;

  auto entdoc = std::static_pointer_cast<EntityDocumentation>(cxxent->documentation);

  if (!entdoc->description)
    entdoc->description = std::make_shared<dex::Document>();

  writer = std::make_shared<DocumentWriter>(entdoc->description);
}

ProgramParser::ProgramParser(std::shared_ptr<dex::Program> prog)
  : m_program(prog)
{
  m_state.enter<FrameType::Idle>();
  m_state.current().node = prog->globalNamespace();
}

ProgramParser::State& ProgramParser::state()
{
  return m_state;
}

std::shared_ptr<cxx::Entity> ProgramParser::currentEntity() const
{
  return std::static_pointer_cast<cxx::Entity>(m_state.current().node);
}

std::shared_ptr<DocumentWriter> ProgramParser::contentWriter()
{
  return currentFrame().writer;
}

void ProgramParser::class_(std::string name)
{
  if (currentFrame().node->is<cxx::Function>())
    throw BadCall{ "ProgramParser::class()", "\\class cannot be used inside \\fn" };
  else if (currentFrame().node->is<cxx::Enum>())
    throw BadCall{ "ProgramParser::class()", "\\class cannot be used inside \\enum" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  auto the_class = find<cxx::Class>(*static_cast<cxx::Entity*>(currentFrame().node.get()), name);

  if (the_class == nullptr)
  {
    the_class = std::make_shared<cxx::Class>(std::move(name), parent);
    the_class->documentation = std::make_shared<ClassDocumentation>();
    // TODO: set source location

    appendChild(the_class);
  }

  m_state.enter<FrameType::Class>(the_class);
  m_lastblock_entity = the_class;
}

void ProgramParser::endclass()
{
  if (!currentFrame().node->is<cxx::Class>())
    throw BadCall{ "ProgramParser::endclass()", "\\endclass but no \\class" };

  exitFrame();
  m_lastblock_entity = currentEntity();
}

void ProgramParser::fn(std::string signature)
{
  if (currentFrame().node->is<cxx::Function>())
    throw BadCall{ "ProgramParser::fn()", "\\fn cannot be nested" };
  else if (currentFrame().node->is<cxx::Enum>())
    throw BadCall{ "ProgramParser::fn()", "\\fn cannot be used inside \\enum" };

  auto parent = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  while (signature.back() == ' ')
    signature.pop_back();

  if (signature.back() != ';')
    signature.push_back(';');

  std::shared_ptr<cxx::Function> the_fn = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseFunctionSignature(signature);
    }
    catch (...)
    {
      LOG_INFO << "could not parse function signature '" << signature << "'";
      return std::make_shared<cxx::Function>(signature, parent);
    }
  }();

  the_fn->documentation = std::make_shared<FunctionDocumentation>();
  // TODO: set source location

  appendChild(the_fn);

  m_state.enter<FrameType::Function>(the_fn);
}

void ProgramParser::endfn()
{
  if (!currentFrame().node->is<cxx::Function>())
    throw BadCall{ "ProgramParser::endfn()", "\\endfn but no \\fn" };

  exitFrame();
}

void ProgramParser::namespace_(std::string name)
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadCall{ "ProgramParser::namespace()", "\\namespace can only be used inside \\namespace" };

  auto parent_ns = std::dynamic_pointer_cast<cxx::Namespace>(currentFrame().node);

  auto the_namespace = find<cxx::Namespace>(*static_cast<cxx::Entity*>(currentFrame().node.get()), name);

  if (the_namespace == nullptr)
  {
    the_namespace = std::make_shared<cxx::Namespace>(std::move(name), parent_ns);
    the_namespace->documentation = std::make_shared<NamespaceDocumentation>();
    // TODO: set source location

    appendChild(parent_ns, the_namespace);
  }

  m_state.enter<FrameType::Namespace>(the_namespace);
  m_lastblock_entity = the_namespace;
}

void ProgramParser::endnamespace()
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadCall{ "ProgramParser::endnamespace()", "\\endnamespace but no \\namespace" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramParser::enum_(std::string name)
{
  if (!currentFrame().node->is<cxx::Namespace>() && !currentFrame().node->is<cxx::Class>())
    throw BadCall{ "ProgramParser::enum()", "\\enum must be inside of \\namespace or \\class" };

  auto parent_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);

  auto new_enum = std::make_shared<cxx::Enum>(std::move(name), parent_entity);
  new_enum->documentation = std::make_shared<EnumDocumentation>();

  if (parent_entity->is<cxx::Namespace>())
  {
    const auto ns = std::static_pointer_cast<cxx::Namespace>(parent_entity);
    ns->entities.push_back(new_enum);
  }
  else
  {
    const auto cla = std::static_pointer_cast<cxx::Class>(parent_entity);
    cla->members.push_back(new_enum);
  }

  m_state.enter<FrameType::Enum>(new_enum);
  m_lastblock_entity = new_enum;
}

void ProgramParser::endenum()
{
  if (!currentFrame().node->is<cxx::Enum>())
    throw BadCall{ "ProgramParser::endenum()", "\\endenum but no \\enum" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramParser::value(std::string name)
{
  if (currentFrame().type == FrameType::EnumValue)
    exitFrame();

  if (!currentFrame().node->is<cxx::Enum>())
  {
    if (m_lastblock_entity->node_kind() != cxx::NodeKind::Enum)
      throw BadCall{ "ProgramParser::value()", "\\value must be near \\enum" };

    m_state.enter<FrameType::Enum>(m_lastblock_entity);
  }

  const auto en = std::static_pointer_cast<cxx::Enum>(currentFrame().node);

  auto enum_value = std::make_shared<cxx::EnumValue>(std::move(name), en);
  enum_value->documentation = std::make_shared<EnumValueDocumentation>();

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
  if (!currentFrame().node->is<cxx::Namespace>() && !currentFrame().node->is<cxx::Class>())
    throw BadCall{ "ProgramParser::variable()", "\\variable must be inside \\namespace or \\class" };

  auto parent_entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  if (decl.back() != ';')
    decl.push_back(';');

  std::shared_ptr<cxx::Variable> the_var = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseVariable(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse variable declaration '" << decl << "'";
      return std::make_shared<cxx::Variable>(cxx::Type::Auto, decl, parent_entity);
    }
  }();

  the_var->documentation = std::make_shared<VariableDocumentation>();

  if (parent_entity->is<cxx::Namespace>())
  {
    const auto ns = std::static_pointer_cast<cxx::Namespace>(parent_entity);
    ns->entities.push_back(the_var);
  }
  else
  {
    const auto cla = std::static_pointer_cast<cxx::Class>(parent_entity);
    cla->members.push_back(the_var);
  }

  m_state.enter<FrameType::Variable>(the_var);
  m_lastblock_entity = the_var;
}

void ProgramParser::endvariable()
{
  if (!currentFrame().node->is<cxx::Variable>())
    throw BadCall{ "ProgramParser::endvariable()", "\\endvariable but no \\variable" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramParser::typedef_(std::string decl)
{
  if (!currentFrame().node->is<cxx::Namespace>() && !currentFrame().node->is<cxx::Class>())
    throw BadCall{ "ProgramParser::typedef()", "\\typedef must be inside \\namespace or \\class" };

  auto parent_entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  if (decl.back() != ';')
    decl.push_back(';');
  
  decl = "typedef " + decl;

  std::shared_ptr<cxx::Typedef> the_typedef = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseTypedef(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse variable declaration '" << decl << "'";
      return std::make_shared<cxx::Typedef>(cxx::Type::Auto, decl, parent_entity);
    }
  }();

  the_typedef->documentation = std::make_shared<TypedefDocumentation>();

  if (parent_entity->is<cxx::Namespace>())
  {
    const auto ns = std::static_pointer_cast<cxx::Namespace>(parent_entity);
    ns->entities.push_back(the_typedef);
  }
  else
  {
    const auto cla = std::static_pointer_cast<cxx::Class>(parent_entity);
    cla->members.push_back(the_typedef);
  }

  m_state.enter<FrameType::Typedef>(the_typedef);
  m_lastblock_entity = the_typedef;
}

void ProgramParser::endtypedef()
{
  if (!currentFrame().node->is<cxx::Typedef>())
    throw BadCall{ "ProgramParser::endtypedef()", "\\endtypedef but no \\typedef" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramParser::macro(std::string decl)
{
  if (!currentFrame().node->is<cxx::Namespace>())
    throw BadCall{ "ProgramParser::typedef()", "\\macro must be inside \\namespace" };

  auto parent_entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);

  while (decl.back() == ' ')
    decl.pop_back();

  std::shared_ptr<cxx::Macro> the_macro = [&]() {
    try
    {
      return cxx::parsers::RestrictedParser::parseMacro(decl);
    }
    catch (...)
    {
      LOG_INFO << "could not parse macro declaration '" << decl << "'";
      // failing to parse a macro means its very likely the input is malformed
      throw ParserException{"bad syntax for \\macro"};
    }
  }();

  the_macro->documentation = std::make_shared<MacroDocumentation>();

  m_program->macros.push_back(the_macro);

  m_state.enter<FrameType::Macro>(the_macro);
  m_lastblock_entity = the_macro;
}

void ProgramParser::endmacro()
{
  if (!currentFrame().node->is<cxx::Macro>())
    throw BadCall{ "ProgramParser::macro()", "\\endmacro but no \\macro" };

  exitFrame();
  m_lastblock_entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
}

void ProgramParser::brief(std::string brieftext)
{
  auto entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  doc(entity->documentation).brief() = std::move(brieftext);
}

void ProgramParser::since(std::string version)
{
  auto entity = std::dynamic_pointer_cast<cxx::Entity>(currentFrame().node);
  doc(entity->documentation).since() = dex::Since{ version };
}

void ProgramParser::param(std::string des)
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::param()", "\\param must inside \\fn" };

  auto fun = std::static_pointer_cast<cxx::Function>(currentFrame().node);
  auto param_doc = std::make_shared<dex::FunctionParameterDocumentation>(std::move(des));

  for (size_t i(0); fun->parameters.size(); ++i)
  {
    if (fun->parameters.at(i)->documentation == nullptr)
    {
      fun->parameters.at(i)->documentation = param_doc;
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

  auto entity = std::static_pointer_cast<cxx::Entity>(currentFrame().node);
  auto doc = std::static_pointer_cast<FunctionDocumentation>(entity->documentation);
  doc->returnValue() = std::move(des);
}

void ProgramParser::nonmember()
{
  Frame& f = currentFrame();

  if (f.type != FrameType::Function)
    throw BadCall{ "ProgramParser::nonmember()", "\\nonmember must be inside \\fn" };

  auto func = std::static_pointer_cast<cxx::Function>(currentFrame().node);

  if(!func->parent()->is<cxx::Class>())
    throw BadCall{ "ProgramParser::nonmember()", "\\nonmember cannot be used for \\fn outside of \\class" };

  auto the_class = std::static_pointer_cast<cxx::Class>(func->parent());

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

  auto func = std::static_pointer_cast<cxx::Function>(currentFrame().node);

  std::shared_ptr<cxx::Entity> parent = func->parent();

  auto the_class = m_program->resolve(cxx::Name::fromSimpleIdentifier(class_name), parent);

  if(!the_class || !the_class->is<cxx::Class>())
    throw BadCall{ "ProgramParser::relates()", "\\relates must specifiy a known class name" };

  m_program->related.relates(func, std::static_pointer_cast<cxx::Class>(the_class));
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
  auto is_terminal_node = [](const std::shared_ptr<cxx::Node>& node) -> bool{
    return node->node_kind() == cxx::NodeKind::Enum || node->node_kind() == cxx::NodeKind::Function
      || node->node_kind() == cxx::NodeKind::Variable
      || node->node_kind() == cxx::NodeKind::Typedef
      || node->node_kind() == cxx::NodeKind::Macro;
  };

  while (is_terminal_node(m_state.current().node))
  {
    exitFrame();
  }
}

ProgramParser::Frame& ProgramParser::currentFrame()
{
  return m_state.current();
}

void ProgramParser::exitFrame()
{
  Frame& f = m_state.current();

  if (f.node->isEntity())
  {
    auto ent = std::static_pointer_cast<cxx::Entity>(f.node);
    f.writer->finish();
  }

  m_state.leave();
}

void ProgramParser::appendChild(std::shared_ptr<cxx::Entity> e)
{
  appendChild(currentFrame().node, e);
}

void ProgramParser::appendChild(std::shared_ptr<cxx::Node> parent, std::shared_ptr<cxx::Entity> child)
{
  switch (parent->kind())
  {
  case cxx::NodeKind::Namespace:
    parent->get<cxx::Namespace::Entities>().push_back(child);
    break;
  case cxx::NodeKind::Class:
    parent->get<cxx::Class::Members>().push_back(child);
    break;
  case cxx::NodeKind::Enum:
    parent->get<cxx::Enum::Values>().push_back(std::static_pointer_cast<cxx::EnumValue>(child));
    break;
  default:
    throw std::runtime_error("ProgramParser::appendChild() failed");
  }
  
  child->weak_parent = std::static_pointer_cast<cxx::Entity>(parent);
}

} // namespace dex
