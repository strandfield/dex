// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_STRINGIFIER_H
#define DEX_OUTPUT_LIQUID_STRINGIFIER_H

#include "dex/dex-output.h"

#include "dex/model/document.h"

#include "dex/output/liquid-wrapper.h"

#include <json-toolkit/json.h>

namespace dex
{

class Image;
class List;
class ListItem;
class Paragraph;
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

  std::string stringify(const liquid::Value& val) const;

protected:
  virtual std::string stringify_domnode(const dex::DocumentNode& node) const;
  virtual std::string stringify_domcontent(const dex::DomNodeList& content) const;

  virtual std::string stringify_array(const liquid::Array& list) const;
  virtual std::string stringify_list(const dex::List& list) const = 0;
  virtual std::string stringify_listitem(const dex::ListItem& li) const = 0;
  virtual std::string stringify_paragraph(const dex::Paragraph& par) const = 0;
  virtual std::string stringify_beginsince(const dex::BeginSince& bsince) const = 0;
  virtual std::string stringify_endsince(const dex::EndSince& esince) const = 0;
  virtual std::string stringify_image(const dex::Image& img) const = 0;
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
