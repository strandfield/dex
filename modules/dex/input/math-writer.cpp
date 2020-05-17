// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/math-writer.h"

#include "dex/input/parser-errors.h"

#include <tex/parsing/mathparserfrontend.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

MathWriter::MathWriter()
{
  m_math_parser.reset(new tex::parsing::MathParserFrontend);
  m_output = std::make_shared<dex::DisplayMath>();
}

MathWriter::~MathWriter()
{

}

void MathWriter::write(char c)
{
  m_math_parser->writeChar(c);
  m_output->source.push_back(c);
}

void MathWriter::finish()
{
  m_math_parser->finish();
  m_output->mlist = std::move(m_math_parser->output());
  m_output->normalize();
}

void MathWriter::writeControlSequence(const std::string& csname)
{
  m_math_parser->writeControlSequence(csname);

  m_output->source.push_back('\\');
  m_output->source += csname;
  m_output->source.push_back(' ');
}

void MathWriter::superscript()
{
  m_math_parser->beginSuperscript();
  m_output->source.push_back('^');
}

void MathWriter::subscript()
{
  m_math_parser->beginSubscript();
  m_output->source.push_back('_');
}

void MathWriter::beginMathList()
{
  m_math_parser->beginMathList();
  m_output->source.push_back('{');
}

void MathWriter::endMathList()
{
  m_math_parser->endMathList();
  m_output->source.push_back('}');
}

void MathWriter::alignmentTab()
{
  m_math_parser->alignmentTab();
  m_output->source.push_back('&');
}

tex::parsing::MathParserFrontend& MathWriter::mathparser()
{
  return *m_math_parser;
}

std::shared_ptr<dex::DisplayMath> MathWriter::output() const
{
  return m_output;
}

} // namespace dex
