// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_PROGRAM_H
#define CXXAST_PROGRAM_H

#include "cxx/cxxast-defs.h"

#include <dom/node.h>

#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace cxx
{

class Entity;
class File;
class Namespace;
class Type;

class CXXAST_API Program : public dom::Node
{
public:
  Program();
  ~Program();

  static const std::string TypeId;
  const std::string& type() const override;

  const std::vector<std::shared_ptr<File>>& files() const;

  const std::shared_ptr<Namespace>& globalNamespace() const;
  
  struct Context
  {
    std::shared_ptr<Entity> scope = nullptr;

    bool operator<(const Context& other) const
    {
      return scope < other.scope;
    }
  };

  typedef std::pair<std::string, Context> ContextualName;

  std::shared_ptr<Type> getType(const std::string& name, Context context);
  std::shared_ptr<Type> getType(const std::string& name, Context context) const;

  std::vector<std::shared_ptr<Type>> unresolvedTypes() const;

private:
  std::vector<std::shared_ptr<File>> m_files;
  std::shared_ptr<Namespace> m_global_namespace;
  std::map<ContextualName, std::shared_ptr<Type>> m_types;
};

} // namespace cxx

namespace cxx
{

inline const std::vector<std::shared_ptr<File>>& Program::files() const
{
  return m_files;
}

inline const std::shared_ptr<Namespace>& Program::globalNamespace() const
{
  return m_global_namespace;
}

} // namespace cxx

#endif // CXXAST_PROGRAM_H
