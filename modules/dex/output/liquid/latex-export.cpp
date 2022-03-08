// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid/latex-export.h"

#include "dex/output/paragraph-converter.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/since.h"

namespace dex
{

class LatexParagraphConverter : public ParagraphConverter
{
public:

  using ParagraphConverter::ParagraphConverter;

  void process_text(const std::string& text) override
  {
    result.reserve(result.size() + text.size());

    for (char c : text)
    {
      if (c == '\\')
        result += "\\textbackslash{}";
      else if (c == '{' || c == '}' || c == '#' || c == '%' || c == '_')
        result.push_back('\\'), result.push_back(c);
      else
        result.push_back(c);
    }
  }

  void process_bold(const dex::ParagraphIterator it) override
  {
    result += "\\textbf{";
    process(it);
    result += "}";
  }

  void process_italic(const dex::ParagraphIterator it) override
  {
    result += "\\textit{";
    process(it);
    result += "}";
  }

  void process_typewriter(const dex::ParagraphIterator it) override
  {
    result += "\\texttt{";
    process(it);
    result += "}";
  }

  void process_link(const dex::ParagraphIterator it, const std::string& url) override
  {
    result += "\\href{";
    result += url;
    result += "}{";
    process(it);
    result += "}";
  }

  void process_index(const dex::ParagraphIterator it, const std::string& key) override
  {
    result += "\\index{";
    result += key;
    result += "}";
  }
};

LatexStringifier::LatexStringifier(LiquidExporter& exp)
  : LiquidStringifier(exp)
{

}

std::string LatexStringifier::stringify_list(const dex::List& list) const
{
  std::string result = "\\begin{itemize}\n";

  for (const auto& li : list.items)
  {
    result += "  \\item " + stringify_listitem(static_cast<dex::ListItem&>(*li)) + "\n";
  }

  result += "\\end{itemize}";

  return result;
}

std::string LatexStringifier::stringify_listitem(const dex::ListItem& li) const
{
  return stringify_domcontent(li.content);
}

std::string LatexStringifier::stringify_paragraph(const dex::Paragraph& par) const
{
  LatexParagraphConverter converter{ par };
  converter.process();
  return std::string(std::move(converter.result));
}

std::string LatexStringifier::stringify_image(const dex::Image& img) const
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

std::string LatexStringifier::stringify_beginsince(const dex::BeginSince& bsince) const
{
  return "[Since:" + bsince.version + " --- ";
}

std::string LatexStringifier::stringify_endsince(const dex::EndSince& esince) const
{
  return "---endsince]\n";
}


std::string LatexStringifier::format_group_item(const std::shared_ptr<dex::Entity>& e) const
{
  std::string result = [&]() -> std::string {
    if (e->is<dex::Function>())
      return e->name + "()";
    else
      return e->name;
  }();

  return result;
}

std::string LatexStringifier::stringify_grouptable(const dex::GroupTable& table) const
{
  std::shared_ptr<Group> group = renderer.model()->groups.get(table.groupname);

  if (!group)
    return "";

  std::string result;

  result += "\\begin{itemize}\n";

  for (const auto& e : group->content.entities)
  {
    result += "  \\item " + format_group_item(e) + "\n";
  }

  result += "\\end{itemize}\n";

  return result;
}

std::string LatexStringifier::stringify_codeblock(const dex::CodeBlock& codeblock) const
{
  std::string result = "\\begin{lstlisting}\n";

  result += codeblock.code;

  if (result.back() != '\n')
    result.push_back('\n');

  result += "\\end{lstlisting}";

  return result;
}

std::string LatexStringifier::stringify_frontmatter(const dex::FrontMatter& fm) const
{
  return "\\frontmatter";
}

std::string LatexStringifier::stringify_mainmatter(const dex::MainMatter& mm) const
{
  return "\\mainmatter";
}

std::string LatexStringifier::stringify_backmatter(const dex::BackMatter& bm) const
{
  return "\\backmatter";
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

std::string LatexStringifier::stringify_tableofcontents(const dex::TableOfContents& toc) const
{
  return "\\tableofcontents";
}

std::string LatexStringifier::stringify_index(const dex::Index& /* idx */) const
{
  return "\\printindex";
}

} // namespace dex
