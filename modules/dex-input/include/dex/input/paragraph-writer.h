// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARAGRAPHWRITER_H
#define DEX_INPUT_PARAGRAPHWRITER_H

#include "dex/dex-input.h"

#include "dex/common/state.h"

#include "dex/input/dom-writer.h"

#include <dom/paragraph.h>

namespace tex
{
namespace parsing
{
class MathParserFrontend;
} // namespace parsing
} // namespace tex

namespace dex
{

class DEX_INPUT_API ParagraphWriter : public DomWriter
{
public:
  ParagraphWriter();
  ~ParagraphWriter();

  void write(char c) override;
  void write(const std::string& str);

  void writeCs(const std::string& str);

  void mathshift();
  void alignmenttab();
  void superscript();
  void subscript();

  void begintextbf();
  void endtextbf();

  void begintextit();
  void endtextit();

  void begintexttt();
  void endtexttt();

  void writeLink(std::string url, const std::string& text);
  void writeStyledText(std::string style, const std::string& text);

  void writeSince(const std::string& version, const std::string& text);

  void finish() override;

  std::shared_ptr<dom::Paragraph> output() const;

protected:
  void beginStyledText(std::string style);
  void endStyledText(const char* style);

private:
  std::vector<std::shared_ptr<dom::ParagraphMetaData>> m_pending_metadata;
  std::unique_ptr<tex::parsing::MathParserFrontend> m_math_parser;
};

} // namespace dex

#endif // DEX_INPUT_PARAGRAPHWRITER_H
