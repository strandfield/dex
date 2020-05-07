// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_MATHWRITER_H
#define DEX_INPUT_MATHWRITER_H

#include "dex/input/content-writer.h"

#include "dex/model/display-math.h"

namespace tex
{
namespace parsing
{
class MathParserFrontend;
} // namespace parsing
} // namespace tex

namespace dex
{

class DEX_INPUT_API MathWriter : public ContentWriter
{
public:
  MathWriter();
  ~MathWriter();

  void write(char c) override;
  void finish() override;

  void writeControlSequence(const std::string& csname);

  void superscript();
  void subscript();

  void beginMathList();
  void endMathList();

  void alignmentTab();

  tex::parsing::MathParserFrontend& mathparser();

  std::shared_ptr<dex::DisplayMath> output() const;
  
private:
  std::unique_ptr<tex::parsing::MathParserFrontend> m_math_parser;
  std::shared_ptr<dex::DisplayMath> m_output;
};

} // namespace dex

#endif // DEX_INPUT_MATHWRITER_H
