// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid/liquid-exporter.h"

#include "dex/output/liquid/liquid-filters.h"
#include "dex/output/liquid/liquid-wrapper.h"

#include "dex/output/liquid/markdown-export.h"
#include "dex/output/liquid/latex-export.h"
#include "dex/output/config.h"

#include "dex/model/model.h"

#include "dex/common/errors.h"
#include "dex/common/file-utils.h"
#include "dex/common/string-utils.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <set>


namespace dex
{

liquid::Value json_to_liquid(const json::Json& js)
{
  if (js.isInteger())
    return js.toInt();
  else if (js.isBoolean())
    return js.toBool();
  else if (js.isNumber())
    return js.toNumber();
  else if (js.isString())
    return js.toString();

  if (js.isObject())
  {
    liquid::Map r = {};

    for (const auto& entry : js.toObject().data())
      r[entry.first] = json_to_liquid(entry.second);

    return r;
  }
  else
  {
    liquid::Array r = {};

    for (const auto& entry : js.toArray().data())
      r.push(json_to_liquid(entry));

    return r;
  }
}

liquid::Template open_liquid_template(const std::filesystem::path& path)
{
  std::string tmplt = file_utils::read_all(path);
  return liquid::parse(tmplt);
}

TemplateWithFrontMatter open_template_with_front_matter(const std::filesystem::path& path)
{
  std::ifstream file{ path.string(), std::ios::in };

  std::string l;
  std::getline(file, l);

  std::string frontmatter;
  std::getline(file, l);

  while (!dex::StdStringCRef(l).starts_with("---"))
  {
    frontmatter += l;
    frontmatter += "\n";
    std::getline(file, l);
  }

  YAML::Node yam = YAML::Load(frontmatter.data());

  std::string tmplt_content;

  while (!file.eof())
  {
    std::getline(file, l);
    tmplt_content += l;
    tmplt_content += '\n';
  }

  TemplateWithFrontMatter result;
  result.frontmatter = yaml_to_json(yam).toObject();
  result.model = liquid::parse(tmplt_content);

  return result;
}

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
    if (!exporter.layouts().class_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.layouts().class_template.filesuffix);
      exporter.dump(cla);
    }

    ProgramVisitor::visit(cla);
  }

  void visit(dex::Namespace& ns) override
  {
    if (!exporter.layouts().namespace_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.layouts().namespace_template.filesuffix);
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
    if (!exporter.layouts().document_template.model.nodes().empty())
    {
      exporter.selectStringifier(exporter.layouts().document_template.filesuffix);
      exporter.dump(doc);
    }
  }
};

LiquidExporter::LiquidExporter(std::string folder_path, const json::Json config)
  : m_folder_path(std::move(folder_path)),
    m_config(config)
{
  m_stringifiers["md"] = std::make_shared<MarkdownStringifier>(*this);
  m_stringifiers["tex"] = std::make_shared<LatexStringifier>(*this);

  m_filters = std::make_unique<LiquidFilters>(*this);

  m_output_path = m_folder_path + "/_output";

  if (m_config == json::null)
  {
    m_config = dex::read_output_config(m_folder_path + "/_config.yml");
  }

  listLayouts();
  listIncludes();
}

LiquidExporter::~LiquidExporter()
{

}

const std::string& LiquidExporter::folderPath() const
{
  return m_folder_path;
}

const std::string& LiquidExporter::outputPath() const
{
  return m_output_path;
}

std::filesystem::path LiquidExporter::outputDir() const
{
  return  std::filesystem::path(outputPath());
}

const LiquidExporter::Layouts& LiquidExporter::layouts() const
{
  return m_layouts;
}

void LiquidExporter::setVariables(const json::Object& obj)
{
  setVariables(json_to_liquid(obj).toMap());
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

  if (std::filesystem::exists(outputDir()))
    std::filesystem::remove_all(outputDir());

  LiquidExporterModelVisitor visitor{ *this, };
  visitor.visitModel(*model());

  std::filesystem::directory_iterator diriterator{ folderPath() };

  for (const std::filesystem::directory_entry& entry : diriterator)
  {
    if (dex::StdStringCRef(entry.path().filename().string()).starts_with("_"))
      continue;

    if (entry.is_directory())
    {
      renderDirectory(entry.path());
    }
    else if(entry.is_regular_file())
    {
      renderFile(entry.path());
    }
  }
}

std::string LiquidExporter::get_url(const dex::Entity& e) const
{
  if (e.is<dex::Class>())
  {
    return m_layouts.class_template.outdir + "/" + e.name + "." + m_layouts.class_template.filesuffix;
  }
  else if (e.is<dex::Namespace>())
  {
    if (e.name.empty())
      return m_layouts.namespace_template.outdir + "/global." + m_layouts.namespace_template.filesuffix;
    else
      return m_layouts.namespace_template.outdir + "/" + e.name + "." + m_layouts.namespace_template.filesuffix;
  }

  return "";
}

std::string LiquidExporter::get_url(const dex::Document& doc) const
{
  // @TODO: remove spaces and illegal characters
  return m_layouts.document_template.outdir + "/" + doc.title + "." + m_layouts.document_template.filesuffix;
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

void LiquidExporter::dump(const std::shared_ptr<model::Object>& obj, const char* obj_field_name, const LiquidLayout& layout)
{
  const std::string url = get_url(obj);

  if (url.empty())
    return;

  liquid::Map context;
  setupContext(context);
  context[obj_field_name] = to_liquid(obj);
  context["url"] = url;

  std::string output = liquid::Renderer::render(layout.model, context);

  postProcess(output);

  write(output, outputPath() + "/" + url);
}

void LiquidExporter::dump(dex::Class& cla)
{
  dump(cla.shared_from_this(), "class", m_layouts.class_template);
}

void LiquidExporter::dump(dex::Namespace& ns)
{
  dump(ns.shared_from_this(), "namespace", m_layouts.namespace_template);
}

void LiquidExporter::dump(dex::Document& doc)
{
  dump(doc.shared_from_this(), "document", m_layouts.document_template);
}

void LiquidExporter::setModel(std::shared_ptr<Model> model)
{
  m_model = model;
}

std::string LiquidExporter::stringify(const liquid::Value& val)
{
  return m_stringifier->stringify(val);
}

void LiquidExporter::listLayouts()
{
  std::filesystem::path dir = std::filesystem::path(folderPath()) / "_layouts";

  if (!std::filesystem::exists(dir))
    return;

  std::filesystem::directory_iterator diriterator{ dir };

  for (const std::filesystem::directory_entry& entry : diriterator)
  {
    std::filesystem::path p = entry.path();

    if (p.stem() == "class")
      m_layouts.class_template = parseLayout(p, "class", "classes");
    else if (p.stem() == "namespace")
      m_layouts.namespace_template = parseLayout(p, "namespace", "namespaces");
    else if (p.stem() == "document")
      m_layouts.document_template = parseLayout(p, "document", "documents");
  }
}

LiquidLayout LiquidExporter::parseLayout(const std::filesystem::path& fileinfo, const std::string& name, std::string default_out)
{
  LiquidLayout result;
  std::string path = fileinfo.string();
  result.model = open_liquid_template(path);
  result.model.skipWhitespacesAfterTag();
  result.outdir = dex::config::read(m_config["output"], name, std::move(default_out)).toString();
  result.filesuffix = fileinfo.extension().string();
  if (!result.filesuffix.empty())
    result.filesuffix = std::string(result.filesuffix.begin() + 1, result.filesuffix.end());
  return result;
}

void LiquidExporter::listIncludes()
{
  std::filesystem::path dir = std::filesystem::path(folderPath()) / "_includes";

  if (!std::filesystem::exists(dir))
    return;

  std::filesystem::directory_iterator diriterator{ dir };

  for (const std::filesystem::directory_entry& entry : diriterator)
  {
    std::filesystem::path p = entry.path();
    liquid::Template tmplt = open_liquid_template(p);
    tmplt.skipWhitespacesAfterTag();
    std::string p_str = p.string();
    p_str.erase(p_str.begin(), p_str.begin() + folderPath().size() + 11);
    templates()[p_str] = std::move(tmplt);
  }
}

void LiquidExporter::renderDirectory(const std::filesystem::path& dirpath)
{
  std::filesystem::recursive_directory_iterator diriterator{ dirpath };

  for (const std::filesystem::directory_entry& entry : diriterator)
  {
    if (entry.is_regular_file())
      renderFile(entry.path());
  }
}

void LiquidExporter::renderFile(const std::filesystem::path& filepath)
{
  std::filesystem::path srcrelpath = std::filesystem::relative(filepath, folderPath());
  std::filesystem::path destpath = outputDir() / srcrelpath;

  if (!isSpecialFile(filepath))
  {
    checkWriteDirectory(destpath.string());
    std::filesystem::copy_file(filepath, destpath);
    return;
  }
  
  TemplateWithFrontMatter tmplt = open_template_with_front_matter(filepath);
  tmplt.model.skipWhitespacesAfterTag();

  // @TODO: use tmplt.frontmatter

  liquid::Map context;
  setupContext(context);

  selectStringifier(filepath.extension().string().substr(1));

  std::string output = liquid::Renderer::render(tmplt.model, context);

  postProcess(output);

  write(output, destpath);
}

bool LiquidExporter::isSpecialFile(const std::filesystem::path& fileinfo) const
{
  static const std::set<std::string> suffix_whitelist = {
    ".md", ".tex", ".html", ".txt", ".css", ".js"
  };

  if (suffix_whitelist.find(fileinfo.extension().string()) == suffix_whitelist.end())
    return false;

  std::ifstream file{ fileinfo.string() };
  std::string head = dex::file_utils::read(file, 3);

  // Check if file has a front-matter
  return head == "---";
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

void LiquidExporter::checkWriteDirectory(const std::filesystem::path& filepath)
{
  auto parent_path = filepath.parent_path();

  if (!std::filesystem::exists(parent_path))
  {
    bool success = std::filesystem::create_directories(parent_path);

    if (!success)
      throw IOException{ parent_path.string(), "could not create directory" };
  }
}

void LiquidExporter::write(const std::string& data, const std::filesystem::path& filepath)
{
  if (data.empty())
    return;

  checkWriteDirectory(filepath);

  dex::file_utils::write_file(filepath, data);
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
