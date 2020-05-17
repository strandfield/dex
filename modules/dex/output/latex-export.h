// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LATEX_EXPORT_H
#define DEX_OUTPUT_LATEX_EXPORT_H

#include "dex/output/liquid-exporter.h"

namespace dex
{

class DEX_OUTPUT_API LatexExport : protected LiquidExporter
{
public:
  LatexExport();

  using LiquidExporter::setVariables;

  void dump(std::shared_ptr<Model> model, const QDir& dir);

protected:
  void postProcess(std::string& output) override;

protected:
  std::string stringify_list(const dom::List& list) override;
  std::string stringify_listitem(const dom::ListItem& li) override;
  std::string stringify_paragraph(const dom::Paragraph& par) override;
  std::string stringify_image(const dom::Image& img) override;
  std::string stringify_math(const dex::DisplayMath& math) override;
  std::string stringify_section(const dex::Sectioning& sec) override;
};

} // namespace dex

#endif // DEX_OUTPUT_LATEX_EXPORT_H
