// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODEL_H
#define DEX_MODEL_MODEL_H

#include "dex/model/model-base.h"

#include "dex/model/class-documentation.h"
#include "dex/model/enum-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"
#include "dex/model/variable-documentation.h"

#include "dex/model/group.h"
#include "dex/model/manual.h"
#include "dex/model/program.h"

namespace dex
{

class DEX_MODEL_API Model : public model::Object
{
public:
  std::vector<std::shared_ptr<dex::Document>> documents;
  std::shared_ptr<dex::Program> m_program;
  GroupManager groups;

public:
  Model() = default;

  static constexpr model::Kind ClassKind = model::Kind::Model;
  model::Kind kind() const override;

  bool empty() const;

  std::shared_ptr<dex::Program> program() const;
  std::shared_ptr<dex::Program> getOrCreateProgram();  
  void setProgram(std::shared_ptr<dex::Program> prog);
};

} // namespace dex

namespace dex
{

inline std::shared_ptr<dex::Program> Model::program() const
{
  return m_program;
}

inline std::shared_ptr<dex::Program> Model::getOrCreateProgram()
{
  if (!m_program)
    m_program = std::make_shared<dex::Program>();

  return m_program;
}

inline void Model::setProgram(std::shared_ptr<dex::Program> prog)
{
  m_program = prog;
}

} // namespace dex

#endif // DEX_MODEL_MODEL_H
