// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-stringifier.h"

#include "dex/output/liquid-exporter.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/model.h"

namespace dex
{

LiquidStringifier::LiquidStringifier(LiquidExporter& exp)
  : renderer(exp)
{

}

LiquidStringifier::~LiquidStringifier()
{

}

std::string LiquidStringifier::stringify(const liquid::Value& val) const
{
  if (!val.isMap() && !val.isArray())
    return liquid::Renderer::defaultStringify(val);
  else if (val.isArray())
    return stringify_array(val.toArray());

  auto dom_node = liquid_cast<dex::DocumentNode>(val);

  if (dom_node)
    return stringify_domnode(*dom_node);

  assert(("Not implemented", false));
  return {};
}

std::string LiquidStringifier::stringify_domnode(const dex::DocumentNode& node) const
{
  if (node.is<dex::Paragraph>())
    return stringify_paragraph(static_cast<const dex::Paragraph&>(node));
  else if (node.is<dex::List>())
    return stringify_list(static_cast<const dex::List&>(node));
  else if (node.is<dex::ListItem>())
    return stringify_listitem(static_cast<const dex::ListItem&>(node));
  else if (node.is<dex::Image>())
    return stringify_image(static_cast<const dex::Image&>(node));
  else if (node.is<dex::BeginSince>())
    return stringify_beginsince(static_cast<const dex::BeginSince&>(node));
  else if (node.is<dex::EndSince>())
    return stringify_endsince(static_cast<const dex::EndSince&>(node));
  else if (node.is<dex::Sectioning>())
    return stringify_section(static_cast<const dex::Sectioning&>(node));
  else if (node.is<dex::DisplayMath>())
    return stringify_math(static_cast<const dex::DisplayMath&>(node));
  else if (node.is<dex::GroupTable>())
    return stringify_grouptable(static_cast<const dex::GroupTable&>(node));
  else if (node.is<dex::CodeBlock>())
    return stringify_codeblock(static_cast<const dex::CodeBlock&>(node));  
  else if (node.is<dex::TableOfContents>())
    return stringify_tableofcontents(static_cast<const dex::TableOfContents&>(node));
  else if (node.is<dex::Index>())
    return stringify_index(static_cast<const dex::Index&>(node));
  else if (node.is<dex::FrontMatter>())
    return stringify_frontmatter(static_cast<const dex::FrontMatter&>(node));
  else if (node.is<dex::MainMatter>())
    return stringify_mainmatter(static_cast<const dex::MainMatter&>(node));
  else if (node.is<dex::BackMatter>())
    return stringify_backmatter(static_cast<const dex::BackMatter&>(node));

  assert(("dom element not implemented", false));
  return {};
}

std::string LiquidStringifier::stringify_domcontent(const dex::DomNodeList& content) const
{
  std::string result;

  for (const auto& node : content)
  {
    result += stringify_domnode(*node);
  }

  return result;
}

std::string LiquidStringifier::stringify_array(const liquid::Array& list) const
{
  std::string result;

  for (size_t i(0); i < list.length(); ++i)
  {
    liquid::Value val = list.at(i);
    result += stringify(val);
    result += "\n\n";
  }

  return result;
}

} // namespace dex
