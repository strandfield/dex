// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "cxx/program.h"

#include "cxx/namespace.h"
#include "cxx/type.h"

namespace cxx
{

const std::string Program::TypeId = "program";

const std::string& Program::type() const
{
  return TypeId;
}

Program::Program()
{
  m_global_namespace = std::make_shared<Namespace>("");
}

Program::~Program()
{

}

std::shared_ptr<Type> Program::getType(const std::string& name, Context context)
{
  if (context.scope == nullptr)
    context.scope = m_global_namespace;

  ContextualName name_context{ name, context };

  auto it = m_types.find(name_context);

  if (it != m_types.end())
  {
    return it->second;
  }

  std::shared_ptr<Type>& result = m_types[name_context];
  result = std::make_shared<Type>(name);
  return result;
}

std::shared_ptr<Type> Program::getType(const std::string& name, Context context) const
{
  if (context.scope == nullptr)
    context.scope = m_global_namespace;

  ContextualName name_context{ name, context };

  auto it = m_types.find(name_context);

  if (it != m_types.end())
  {
    return it->second;
  }
  else
  {
    return nullptr;
  }
}

std::vector<std::shared_ptr<Type>> Program::unresolvedTypes() const
{
  std::vector<std::shared_ptr<Type>> result;

  for (const auto& elem : m_types)
  {
    if (elem.second == nullptr)
      result.push_back(elem.second);
  }

  return result;
}

} // namespace cxx
