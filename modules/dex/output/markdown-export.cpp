// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/markdown-export.h"

#include "dex/output/liquid-exporter.h"
#include "dex/output/paragraph-converter.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/inline-math.h"
#include "dex/model/since.h"

#include "dex/common/logging.h"

namespace dex
{

class MarkdownParagraphConverter : public ParagraphConverter
{
public:

  using ParagraphConverter::ParagraphConverter;

  void process_bold(const dex::ParagraphIterator it) override
  {
    result += "**";
    process(it);
    result += "**";
  }

  void process_italic(const dex::ParagraphIterator it) override
  {
    result += "*";
    process(it);
    result += "*";
  }

  void process_typewriter(const dex::ParagraphIterator it) override
  {
    result += "`";
    process(it);
    result += "`";
  }

  void process_link(const dex::ParagraphIterator it, const std::string& url) override
  {
    result += "[";
    process(it);
    result += "](";
    result += url;
    result += ")";
  }

  void process_math(const dex::ParagraphIterator it) override
  {
    result += "`";
    auto text_begin = it.paragraph().text().begin();
    result.insert(result.end(), text_begin + it.range().begin() + 1, text_begin + it.range().end() - 1);
    result += "`";
  }
};

MarkdownStringifier::MarkdownStringifier(LiquidExporter& exp)
  : LiquidStringifier(exp)
{

}

std::string MarkdownStringifier::stringify_list(const dex::List& list) const
{
  // @TODO: handle nested list

  std::string result;

  for (const auto& li : list.items)
  {
    result += "- " + stringify_listitem(static_cast<dex::ListItem&>(*li)) + "\n";
  }

  return result;
}

std::string MarkdownStringifier::stringify_listitem(const dex::ListItem& li) const
{
  return stringify_domcontent(li.content);
}

std::string MarkdownStringifier::stringify_paragraph(const dex::Paragraph& par) const
{
  MarkdownParagraphConverter converter{ par };
  converter.process();
  return std::string(std::move(converter.result));
}

std::string MarkdownStringifier::stringify_image(const dex::Image& img) const
{
  return "![image](" + img.src + ")";
}

std::string MarkdownStringifier::stringify_beginsince(const dex::BeginSince& bsince) const
{
  return "[since:" + bsince.version + " --- ";
}

std::string MarkdownStringifier::stringify_endsince(const dex::EndSince& esince) const
{
  return " ---endsince]\n";
}

std::string MarkdownStringifier::stringify_math(const dex::DisplayMath& math) const
{
  std::string result = "```tex\n";
  result += math.source;
  result += "\n```\n";
  return result;
}

std::string MarkdownStringifier::format_group_item(const std::shared_ptr<dex::Entity>& e) const
{
  const std::string url = [&]() -> std::string {
    json::Object json_obj = renderer.modelMapping().get(*e).toObject();
    auto it = json_obj.data().find("url");

    if (it != json_obj.data().end())
      return json_obj["url"].toString();
    else
      return "";
  }();

  std::string result = [&]() -> std::string {
    if (e->is<dex::Function>())
      return e->name + "()";
    else
      return e->name;
  }();

  if (!url.empty())
    return "- [" + result + "](" + url + ")";
  else
    return "- " + result;
}

std::string MarkdownStringifier::stringify_grouptable(const dex::GroupTable& table) const
{
  std::shared_ptr<Group> group = renderer.model()->groups.get(table.groupname);

  if (!group)
    return "";

  std::string result;

  for (const auto& e : group->content.entities)
  {
    result += format_group_item(e) + "\n";
  }

  return result;
}

std::string MarkdownStringifier::stringify_codeblock(const dex::CodeBlock& codeblock) const
{
  std::string result = "```";

  if (!codeblock.lang.empty())
    result += codeblock.lang;

  result += "\n";
  result += codeblock.code;
  
  if (result.back() != '\n')
    result.push_back('\n');

  result += "```";
  return result;
}

std::string MarkdownStringifier::stringify_frontmatter(const dex::FrontMatter& fm) const
{
  LOG_WARNING << "\\frontmatter not supported in Markdown output";
  return std::string();
}

std::string MarkdownStringifier::stringify_mainmatter(const dex::MainMatter& mm) const
{
  LOG_WARNING << "\\mainmatter not supported in Markdown output";
  return std::string();
}

std::string MarkdownStringifier::stringify_backmatter(const dex::BackMatter& bm) const
{
  LOG_WARNING << "\\backmatter not supported in Markdown output";
  return std::string();
}

std::string MarkdownStringifier::stringify_section(const dex::Sectioning& sec) const
{
  std::string result;

  for (int i(0); i < (sec.depth - dex::Sectioning::Part) + 1; ++i)
    result.push_back('#');

  result.push_back(' ');

  result += sec.name + "\n\n";

  for (const auto& c : sec.content)
  {
    result += stringify_domnode(*c) + "\n\n";
  }

  return result;
}

std::string MarkdownStringifier::stringify_tableofcontents(const dex::TableOfContents& toc) const
{
  LOG_WARNING << "Table of contents not supported in Markdown output";
  return std::string();
}

std::string MarkdownStringifier::stringify_index(const dex::Index& idx) const
{
  LOG_WARNING << "Index not supported in Markdown output";
  return std::string();
}

} // namespace dex
