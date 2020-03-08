// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODEL_H
#define DEX_MODEL_MODEL_H

#include "dex/model/class-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"

#include "dex/model/manual.h"

#include <cxx/program.h>

namespace dex
{

class DEX_MODEL_API Model
{
private:
  std::vector<std::shared_ptr<Manual>> m_manuals;
  std::shared_ptr<cxx::Program> m_program;

public:
  Model() = default;

  std::vector<std::shared_ptr<Manual>>& manuals();
  const std::vector<std::shared_ptr<Manual>>& manuals() const;

  std::shared_ptr<cxx::Program> program() const;
  std::shared_ptr<cxx::Program> getOrCreateProgram();  
};

} // namespace dex

namespace dex
{

inline std::vector<std::shared_ptr<Manual>>& Model::manuals()
{
  return m_manuals;
}

inline const std::vector<std::shared_ptr<Manual>>& Model::manuals() const
{
  return m_manuals;
}

inline std::shared_ptr<cxx::Program> Model::program() const
{
  return m_program;
}

inline std::shared_ptr<cxx::Program> Model::getOrCreateProgram()
{
  if (!m_program)
    m_program = std::make_shared<cxx::Program>();

  return m_program;
}

} // namespace dex

#endif // DEX_MODEL_MODEL_H
