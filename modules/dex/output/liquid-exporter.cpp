// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter.h"

#include "dex/output/liquid-filters.h"
#include "dex/output/liquid-wrapper.h"

#include "dex/output/markdown-export.h"
#include "dex/output/latex-export.h"

#include "dex/common/errors.h"
#include "dex/common/json-utils.h"

#include "dex/model/model.h"

#include <QDirIterator>

namespace dex
{

class LiquidExporterModelVisitor : public ProgramVisitor
{
public:
  LiquidExporter& exporter;

  LiquidExporterModelVisitor(LiquidExporter& e)
    : exporter{ e }
  {
  }

  void visitModel(const dex::Model& model)
  {
    for (auto d : model.documents)
      visit_document(*d);

    if (model.program())
    {
      ProgramVisitor::visit(*model.program());
    }
  }

  void visit(dex::Class& cla) override
  {
    if (!exporter.profile().class_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.profile().class_template.filesuffix);
      exporter.dump(cla);
    }

    ProgramVisitor::visit(cla);
  }

  void visit(dex::Namespace& ns) override
  {
    if (!exporter.profile().namespace_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.profile().namespace_template.filesuffix);
      exporter.dump(ns);
    }

    ProgramVisitor::visit(ns);
  }

  void visit(dex::Enum& /* enm */) override
  {
    // @TODO: one page per enum ?

    // do not visit enum values
    // ProgramVisitor::visit(enm);
  }

  void visit(dex::Function& /* fn */) override
  {
    // @TODO: one page per function ?

    // do not visit function parameters
    // ProgramVisitor::visit(fn);
  }

  void visit_document(dex::Document& doc)
  {
    if (!exporter.profile().document_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.profile().document_template.filesuffix);
      exporter.dump(doc);
    }
  }
};

LiquidExporter::LiquidExporter()
{
  m_stringifiers["md"] = std::make_shared<MarkdownStringifier>(*this);
  m_stringifiers["tex"] = std::make_shared<LatexStringifier>(*this);

  m_filters = std::make_unique<LiquidFilters>(*this);
}

LiquidExporter::~LiquidExporter()
{

}

void LiquidExporter::setProfile(Profile pro)
{
  m_profile = std::move(pro);
  m_output_dir = QDir(QString::fromStdString(m_profile.profile_path + "/_output"));
  
  templates().clear();

  for (const std::pair<std::string, liquid::Template>& tmplts : m_profile.liquid_templates)
    templates()[tmplts.first] = tmplts.second;
}

QDir LiquidExporter::outputDir() const
{
  return m_output_dir;
}

void LiquidExporter::setVariables(liquid::Map obj)
{
  m_user_variables = obj;
}

const liquid::Map& LiquidExporter::variables() const
{
  return m_user_variables;
}

void LiquidExporter::render()
{
  if (model()->empty())
    return;

  LiquidExporterModelVisitor visitor{ *this, };
  visitor.visitModel(*model());

  QDirIterator diriterator{ m_profile.profile_path.c_str(), QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs };

  while (diriterator.hasNext())
  {
    diriterator.next();
    QFileInfo fileinfo = diriterator.fileInfo();

    if (fileinfo.fileName().startsWith("_"))
      continue;

    if (fileinfo.isDir())
    {
      renderDirectory(fileinfo.absoluteFilePath());
    }
    else
    {
      renderFile(fileinfo.absoluteFilePath());
    }
  }
}

std::string LiquidExporter::get_url(const dex::Entity& e) const
{
  if (e.is<dex::Class>())
  {
    return profile().class_template.outdir + "/" + e.name + "." + profile().class_template.filesuffix;
  }
  else if (e.is<dex::Namespace>())
  {
    if (e.name.empty())
      return profile().namespace_template.outdir + "/global." + profile().namespace_template.filesuffix;
    else
      return profile().namespace_template.outdir + "/" + e.name + "." + profile().namespace_template.filesuffix;
  }

  return "";
}

std::string LiquidExporter::get_url(const dex::Document& doc) const
{
  // @TODO: remove spaces and illegal characters
  return profile().document_template.outdir + "/" + doc.title + "." + profile().document_template.filesuffix;
}

std::string LiquidExporter::get_url(const std::shared_ptr<model::Object>& obj) const
{
  if (obj->isProgramEntity())
    return get_url(static_cast<dex::Entity&>(*obj));
  else if (obj->isDocument())
    return get_url(static_cast<dex::Document&>(*obj));
  else
    return {};
}

void LiquidExporter::dump(const std::shared_ptr<model::Object>& obj, const char* obj_field_name, const Profile::Template& tmplt)
{
  const std::string url = get_url(obj);

  if (url.empty())
    return;

  liquid::Map context;
  setupContext(context);
  context[obj_field_name] = to_liquid(obj);
  context["url"] = url;

  std::string output = liquid::Renderer::render(tmplt.model, context);

  postProcess(output);

  write(output, (m_output_dir.absolutePath() + "/" + QString::fromStdString(url)).toStdString());
}

void LiquidExporter::dump(dex::Class& cla)
{
  dump(cla.shared_from_this(), "class", m_profile.class_template);
}

void LiquidExporter::dump(dex::Namespace& ns)
{
  dump(ns.shared_from_this(), "namespace", m_profile.namespace_template);
}

void LiquidExporter::dump(dex::Document& doc)
{
  dump(doc.shared_from_this(), "document", m_profile.document_template);
}

void LiquidExporter::setModel(std::shared_ptr<Model> model)
{
  m_model = model;
}

std::string LiquidExporter::stringify(const liquid::Value& val)
{
  return m_stringifier->stringify(val);
}

void LiquidExporter::renderDirectory(const QString& path)
{
  QDirIterator diriterator{ m_profile.profile_path.c_str(), QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories };

  while (diriterator.hasNext())
  {
    QString path = diriterator.next();
    renderFile(path);
  }
}

void LiquidExporter::renderFile(const QString& filepath)
{
  QFileInfo fileinfo{ filepath };

  QString srcrelpath = QDir(profile().profile_path.c_str()).relativeFilePath(fileinfo.absoluteFilePath());
  QString destpath = outputDir().absolutePath() + "/" + srcrelpath;

  if (!isSpecialFile(fileinfo))
  {
    checkWriteDirectory(destpath.toStdString());
    QFile::copy(filepath, destpath);
    return;
  }
  
  TemplateWithFrontMatter tmplt = open_template_with_front_matter(filepath.toStdString());
  tmplt.model.skipWhitespacesAfterTag();

  // @TODO: use tmplt.frontmatter

  liquid::Map context;
  setupContext(context);

  selectStringifier(fileinfo.suffix().toStdString());

  std::string output = liquid::Renderer::render(tmplt.model, context);

  postProcess(output);

  write(output, destpath.toStdString());
}

bool LiquidExporter::isSpecialFile(const QFileInfo& fileinfo) const
{
  static const std::set<QString> suffix_whitelist = {
    "md", "tex", "html", "txt", "css", "js"
  };

  if (suffix_whitelist.find(fileinfo.suffix()) == suffix_whitelist.end())
    return false;

  QFile file{ fileinfo.absoluteFilePath() };

  if (!file.open(QIODevice::ReadOnly))
    return false;

  // Check if file has a front-matter
  return file.read(3) == "---";
}

void LiquidExporter::selectStringifier(const std::string& filesuffix)
{
  m_stringifier = m_stringifiers[filesuffix];
  m_stringifier->selected();
}

void LiquidExporter::setupContext(liquid::Map& context)
{
  context["model"] = to_liquid(m_model);

  for (const std::string& pname : m_user_variables.propertyNames())
  {
    context[pname] = m_user_variables.property(pname);
  }
}

void LiquidExporter::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
}

void LiquidExporter::checkWriteDirectory(const std::string& filepath)
{
  QFileInfo fileinfo{ QString::fromStdString(filepath) };

  if (!fileinfo.dir().exists())
  {
    const bool success = QDir().mkpath(fileinfo.dir().absolutePath());

    if (!success)
      throw IOException{ fileinfo.dir().absolutePath().toStdString(), "could not create directory" };
  }
}

void LiquidExporter::write(const std::string& data, const std::string& filepath)
{
  if (data.empty())
    return;

  checkWriteDirectory(filepath);

  QFile file{ filepath.c_str() };

  if (!file.open(QIODevice::WriteOnly))
    throw IOException{ filepath, "could not open file for writing" };

  file.write(data.data());

  file.close();
}

void LiquidExporter::trim_right(std::string& str)
{
  // Remove spaces before end of line '\n'
  {
    size_t w = 0;

    for (size_t r = 0; r < str.size();)
    {
      if (str.at(r) == ' ')
      {
        // Find next non space char
        size_t rr = r;

        while (rr < str.size() && str.at(rr) == ' ') ++rr;

        if (str.at(rr) == '\n') // discard the spaces
        {
          r = rr;
        }
        else // keep the spaces
        {
          while (r < rr)
            str[w++] = str[r++];
        }
      }
      else
      {
        str[w++] = str[r++];
      }
    }

    str.resize(w);
  }
}

void LiquidExporter::simplify_empty_lines(std::string& str)
{
  size_t w = 0;

  for (size_t r = 0; r < str.size();)
  {
    if (str.at(r) == '\n')
    {
      // Find next non new-line char
      size_t rr = r;

      while (rr < str.size() && str.at(rr) == '\n') ++rr;

      if (rr - r >= 3) // discard the extra '\n'
      {
        r = rr;
        str[w++] = '\n';
        str[w++] = '\n';
      }
      else // keep the newlines
      {
        while (r < rr)
          str[w++] = str[r++];
      }
    }
    else
    {
      str[w++] = str[r++];
    }
  }

  str.resize(w);
}

liquid::Value LiquidExporter::applyFilter(const std::string& name, const liquid::Value& object, const std::vector<liquid::Value>& args)
{
  return m_filters->apply(name, object, args);
}

} // namespace dex
