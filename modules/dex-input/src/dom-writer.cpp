// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/dom-writer.h"

#include "dex/input/parser-errors.h"

#include <cassert>
#include <stdexcept>

namespace dex
{

DomWriter::DomWriter()
{

}

DomWriter::~DomWriter()
{

}

std::shared_ptr<dom::Node> DomWriter::output() const
{
  return m_output;
}

void DomWriter::setOutput(std::shared_ptr<dom::Node> out)
{
  m_output = out;
}

} // namespace dex
