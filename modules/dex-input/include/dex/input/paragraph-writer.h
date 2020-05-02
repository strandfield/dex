// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARAGRAPHWRITER_H
#define DEX_INPUT_PARAGRAPHWRITER_H

#include "dex/dex-input.h"

#include "dex/common/state.h"

#include "dex/input/content-writer.h"

#include <dom/paragraph.h>

namespace dex
{

class DEX_INPUT_API ParagraphWriter : public ContentWriter
{
public:
  ParagraphWriter();

  void write(char c) override;
  void write(const std::string& str);

  void writeLink(std::string url, const std::string& text);
  void writeStyledText(std::string style, const std::string& text);

  void writeSince(const std::string& version, const std::string& text);

  void finish() override;

  std::shared_ptr<dom::Paragraph> output() const;
};

} // namespace dex

#endif // DEX_INPUT_PARAGRAPHWRITER_H
