// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_FUNCTION_DOCUMENTATION_H
#define DEX_MODEL_FUNCTION_DOCUMENTATION_H

#include "dex/model/entity-documentation.h"

#include <vector>
#include <optional>

namespace dex
{

class DEX_MODEL_API FunctionParameterDocumentation : public cxx::Documentation
{
public:
  std::string brief;

public:

  explicit FunctionParameterDocumentation(std::string str);
};

class DEX_MODEL_API FunctionDocumentation : public EntityDocumentation
{
private:
  std::optional<std::string> m_returnvalue;

public:

  static const std::string TypeId;
  const std::string& className() const override;

  const std::optional<std::string>& returnValue() const;
  std::optional<std::string>& returnValue();
};

} // namespace dex

namespace dex
{

inline const std::optional<std::string>& FunctionDocumentation::returnValue() const
{
  return m_returnvalue;
}

inline std::optional<std::string>& FunctionDocumentation::returnValue()
{
  return m_returnvalue;
}

} // namespace dex

#endif // DEX_MODEL_FUNCTION_DOCUMENTATION_H
