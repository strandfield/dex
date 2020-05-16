// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/paragraph-converter.h"

#include "dex/model/inline-math.h"
#include "dex/model/since.h"

#include <dom/paragraph/iterator.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

ParagraphConverter::ParagraphConverter(const dom::Paragraph& par)
  : paragraph(par)
{

}

void ParagraphConverter::process()
{
  process(paragraph.begin(), paragraph.end());
}

void ParagraphConverter::process(const dom::ParagraphIterator begin, const dom::ParagraphIterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    if (it.isText())
    {
      result += it.range().text();
    }
    else
    {
      if ((*it)->is<dom::TextStyle>())
      {
        process_style(it, std::static_pointer_cast<dom::TextStyle>(*it)->style());
      }
      else if ((*it)->is<dom::Link>())
      {
        process_link(it, std::static_pointer_cast<dom::Link>(*it)->url());
      }
      else if((*it)->type() == dex::InlineMath::TypeId)
      {
        process_math(it);
      }
      else
      {
        process(it);
      }
    }
  }
}

void ParagraphConverter::process_style(const dom::ParagraphIterator it, const std::string& style)
{
  if (style == "bold")
    process_bold(it);
  else if (style == "italic")
    process_italic(it);
  else if (style == "code")
    process_typewriter(it);
  else
    process(it);
}

void ParagraphConverter::process_bold(const dom::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_italic(const dom::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_typewriter(const dom::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_link(const dom::ParagraphIterator it, const std::string& /* url */)
{
  process(it);
}

void ParagraphConverter::process_math(const dom::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process(const dom::ParagraphIterator it)
{
  if (it.hasChild())
  {
    process(it.begin(), it.end());
  }
  else
  {
    result += it.range().text();
  }
}

} // namespace dex
