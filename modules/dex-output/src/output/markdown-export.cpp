// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/markdown-export.h"

#include "dex/output/liquid-exporter-url-annotator.h"
#include "dex/output/paragraph-converter.h"

#include "dex/model/inline-math.h"
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

class MarkdownParagraphConverter : public ParagraphConverter
{
public:

  using ParagraphConverter::ParagraphConverter;

  void process_bold(const dom::ParagraphIterator it) override
  {
    result += "**";
    process(it);
    result += "**";
  }

  void process_italic(const dom::ParagraphIterator it) override
  {
    result += "*";
    process(it);
    result += "*";
  }

  void process_typewriter(const dom::ParagraphIterator it) override
  {
    result += "`";
    process(it);
    result += "`";
  }

  void process_math(const dom::ParagraphIterator it) override
  {
    result += "`";
    auto text_begin = it.paragraph().text().begin();
    result.insert(result.end(), text_begin + it.range().begin() + 1, text_begin + it.range().end() - 1);
    result += "`";
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

  LiquidExporterUrlAnnotator url_annotator{ profile(), ".md" };
  url_annotator.annotate(*model, json_export);

  LiquidExporter::render();
}

void MarkdownExport::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
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
  MarkdownParagraphConverter converter{ par };
  converter.process();
  return std::string(std::move(converter.result));
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
