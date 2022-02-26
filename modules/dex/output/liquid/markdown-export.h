// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_MARKDOWN_EXPORT_H
#define DEX_OUTPUT_MARKDOWN_EXPORT_H

#include "dex/output/liquid/liquid-stringifier.h"

namespace dex
{

class Entity;

class MarkdownStringifier : public LiquidStringifier
{
public:
  bool just_the_docs = false;

public:
  explicit MarkdownStringifier(LiquidExporter& exp);

protected:
  void selected() override;

  std::string stringify_list(const dex::List& list) const override;
  std::string stringify_listitem(const dex::ListItem& li) const override;
  std::string stringify_paragraph(const dex::Paragraph& par) const override;
  std::string stringify_image(const dex::Image& img) const override;
  std::string stringify_beginsince(const dex::BeginSince& bsince) const override;
  std::string stringify_endsince(const dex::EndSince& esince) const override;
  std::string stringify_math(const dex::DisplayMath& math) const override;

  std::string format_group_item(const std::shared_ptr<dex::Entity>& e) const;
  std::string stringify_grouptable(const dex::GroupTable& table) const override;

  std::string stringify_codeblock(const dex::CodeBlock& codeblock) const override;

  std::string stringify_frontmatter(const dex::FrontMatter& fm) const override;
  std::string stringify_mainmatter(const dex::MainMatter& mm) const override;
  std::string stringify_backmatter(const dex::BackMatter& bm) const override;
  std::string stringify_section(const dex::Sectioning& sec) const override;
  std::string stringify_tableofcontents(const dex::TableOfContents& toc) const override;
  std::string stringify_index(const dex::Index& idx) const override;
};

} // namespace dex

#endif // DEX_OUTPUT_MARKDOWN_EXPORT_H
