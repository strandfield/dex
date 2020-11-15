// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter-url-annotator.h"

#include <dex/model/manual.h>

#include <cxx/class.h>
#include <cxx/namespace.h>

namespace dex
{

LiquidExporterUrlAnnotator::LiquidExporterUrlAnnotator(json::Object& js_object, const JsonExportMapping& mapping, const LiquidExporterProfile& pro)
  : JsonUrlAnnotator(js_object, mapping), 
    profile(pro)
{

}

std::string LiquidExporterUrlAnnotator::get_url(const cxx::Entity& e) const
{
  if (e.is<cxx::Class>())
  {
    return profile.class_template.outdir + "/" + e.name + "." + profile.class_template.filesuffix;
  }
  else if (e.is<cxx::Namespace>())
  {
    if(e.name.empty())
      return profile.namespace_template.outdir + "/global." + profile.namespace_template.filesuffix;
    else
      return profile.namespace_template.outdir + "/" + e.name + "." + profile.namespace_template.filesuffix;
  }

  return "";
}

std::string LiquidExporterUrlAnnotator::get_url(const dex::Document& doc) const
{
  // @TODO: remove spaces and illegal characters
  return profile.document_template.outdir + "/" + doc.title + "." + profile.document_template.filesuffix;
}

} // namespace dex
