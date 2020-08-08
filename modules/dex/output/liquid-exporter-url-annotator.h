// Copyright (C) 2020 Vincent Chambrin
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
  std::string suffix;

  LiquidExporterUrlAnnotator(json::Object& js_object, const JsonExportMapping& mapping, const LiquidExporterProfile& pro, std::string file_extension);

protected:
  std::string get_url(const cxx::Entity& e) const override;
  std::string get_url(const dex::Manual& man) const override;
};

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_URL_ANNOTATOR_H
