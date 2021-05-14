// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_URL_ANNOTATOR_H
#define DEX_OUTPUT_LIQUID_EXPORTER_URL_ANNOTATOR_H

#include "dex/output/json-output-annotator.h"
#include "dex/output/liquid-exporter-profile.h"

namespace dex
{

class LiquidExporterUrlAnnotator : public JsonUrlAnnotator
{
public:
  const LiquidExporterProfile& profile;

  LiquidExporterUrlAnnotator(json::Object& js_object, const JsonExportMapping& mapping, const LiquidExporterProfile& pro);

protected:
  std::string get_url(const dex::Entity& e) const override;
  std::string get_url(const dex::Document& doc) const override;
};

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_URL_ANNOTATOR_H
