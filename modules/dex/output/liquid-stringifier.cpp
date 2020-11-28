// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-stringifier.h"

#include "dex/output/liquid-exporter.h"

#include "dex/model/code-block.h"
#include "dex/model/display-math.h"
#include "dex/model/model.h"

#include <cxx/entity.h>

#include <dom/image.h>
#include <dom/list.h>

namespace dex
{

LiquidStringifier::LiquidStringifier(LiquidExporter& exp)
  : renderer(exp)
{

}

LiquidStringifier::~LiquidStringifier()
{

}

std::string LiquidStringifier::stringify(const json::Json& val) const
{
  if (!val.isObject() && !val.isArray())
    return liquid::Renderer::defaultStringify(val);
  else if (val.isArray())
    return stringify_array(val.toArray());

  json::Object obj = val.toObject();

  auto path_it = obj.data().find("_path");

  if (path_it != obj.data().end())
  {
    Model::Path path = Model::parse_path(path_it->second.toString());
    Model::Node model_node = renderer.model()->get(path);

    if (std::holds_alternative<std::shared_ptr<dom::Node>>(model_node))
    {
      auto dom_node = std::get< std::shared_ptr<dom::Node>>(model_node);
      return stringify_domnode(*dom_node);
    }
  }
  else
  {
    assert(("element has no path", false));
    return {};
  }

  assert(("Not implemented", false));
  return {};
}

std::string LiquidStringifier::stringify_domnode(const dom::Node& node) const
{
  if (node.is<dom::Paragraph>())
    return stringify_paragraph(static_cast<const dom::Paragraph&>(node));
  else if (node.is<dom::List>())
    return stringify_list(static_cast<const dom::List&>(node));
  else if (node.is<dom::ListItem>())
    return stringify_listitem(static_cast<const dom::ListItem&>(node));
  else if (node.is<dom::Image>())
    return stringify_image(static_cast<const dom::Image&>(node));
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

  assert(("dom element not implemented", false));
  return {};
}

std::string LiquidStringifier::stringify_domcontent(const dom::NodeList& content) const
{
  std::string result;

  for (const auto& node : content)
  {
    result += stringify_domnode(*node);
  }

  return result;
}

std::string LiquidStringifier::stringify_array(const json::Array& list) const
{
  std::string result;

  for (const auto& val : list.data())
  {
    result += stringify(val);
    result += "\n\n";
  }

  return result;
}

} // namespace dex
