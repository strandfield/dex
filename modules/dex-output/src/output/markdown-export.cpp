// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/markdown-export.h"

#include "dex/output/liquid-exporter-url-annotator.h"

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
#include <dom/paragraph/iterator.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

namespace dex
{

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

  LiquidExporterUrlAnnotator url_annotator{ profile(), ".md" };
  url_annotator.annotate(*model, json_export);

  LiquidExporter::render();
}

void MarkdownExport::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
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

static void paragraph_conv_proc(const dom::ParagraphIterator begin, const dom::ParagraphIterator end, std::string& str)
{
  for (auto it = begin; it != end; ++it)
  {
    if (it.isText())
    {
      str += it.range().text();
    }
    else
    {
      const std::string& style = std::static_pointer_cast<dom::TextStyle>(*it)->style();
      
      std::string marker;

      if (style == "bold")
        marker = "**";

      str += marker;

      if (it.hasChild())
      {
        paragraph_conv_proc(it.begin(), it.end(), str);
      }
      else
      {
        str += it.range().text();
      }

      str += marker;
    }
  }
}

std::string MarkdownExport::stringify_paragraph(const dom::Paragraph& par)
{
  std::string result = "";
  paragraph_conv_proc(par.begin(), par.end(), result);
  return result;
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
