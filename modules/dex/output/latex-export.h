// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LATEX_EXPORT_H
#define DEX_OUTPUT_LATEX_EXPORT_H

#include "dex/output/liquid-exporter.h"
#include "dex/output/liquid-stringifier.h"

namespace cxx
{
class Entity;
} // namespace cxx

namespace dex
{

class LatexStringifier : public LiquidStringifier
{
public:
  explicit LatexStringifier(LiquidExporter& exp);

protected:
  std::string stringify_list(const dom::List& list) const override;
  std::string stringify_listitem(const dom::ListItem& li) const override;
  std::string stringify_paragraph(const dom::Paragraph& par) const override;
  std::string stringify_image(const dom::Image& img) const override;
  std::string stringify_math(const dex::DisplayMath& math) const override;
  std::string stringify_beginsince(const dex::BeginSince& bsince) const override;
  std::string stringify_endsince(const dex::EndSince& esince) const override;

  std::string format_group_item(const std::shared_ptr<cxx::Entity>& e) const;
  std::string stringify_grouptable(const dex::GroupTable& table) const override;

  std::string stringify_codeblock(const dex::CodeBlock& codeblock) const override;

  std::string stringify_section(const dex::Sectioning& sec) const override;
};

} // namespace dex

#endif // DEX_OUTPUT_LATEX_EXPORT_H
