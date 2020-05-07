// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/latex-export.h"

#include "dex/output/liquid-exporter-url-annotator.h"
#include "dex/output/paragraph-converter.h"

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

class LatexParagraphConverter : public ParagraphConverter
{
public:

  using ParagraphConverter::ParagraphConverter;

  void process_bold(const dom::ParagraphIterator it) override
  {
    result += "\\textbf{";
    process(it);
    result += "}";
  }

  void process_italic(const dom::ParagraphIterator it) override
  {
    result += "\\textit{";
    process(it);
    result += "}";
  }

  void process_typewriter(const dom::ParagraphIterator it) override
  {
    result += "\\texttt{";
    process(it);
    result += "}";
  }
};

LatexExport::LatexExport()
{
  LiquidExporterProfile prof;
  prof.load(QDir{ ":/templates/latex" });
  profile() = std::move(prof);
}

void LatexExport::dump(std::shared_ptr<Model> model, const QDir& dir)
{
  LiquidExporter::setOutputDir(dir);
  LiquidExporter::setModel(model);

  json::Object json_export = LiquidExporter::serializedModel();

  LiquidExporterUrlAnnotator url_annotator{ profile(), ".tex" };
  url_annotator.annotate(*model, json_export);

  LiquidExporter::render();
}

void LatexExport::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
}

std::string LatexExport::stringify_list(const dom::List& list)
{
  std::string result = "\\begin{itemize}\n";

  for (const auto& li : list.items)
  {
    result += "  \\item " + stringify_listitem(*li) + "\n";
  }

  result += "\\end{itemize}";

  return result;
}

std::string LatexExport::stringify_listitem(const dom::ListItem& li)
{
  return stringify_domcontent(li.content);
}

std::string LatexExport::stringify_paragraph(const dom::Paragraph& par)
{
  LatexParagraphConverter converter{ par };
  converter.process();
  return std::string(std::move(converter.result));
}

std::string LatexExport::stringify_image(const dom::Image& img)
{
  return "\\includegraphics{" + img.src + "}";
}

std::string LatexExport::stringify_section(const dex::Sectioning& sec)
{
  std::string result;

  switch (sec.depth)
  {
  case dex::Sectioning::Part:
    result += "\\part{";
    break;
  case dex::Sectioning::Chapter:
    result += "\\chapter{";
    break;
  case dex::Sectioning::Section:
    result += "\\section{";
    break;
  default:
    result += "{";
    break;
  }
  
  result += sec.name + "}\n\n";

  for (const auto& c : sec.content)
  {
    result += stringify_domnode(*c) + "\n";
  }

  return result;
}

} // namespace dex
