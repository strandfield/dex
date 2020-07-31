// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODEL_H
#define DEX_MODEL_MODEL_H

#include "dex/model/class-documentation.h"
#include "dex/model/enum-documentation.h"
#include "dex/model/function-documentation.h"
#include "dex/model/namespace-documentation.h"
#include "dex/model/variable-documentation.h"

#include "dex/model/manual.h"
#include "dex/model/program.h"

#include <variant>

namespace dex
{

class DEX_MODEL_API Model
{
private:
  std::vector<std::shared_ptr<Manual>> m_manuals;
  std::shared_ptr<dex::Program> m_program;

public:
  Model() = default;

  bool empty() const;

  std::vector<std::shared_ptr<Manual>>& manuals();
  const std::vector<std::shared_ptr<Manual>>& manuals() const;

  std::shared_ptr<dex::Program> program() const;
  std::shared_ptr<dex::Program> getOrCreateProgram();  
  void setProgram(std::shared_ptr<dex::Program> prog);

  struct DEX_MODEL_API PathElement
  {
    std::string name;
    size_t index = std::numeric_limits<size_t>::max();

    PathElement(std::string n);
    PathElement(std::string n, size_t i);
  };

  typedef std::vector<PathElement> Path;

  typedef std::variant<std::shared_ptr<cxx::Program>, std::shared_ptr<cxx::Entity>, std::shared_ptr<cxx::Documentation>, std::shared_ptr<dom::Node>> Node;

  Node get(const Path& path) const;
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
