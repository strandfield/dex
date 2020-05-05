// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/paragraph-writer.h"

#include "dex/model/since.h"

#include "dex/input/parser-errors.h"

#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

ParagraphWriter::ParagraphWriter()
{
  setOutput(std::make_shared<dom::Paragraph>());
}

void ParagraphWriter::write(char c)
{
  output()->addChar(c);
}

void ParagraphWriter::write(const std::string& str)
{
  output()->addText(str);
}

void ParagraphWriter::begintextbf()
{
  dom::Paragraph& par = *output();
  size_t start = par.length();

  auto style = std::make_shared<dom::TextStyle>(dom::ParagraphRange(par, start, par.length()), "bold");
  m_pending_metadata.push_back(style);
}

void ParagraphWriter::endtextbf()
{
  auto style = m_pending_metadata.back();

  if (!style->is<dom::TextStyle>() || static_cast<dom::TextStyle*>(style.get())->style() != "bold")
    throw std::runtime_error{ "ParagraphWriter::endtextbf() mismatch" };

  m_pending_metadata.pop_back();

  dom::Paragraph& par = *output();
  size_t end = par.length();
  style->range() = dom::ParagraphRange(par, style->range().begin(), end);

  par.addMetaData(style);
}

void ParagraphWriter::writeLink(std::string url, const std::string& text)
{
  dom::Paragraph& par = *output();
  size_t start = par.length();
  par.addText(text);

  auto link = std::make_shared<dom::Link>(dom::ParagraphRange(par, start, par.length()), std::move(url));
  par.addMetaData(link);
}

void ParagraphWriter::writeStyledText(std::string style_name, const std::string& text)
{
  dom::Paragraph& par = *output();
  size_t start = par.length();
  par.addText(text);

  auto style = std::make_shared<dom::TextStyle>(dom::ParagraphRange(par, start, par.length()), std::move(style_name));
  par.addMetaData(style);
}

void ParagraphWriter::writeSince(const std::string& version, const std::string& text)
{
  dom::Paragraph& par = *output();
  size_t start = par.length();
  par.addText(text);

  par.add<dex::Since>(dom::ParagraphRange(par, start, par.length()), version);
}

void ParagraphWriter::finish()
{
  dom::Paragraph& par = *output();
  
  // Removing trailing space, if any
  if (par.length() > 0 && par.text().back() == ' ')
  {
    par.text().pop_back();

    for (auto data : par.metadata())
    {
      if (data->range().end() > par.length())
      {
        const dom::ParagraphRange parrange = data->range();
        data->range() = dom::ParagraphRange(parrange.paragraph(), parrange.begin(), par.length());
      }
    }
  }
}

std::shared_ptr<dom::Paragraph> ParagraphWriter::output() const
{
  return std::static_pointer_cast<dom::Paragraph>(ContentWriter::output());
}

} // namespace dex
