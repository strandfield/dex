// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/liquid-exporter-url-annotator.h"

#include <dex/model/manual.h>

#include <cxx/class.h>

namespace dex
{

LiquidExporterUrlAnnotator::LiquidExporterUrlAnnotator(const LiquidExporterProfile& pro, std::string file_extension)
  : profile(pro),
    suffix(std::move(file_extension))
{

}

std::string LiquidExporterUrlAnnotator::get_url(const cxx::Entity& e) const
{
  if (e.is<cxx::Class>())
    return profile.class_template.outdir + "/" + e.name() + suffix;

  return "";
}

std::string LiquidExporterUrlAnnotator::get_url(const dex::Manual& man) const
{
  // @TODO: remove spaces and illegal characters
  return profile.manual_template.outdir + "/" + man.title + suffix;
}

} // namespace dex
