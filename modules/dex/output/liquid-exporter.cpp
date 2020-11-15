// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter.h"

#include "dex/output/liquid-exporter-url-annotator.h"

#include "dex/output/liquid-filters.h"

#include "dex/output/markdown-export.h"
#include "dex/output/latex-export.h"

#include "dex/common/errors.h"
#include "dex/common/json-utils.h"
#include "dex/common/settings.h"

#include "dex/model/model.h"

#include <cxx/class.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

namespace dex
{

class LiquidExporterModelVisitor : public ModelVisitor
{
public:
  LiquidExporter& exporter;
  json::Object& serializedModel;
  JsonExportMapping& json_mapping;

  LiquidExporterModelVisitor(LiquidExporter& e, json::Object& m, JsonExportMapping& mapping)
    : exporter{ e },
    serializedModel{ m },
    json_mapping(mapping)
  {
  }

  void visit_class(const cxx::Class& cla) override
  {
    if (!exporter.profile().class_template.model.nodes().empty())
    {
      // We were previously fetching the json with
      //   JsonUrlAnnotator::get(path(), serializedModel).toObject();
      // but using the JsonExportMapping is expected to be faster.
      // @TODO: fallback to the path method if the object is not in the map
      json::Object obj = json_mapping.get(cla).toObject();
      exporter.selectStringifier(exporter.profile().class_template.filesuffix);
      exporter.dump(cla, obj);
    }

    ModelVisitor::visit_class(cla);
  }

  void visit_namespace(const cxx::Namespace& ns) override
  {
    if (!exporter.profile().namespace_template.model.nodes().empty())
    {
      json::Object obj = json_mapping.get(ns).toObject();
      exporter.selectStringifier(exporter.profile().namespace_template.filesuffix);
      exporter.dump(ns, obj);
    }

    ModelVisitor::visit_namespace(ns);
  }

  void visit_document(const dex::Document& doc) override
  {
    if (!exporter.profile().document_template.model.nodes().empty())
    {
      json::Object obj = JsonUrlAnnotator::get(path(), serializedModel).toObject();
      exporter.selectStringifier(exporter.profile().document_template.filesuffix);
      exporter.dump(doc, obj);
    }

    ModelVisitor::visit_document(doc);
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
  
  templates().clear();

  for (const std::pair<std::string, liquid::Template>& tmplts : m_profile.liquid_templates)
    templates()[tmplts.first] = tmplts.second;
}

QDir LiquidExporter::outputDir() const
{
  return m_output_dir;
}

void LiquidExporter::setOutputDir(const QDir& dir)
{
  m_output_dir = dir;
}

void LiquidExporter::setVariables(json::Object obj)
{
  m_user_variables = obj;
}

const json::Object& LiquidExporter::variables() const
{
  return m_user_variables;
}

void LiquidExporter::render()
{
  if (model()->empty())
    return;

  annotateModel();

  LiquidExporterModelVisitor visitor{ *this, m_serialized_model, m_model_mapping };
  visitor.visit(*model());

  for (const std::pair<std::string, liquid::Template>& file : profile().files)
  {
    json::Object context;
    setupContext(context);

    selectStringifier(QFileInfo(QString::fromStdString(file.first)).suffix().toStdString());

    std::string output = liquid::Renderer::render(file.second, context);

    postProcess(output);

    write(output, (outputDir().absolutePath() + "/" + QString::fromStdString(file.first)).toStdString());
  }
}

Model::Path LiquidExporter::convertToModelPath(const JsonPath& jspath)
{
  Model::Path result;

  for (const JsonPathElement& jspath_elem : jspath)
  {
    if (std::holds_alternative<size_t>(jspath_elem))
      result.back().index = std::get<size_t>(jspath_elem);
    else
      result.push_back(Model::PathElement(std::get<std::string>(jspath_elem)));
  }

  return result;
}

void LiquidExporter::annotateModel()
{
  LiquidExporterUrlAnnotator url_annotator{ m_serialized_model, m_model_mapping, profile() };
  url_annotator.annotate(*m_model);
}

void LiquidExporter::dump(const json::Object& obj, const char* obj_field_name, const Profile::Template& tmplt)
{
  if (obj["url"] == nullptr)
  {
    return;
  }

  const std::string url = obj["url"].toString();

  json::Object context;
  setupContext(context);
  context[obj_field_name] = obj;

  std::string output = liquid::Renderer::render(tmplt.model, context);

  postProcess(output);

  write(output, (m_output_dir.absolutePath() + "/" + QString::fromStdString(url)).toStdString());
}

void LiquidExporter::dump(const cxx::Class& /* cla */, const json::Object& obj)
{
  dump(obj, "class", m_profile.class_template);
}

void LiquidExporter::dump(const cxx::Namespace& /* ns */, const json::Object& obj)
{
  dump(obj, "namespace", m_profile.namespace_template);
}

void LiquidExporter::dump(const dex::Document& /* doc */, const json::Object& obj)
{
  dump(obj, "document", m_profile.document_template);
}

void LiquidExporter::setModel(std::shared_ptr<Model> model)
{
  m_model = model;
  
  JsonExport json_export{ *m_model };
  json_export.visit(*m_model);

  m_serialized_model = json_export.result;
  m_model_mapping = std::move(json_export.mapping);

  JsonPathAnnotator path_annotator;
  path_annotator.annotate(m_serialized_model);
}

std::string LiquidExporter::stringify(const json::Json& val)
{
  return m_stringifier->stringify(val);
}

void LiquidExporter::selectStringifier(const std::string& filesuffix)
{
  m_stringifier = m_stringifiers[filesuffix];
}

void LiquidExporter::setupContext(json::Object& context)
{
  context["model"] = m_serialized_model;

  for (const auto& e : m_user_variables.data())
  {
    context[e.first] = e.second;
  }
}

void LiquidExporter::postProcess(std::string& output)
{
  LiquidExporter::trim_right(output);
  LiquidExporter::simplify_empty_lines(output);
}

void LiquidExporter::write(const std::string& data, const std::string& filepath)
{
  if (data.empty())
    return;

  QFileInfo fileinfo{ QString::fromStdString(filepath) };

  if (!fileinfo.dir().exists())
  {
    const bool success = QDir().mkpath(fileinfo.dir().absolutePath());

    if (!success)
      throw IOException{ fileinfo.dir().absolutePath().toStdString(), "could not create directory" };
  }

  QFile file{ fileinfo.absoluteFilePath() };

  if (!file.open(QIODevice::WriteOnly))
    throw IOException{ fileinfo.absoluteFilePath().toStdString(), "could not open file for writing" };

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

json::Json LiquidExporter::applyFilter(const std::string& name, const json::Json& object, const std::vector<json::Json>& args)
{
  return m_filters->apply(name, object, args);
}

} // namespace dex
