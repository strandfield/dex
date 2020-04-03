// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/markdown-export.h"

#include "dex//output/json-output-annotator.h"

#include "dex/model/since.h"

#include "dex/common/file-utils.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

#include <dom/paragraph.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

struct JsonMarkdownUrlAnnotator : JsonUrlAnnotator
{
  std::string get_url(const cxx::Entity& e) const override
  {
    if (e.is<cxx::Class>())
      return "classes/" + e.name() + ".md";
    else if (e.is<cxx::Namespace>())
      return "namespaces/" + e.name() + ".md";

    return "";
  }
};

MarkdownExport::MarkdownExport()
{
  std::string tmplt = file_utils::read_all(":/templates/markdown/class.md");
  templates().class_template = liquid::parse(tmplt);
}

void MarkdownExport::dump(std::shared_ptr<Model> model, const QDir& dir)
{
  LiquidExporter::setOutputDir(dir);
  LiquidExporter::setModel(model);

  json::Object json_export = LiquidExporter::serializedModel();

  JsonMarkdownUrlAnnotator url_annotator;
  url_annotator.annotate(*model, json_export);

  LiquidExporter::dumpClasses();
}

void MarkdownExport::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
}

json::Json MarkdownExport::applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args)
{
  return LiquidExporter::applyFilter(name, object, args);
}

std::string MarkdownExport::stringify_array(const json::Array& list)
{
  std::string result;

  for (const auto& val : list.data())
  {
    result += stringify(val);
    result += "\n\n";
  }

  return result;
}

std::string MarkdownExport::stringify_paragraph(const dom::Paragraph& par)
{
  // @TODO : stringify paragraph with formatting
  return par.text();
}

} // namespace dex
