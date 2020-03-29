// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/parser-mode.h"

#include "dex/input/parser-machine.h"

#include <cassert>
#include <stdexcept>

namespace dex
{

ParserMode::ParserMode(ParserMachine& machine, ParserMode* parent)
  : m_machine{machine},
    m_parent{ parent },
    m_done{ false }
{

}

ParserMachine& ParserMode::machine() const
{
  return m_machine;
}

ParserMode* ParserMode::parent() const
{
  return m_parent;
}

bool ParserMode::done() const
{
  return m_done;
}

void ParserMode::childFinished(ParserMode& mode)
{
  assert(("invalid call to ParserMode::childFinished()", false));
}

const std::shared_ptr<dom::Node>& ParserMode::output() const
{
  static std::shared_ptr<dom::Node> static_instance = nullptr;
  return static_instance;
}

void ParserMode::setDone(bool done)
{
  m_done = done;
}

} // namespace dex
