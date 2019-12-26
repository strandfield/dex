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

  void dump(const std::shared_ptr<cxx::Program>& prog, const QDir& dir);

protected:
  void postProcess(std::string& output) override;

protected:
  std::string stringify(const json::Json& val) override;
  json::Json applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args) override;

protected:
  std::string stringify_array(const json::Array& list);
  std::string stringify_paragraph(const json::Object& par);
};

} // namespace dex

#endif // DEX_OUTPUT_MARKDOWN_EXPORT_H
