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

#include <dom/image.h>
#include <dom/list.h>
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

  std::string get_url(const dex::Manual& man) const override
  {
    // @TODO: remove spaces and illegal characters
    return "manuals/" + man.title + ".md";
  }
};

MarkdownExport::MarkdownExport()
{
  LiquidExporterProfile prof;
  prof.load(QDir{ ":/templates/markdown" });
  profile() = std::move(prof);
}

void MarkdownExport::dump(std::shared_ptr<Model> model, const QDir& dir)
{
  LiquidExporter::setOutputDir(dir);
  LiquidExporter::setModel(model);

  json::Object json_export = LiquidExporter::serializedModel();

  JsonMarkdownUrlAnnotator url_annotator;
  url_annotator.annotate(*model, json_export);

  LiquidExporter::render();
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

std::string MarkdownExport::stringify_list(const dom::List& list)
{
  // @TODO: handle nested list

  std::string result;

  for (const auto& li : list.items)
  {
    result += "- " + stringify_listitem(*li) + "\n";
  }

  return result;
}

std::string MarkdownExport::stringify_listitem(const dom::ListItem& li)
{
  return stringify_domcontent(li.content);
}

std::string MarkdownExport::stringify_paragraph(const dom::Paragraph& par)
{
  // @TODO : stringify paragraph with formatting
  return par.text();
}

std::string MarkdownExport::stringify_image(const dom::Image& img)
{
  return "![image](" + img.src + ")";
}

std::string MarkdownExport::stringify_section(const dex::Sectioning& sec)
{
  std::string result;
  
  for (int i(0); i < (sec.depth - dex::Sectioning::Part) + 1; ++i)
    result.push_back('#');

  result.push_back(' ');

  result += sec.name + "\n\n";

  for (const auto& c : sec.content)
  {
    result += stringify_domnode(*c) + "\n";
  }

  return result;
}

} // namespace dex
