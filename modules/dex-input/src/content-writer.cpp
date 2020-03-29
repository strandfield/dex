// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/content-writer.h"

#include "dex/input/parser-errors.h"

#include <cassert>
#include <stdexcept>

namespace dex
{

ContentWriter::ContentWriter()
{

}

ContentWriter::~ContentWriter()
{

}

void ContentWriter::control(const std::string& cs)
{
  throw BadControlSequence{ cs };
}

void ContentWriter::handle(const FunctionCall& call)
{
  throw BadControlSequence{ call.function };
}

std::shared_ptr<dom::Node> ContentWriter::output() const
{
  return m_output;
}

void ContentWriter::setOutput(std::shared_ptr<dom::Node> out)
{
  m_output = out;
}

} // namespace dex
