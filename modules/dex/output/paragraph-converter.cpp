// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/paragraph-converter.h"

#include "dex/model/inline-math.h"
#include "dex/model/paragraph-annotations.h"

namespace dex
{

ParagraphConverter::ParagraphConverter(const dex::Paragraph& par)
  : paragraph(par)
{

}

void ParagraphConverter::process()
{
  process(paragraph.begin(), paragraph.end());
}

void ParagraphConverter::process(const dex::ParagraphIterator begin, const dex::ParagraphIterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    if (it.isText())
    {
      result += it.range().text();
    }
    else
    {
      if ((*it)->is<dex::TextStyle>())
      {
        process_style(it, std::static_pointer_cast<dex::TextStyle>(*it)->style());
      }
      else if ((*it)->is<dex::Link>())
      {
        process_link(it, std::static_pointer_cast<dex::Link>(*it)->url());
      }
      else if((*it)->is<dex::InlineMath>())
      {
        process_math(it);
      }
      else if ((*it)->is<dex::ParIndexEntry>())
      {
        std::shared_ptr<dex::ParagraphMetaData> metad = *it;
        process_index(it, static_cast<const dex::GenericParagraphMetaData<dex::ParIndexEntry>&>(*metad).value().key);
      }
      else
      {
        process(it);
      }
    }
  }
}

void ParagraphConverter::process_style(const dex::ParagraphIterator it, const std::string& style)
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

void ParagraphConverter::process_bold(const dex::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_italic(const dex::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_typewriter(const dex::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_link(const dex::ParagraphIterator it, const std::string& /* url */)
{
  process(it);
}

void ParagraphConverter::process_math(const dex::ParagraphIterator it)
{
  process(it);
}

void ParagraphConverter::process_index(const dex::ParagraphIterator it, const std::string& /* key */)
{
  process(it);
}

void ParagraphConverter::process(const dex::ParagraphIterator it)
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
