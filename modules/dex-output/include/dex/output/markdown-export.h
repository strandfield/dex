// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_MARKDOWN_EXPORT_H
#define DEX_OUTPUT_MARKDOWN_EXPORT_H

#include "dex/output/liquid-exporter.h"

#include <QDir>

namespace cxx
{
class Entity;
class Documentation;
class Program;
} // namespace cxx

namespace dex
{

class DEX_OUTPUT_API MarkdownExport : protected LiquidExporter
{
public:
  MarkdownExport();

  void dump(std::shared_ptr<Model> model, const QDir& dir);

protected:
  void postProcess(std::string& output) override;

protected:
  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

protected:
  std::string stringify_array(const json::Array& list) override;
  std::string stringify_list(const dom::List& list) override;
  std::string stringify_listitem(const dom::ListItem& li) override;
  std::string stringify_paragraph(const dom::Paragraph& par) override;
  std::string stringify_image(const dom::Image& img) override;
};

} // namespace dex

#endif // DEX_OUTPUT_MARKDOWN_EXPORT_H
