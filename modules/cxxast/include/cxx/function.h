// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'cxxast' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef CXXAST_FUNCTION_H
#define CXXAST_FUNCTION_H

#include "cxx/entity.h"

#include <vector>

namespace cxx
{

class FunctionBody;
class Type;

class CXXAST_API Function : public Entity
{
public:
  ~Function() = default;

  explicit Function(std::string name, std::shared_ptr<Entity> parent = nullptr);

  static const std::string TypeId;
  const std::string& type() const override;

  struct Parameter
  {
    std::shared_ptr<Type> type;
    std::string name;
  };

  const std::shared_ptr<Type>& returnType() const;
  const std::vector<Parameter>& parameters() const;
  const std::shared_ptr<FunctionBody>& body() const;

private:
  std::shared_ptr<Type> m_rtype;
  std::vector<Parameter> m_params;
  std::shared_ptr<FunctionBody> m_body;
};

} // namespace cxx

namespace cxx
{

inline Function::Function(std::string name, std::shared_ptr<Entity> parent)
  : Entity{std::move(name), std::move(parent)}
{

}

inline const std::shared_ptr<Type>& Function::returnType() const
{
  return m_rtype;
}

inline const std::vector<Function::Parameter>& Function::parameters() const
{
  return m_params;
}

inline const std::shared_ptr<FunctionBody>& Function::body() const
{
  return m_body;
}

} // namespace cxx

#endif // CXXAST_FUNCTION_H
