// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_STRINGIFIER_H
#define DEX_OUTPUT_LIQUID_STRINGIFIER_H

#include "dex/dex-output.h"

#include <dom/node.h>

#include <json-toolkit/json.h>

namespace dom
{
class Image;
class List;
class ListItem;
class Paragraph;
} // namespace dom

namespace dex
{

class BackMatter;
class BeginSince;
class CodeBlock;
class DisplayMath;
class EndSince;
class FrontMatter;
class GroupTable;
class Index;
class MainMatter;
class Sectioning;
class TableOfContents;

class LiquidExporter;

class DEX_OUTPUT_API LiquidStringifier
{
public:
  LiquidExporter& renderer;

public:
  explicit LiquidStringifier(LiquidExporter& exp);
  virtual ~LiquidStringifier();

  std::string stringify(const json::Json& val) const;

protected:
  virtual std::string stringify_domnode(const dom::Node& node) const;
  virtual std::string stringify_domcontent(const dom::NodeList& content) const;

  virtual std::string stringify_array(const json::Array& list) const;
  virtual std::string stringify_list(const dom::List& list) const = 0;
  virtual std::string stringify_listitem(const dom::ListItem& li) const = 0;
  virtual std::string stringify_paragraph(const dom::Paragraph& par) const = 0;
  virtual std::string stringify_beginsince(const dex::BeginSince& bsince) const = 0;
  virtual std::string stringify_endsince(const dex::EndSince& esince) const = 0;
  virtual std::string stringify_image(const dom::Image& img) const = 0;
  virtual std::string stringify_math(const dex::DisplayMath& math) const = 0;
  virtual std::string stringify_grouptable(const dex::GroupTable& table) const = 0;
  virtual std::string stringify_codeblock(const dex::CodeBlock& codeblock) const = 0;

  virtual std::string stringify_frontmatter(const dex::FrontMatter& fm) const = 0;
  virtual std::string stringify_mainmatter(const dex::MainMatter& mm) const = 0;
  virtual std::string stringify_backmatter(const dex::BackMatter& bm) const = 0;
  virtual std::string stringify_section(const dex::Sectioning& sec) const = 0;
  virtual std::string stringify_tableofcontents(const dex::TableOfContents& toc) const = 0;
  virtual std::string stringify_index(const dex::Index& idx) const = 0;
};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_STRINGIFIER_H
