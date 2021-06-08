// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_PARAGRAPH_EXPORT_H
#define DEX_OUTPUT_PARAGRAPH_EXPORT_H

#include "dex/model/document.h"

namespace dex
{

class ParagraphConverter
{
public:
  const dex::Paragraph& paragraph;
  std::string result;

public:
  explicit ParagraphConverter(const dex::Paragraph& par);

  void process();

protected:
  void process(const dex::ParagraphIterator begin, const dex::ParagraphIterator end);
  void process_style(const dex::ParagraphIterator it, const std::string& style);
  virtual void process_text(const std::string& text);
  virtual void process_bold(const dex::ParagraphIterator it);
  virtual void process_italic(const dex::ParagraphIterator it);
  virtual void process_typewriter(const dex::ParagraphIterator it);
  virtual void process_link(const dex::ParagraphIterator it, const std::string& url);
  virtual void process_math(const dex::ParagraphIterator it);
  virtual void process_index(const dex::ParagraphIterator it, const std::string& key);
  void process(const dex::ParagraphIterator it);
};

} // namespace dex

#endif // DEX_OUTPUT_PARAGRAPH_EXPORT_H
