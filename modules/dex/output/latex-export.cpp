// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/latex-export.h"

#include "dex/output/paragraph-converter.h"

#include "dex/model/display-math.h"
#include "dex/model/since.h"

#include "dex/common/file-utils.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph.h>
#include <dom/paragraph/iterator.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

class LatexParagraphConverter : public ParagraphConverter
{
public:

  using ParagraphConverter::ParagraphConverter;

  void process_bold(const dom::ParagraphIterator it) override
  {
    result += "\\textbf{";
    process(it);
    result += "}";
  }

  void process_italic(const dom::ParagraphIterator it) override
  {
    result += "\\textit{";
    process(it);
    result += "}";
  }

  void process_typewriter(const dom::ParagraphIterator it) override
  {
    result += "\\texttt{";
    process(it);
    result += "}";
  }

  void process_link(const dom::ParagraphIterator it, const std::string& url) override
  {
    result += "\\href{";
    result += url;
    result += "}{";
    process(it);
    result += "}";
  }
};

LatexStringifier::LatexStringifier(LiquidExporter& exp)
  : LiquidStringifier(exp)
{

}

std::string LatexStringifier::stringify_list(const dom::List& list) const
{
  std::string result = "\\begin{itemize}\n";

  for (const auto& li : list.items)
  {
    result += "  \\item " + stringify_listitem(*li) + "\n";
  }

  result += "\\end{itemize}";

  return result;
}

std::string LatexStringifier::stringify_listitem(const dom::ListItem& li) const
{
  return stringify_domcontent(li.content);
}

std::string LatexStringifier::stringify_paragraph(const dom::Paragraph& par) const
{
  LatexParagraphConverter converter{ par };
  converter.process();
  return std::string(std::move(converter.result));
}

std::string LatexStringifier::stringify_image(const dom::Image& img) const
{
  return "\\includegraphics{" + img.src + "}";
}

std::string LatexStringifier::stringify_math(const dex::DisplayMath& math) const
{
  std::string result = "\\[";
  result += math.source;
  result += "\\]";
  return result;
}

std::string LatexStringifier::stringify_section(const dex::Sectioning& sec) const
{
  std::string result;

  switch (sec.depth)
  {
  case dex::Sectioning::Part:
    result += "\\part{";
    break;
  case dex::Sectioning::Chapter:
    result += "\\chapter{";
    break;
  case dex::Sectioning::Section:
    result += "\\section{";
    break;
  default:
    result += "{";
    break;
  }

  result += sec.name + "}\n\n";

  for (const auto& c : sec.content)
  {
    result += stringify_domnode(*c) + "\n\n";
  }

  return result;
}

} // namespace dex
