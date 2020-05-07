// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_PARAGRAPH_EXPORT_H
#define DEX_OUTPUT_PARAGRAPH_EXPORT_H

#include <dom/paragraph.h>

namespace dex
{

class ParagraphConverter
{
public:
  const dom::Paragraph& paragraph;
  std::string result;

public:
  explicit ParagraphConverter(const dom::Paragraph& par);

  void process();

protected:
  void process(const dom::ParagraphIterator begin, const dom::ParagraphIterator end);
  void process_style(const dom::ParagraphIterator it, const std::string& style);
  virtual void process_bold(const dom::ParagraphIterator it);
  virtual void process_italic(const dom::ParagraphIterator it);
  virtual void process_typewriter(const dom::ParagraphIterator it);
  virtual void process_math(const dom::ParagraphIterator it);
  void process(const dom::ParagraphIterator it);
};

} // namespace dex

#endif // DEX_OUTPUT_PARAGRAPH_EXPORT_H
