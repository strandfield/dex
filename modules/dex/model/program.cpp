// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/program.h"

#include <stdexcept>

namespace dex
{

std::string to_string(dex::AccessSpecifier as)
{
  switch (as)
  {
  case dex::AccessSpecifier::PRIVATE:
    return "private";
  case dex::AccessSpecifier::PROTECTED:
    return "protected";
  default:
    return "public";
  }
}

std::shared_ptr<Entity> Entity::shared_from_this()
{
  return std::static_pointer_cast<Entity>(model::Object::shared_from_this());
}

std::shared_ptr<const Entity> Entity::shared_from_this() const
{
  return std::static_pointer_cast<const Entity>(model::Object::shared_from_this());
}

bool Entity::isProgramEntity() const
{
  return true;
}

AccessSpecifier Entity::getAccessSpecifier() const
{
  return AccessSpecifier::PUBLIC;
}

void Entity::setAccessSpecifier(AccessSpecifier aspec)
{
  if (aspec != AccessSpecifier::PUBLIC)
    throw std::runtime_error{ "bad call to Entity::setAccessSpecifier()" };
}


model::Kind Macro::kind() const
{
  return ClassKind;
}




EnumValue::EnumValue(std::string name, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent)
{

}

EnumValue::EnumValue(std::string name, std::string value, std::shared_ptr<Enum> parent)
  : Entity(std::move(name), parent),
  m_value(std::move(value))
{

}

model::Kind EnumValue::kind() const
{
  return ClassKind;
}

std::shared_ptr<Enum> EnumValue::parent() const
{
  return std::static_pointer_cast<Enum>(Entity::parent());
}

std::string& EnumValue::value()
{
  return m_value;
}

const std::string& EnumValue::value() const
{
  return m_value;
}


model::Kind Enum::kind() const
{
  return ClassKind;
}

AccessSpecifier Enum::getAccessSpecifier() const
{
  return access_specifier;
}

void Enum::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}



model::Kind TemplateParameter::kind() const
{
  return ClassKind;
}

TemplateParameter::TemplateParameter()
  : Entity(""),
    m_is_type_parameter(true)
{
  std::get<TemplateNonTypeParameter>(m_data).type = "auto";
}

TemplateParameter::TemplateParameter(std::string name, TemplateNonTypeParameter tntp)
  : Entity(std::move(name)),
    m_is_type_parameter(false)
{
  std::get<TemplateNonTypeParameter>(m_data) = tntp;
}

TemplateParameter::TemplateParameter(std::string name, TemplateTypeParameter ttp)
  : Entity(std::move(name)),
    m_is_type_parameter(true)
{
  std::get<TemplateTypeParameter>(m_data) = ttp;
}

bool TemplateParameter::isTypeParameter() const
{
  return m_is_type_parameter;
}

bool TemplateParameter::isNonTypeParameter() const
{
  return !m_is_type_parameter;
}

bool TemplateParameter::hasDefaultValue()
{
  return (isTypeParameter() && std::get<TemplateTypeParameter>(m_data).default_value != "auto")
    || !std::get<TemplateNonTypeParameter>(m_data).default_value.empty();
}

//TemplateArgument::TemplateArgument(Expression expr)
//{
//  std::get<Expression>(m_data) = expr;
//}
//
//TemplateArgument::TemplateArgument(const Type& type)
//{
//  std::get<Type>(m_data) = type;
//}

TemplateArgument::TemplateArgument(std::string expr_or_type)
{
  std::get<std::string>(m_data) = expr_or_type;
}

bool TemplateArgument::isType() const
{
  return std::get<std::string>(m_data).empty();
}

std::string TemplateArgument::toString() const
{
  return isType() ? get<Type>() : get<std::string>();
}



FunctionParameter::FunctionParameter(Type t, std::string name, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
  type(std::move(t))
{

}

FunctionParameter::FunctionParameter(Type t, std::string name, Expression default_val, std::shared_ptr<Function> parent)
  : Entity(std::move(name), parent),
  type(std::move(t)),
  default_value(std::move(default_val))
{

}

model::Kind FunctionParameter::kind() const
{
  return ClassKind;
}

std::shared_ptr<Function> FunctionParameter::parent() const
{
  return std::static_pointer_cast<Function>(Entity::parent());
}

model::Kind Function::kind() const
{
  return ClassKind;
}

AccessSpecifier Function::getAccessSpecifier() const
{
  return access_specifier;
}

void Function::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

bool Function::isTemplate() const
{
  return !template_parameters.empty();
}

const std::vector<std::shared_ptr<TemplateParameter>>& Function::templateParameters() const
{
  return template_parameters;
}

static void write_params(const Function& f, std::string& out)
{
  for (auto p : f.parameters)
  {
    out += p->type;

    if (!p->name.empty())
      out += " " + p->name;

    out += ", ";
  }

  if (!f.parameters.empty())
  {
    out.pop_back();
    out.pop_back();
  }
}

std::string Function::signature() const
{
  if (isConstructor())
  {
    std::string result;
    if (isExplicit())
      result += "explicit ";
    result += this->name;
    result += "(";
    write_params(*this, result);
    result += ")";

    if (this->specifiers & FunctionSpecifier::Delete)
      result += " = delete";
    else if (this->specifiers & FunctionSpecifier::Default)
      result += " = default";

    return result;
  }
  else if (isDestructor())
  {
    std::string result;

    if (isVirtual())
      result += "virtual ";

    result += this->name;
    result += "()";

    if (this->specifiers & FunctionSpecifier::Delete)
      result += " = delete";
    else if (this->specifiers & FunctionSpecifier::Default)
      result += " = default";

    return result;
  }

  std::string result;

  if (isInline())
    result += "inline ";
  if (isExplicit())
    result += "explicit ";
  if (isStatic())
    result += "static ";
  if (isVirtual())
    result += "virtual ";

  result += this->return_type.type;
  result += " " + this->name;
  result += "(";
  write_params(*this, result);
  result += ")";

  if (isConst())
    result += " const";

  if (this->specifiers & FunctionSpecifier::Delete)
    result += " = delete";
  else if (this->specifiers & FunctionSpecifier::Default)
    result += " = default";
  else if (isVirtualPure())
    result += " = 0";

  return result;
}

std::string Function::specifiersList() const
{
  std::string r;

  if (specifiers & dex::FunctionSpecifier::Inline)
    r += "inline,";
  if (specifiers & dex::FunctionSpecifier::Static)
    r += "static,";
  if (specifiers & dex::FunctionSpecifier::Constexpr)
    r += "constexpr,";
  if (specifiers & dex::FunctionSpecifier::Virtual)
    r += "virtual,";
  if (specifiers & dex::FunctionSpecifier::Override)
    r += "override,";
  if (specifiers & dex::FunctionSpecifier::Final)
    r += "final,";
  if (specifiers & dex::FunctionSpecifier::Const)
    r += "const,";

  if(!r.empty())
  r.pop_back();

  return r;
}


model::Kind Class::kind() const
{
  return ClassKind;
}

AccessSpecifier Class::getAccessSpecifier() const
{
  return access_specifier;
}

void Class::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}

bool Class::isTemplate() const
{
  return !template_parameters.empty();
}

const std::vector<std::shared_ptr<TemplateParameter>>& Class::templateParameters() const
{
  return template_parameters;
}



model::Kind Variable::kind() const
{
  return ClassKind;
}

Expression& Variable::defaultValue()
{
  return m_default_value;
}

const Expression& Variable::defaultValue() const
{
  return m_default_value;
}

int& Variable::specifiers()
{
  return m_flags;
}

int Variable::specifiers() const
{
  return m_flags;
}



model::Kind Typedef::kind() const
{
  return Typedef::ClassKind;
}

AccessSpecifier Typedef::getAccessSpecifier() const
{
  return access_specifier;
}

void Typedef::setAccessSpecifier(AccessSpecifier aspec)
{
  access_specifier = aspec;
}



model::Kind Namespace::kind() const
{
  return ClassKind;
}

std::shared_ptr<Namespace> Namespace::getOrCreateNamespace(const std::string& name)
{
  auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<Entity>& e) {
    return e->is<Namespace>() && e->name == name;
    });

  if (it != entities.end())
    return std::static_pointer_cast<Namespace>(*it);

  auto result = std::make_shared<Namespace>(name, shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Class> Namespace::createClass(std::string name)
{
  auto result = std::make_shared<Class>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Class> Namespace::getOrCreateClass(const std::string& name)
{
  auto it = std::find_if(entities.begin(), entities.end(), [&name](const std::shared_ptr<Entity>& e) {
    return e->is<Class>() && e->name == name;
    });

  if (it != entities.end())
    return std::static_pointer_cast<Class>(*it);

  auto result = std::make_shared<Class>(name, shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Enum> Namespace::createEnum(std::string name)
{
  auto result = std::make_shared<Enum>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}

std::shared_ptr<Function> Namespace::createFunction(std::string name)
{
  auto result = std::make_shared<Function>(std::move(name), shared_from_this());
  entities.push_back(result);
  return result;
}



bool RelatedNonMembers::empty() const
{
  return class_map.empty();
}

void RelatedNonMembers::relates(const std::shared_ptr<Function>& f, const std::shared_ptr<Class>& c)
{
  auto& entry = class_map[c];

  if (entry == nullptr)
    entry = std::make_shared<Entry>(c);

  entry->non_members.push_back(f);
  functions_map[f] = entry;
}

std::shared_ptr<RelatedNonMembers::Entry> RelatedNonMembers::getRelated(const std::shared_ptr<Class>& c) const
{
  auto it = class_map.find(c);
  return it == class_map.end() ? nullptr : it->second;
}

std::shared_ptr<RelatedNonMembers::Entry> RelatedNonMembers::getRelated(const std::shared_ptr<Function>& f) const
{
  auto it = functions_map.find(f);
  return it == functions_map.end() ? nullptr : it->second;
}


Program::Program()
{
  global_namespace = std::make_shared<Namespace>("");
}

Program::~Program()
{

}
model::Kind Program::kind() const
{
  return ClassKind;
}

static std::shared_ptr<Entity> resolve_impl(const std::string& name, const std::shared_ptr<Entity>& context)
{
  if (!context)
    return nullptr;

  if (context->is<Namespace>())
  {
    const Namespace& ns = static_cast<const Namespace&>(*context);

    for (const auto& e : ns.entities)
    {
      if (e->name == name)
        return e;
    }

    return resolve_impl(name, context->parent());
  }
  else if (context->is<Class>())
  {
    const Class& cla = static_cast<const Class&>(*context);

    for (const auto& mem : cla.members)
    {
      if (mem->name == name)
        return mem;
    }

    return resolve_impl(name, context->parent());
  }

  return nullptr;
}

std::shared_ptr<Entity> Program::resolve(const Name& n)
{
  return resolve(n, globalNamespace());
}

std::shared_ptr<Entity> Program::resolve(const Name& n, const std::shared_ptr<Entity>& context)
{
  if (!context)
    return nullptr;

  return resolve_impl(n, context);
}

} // namespace dex
