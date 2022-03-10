// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/output/exporter.h"

#include "dex/output/config.h"
#include "dex/output/json/json-export.h"

#ifdef DEX_EXPORTER_LIQUID_ENABLED
#include "dex/output/liquid/liquid-exporter.h"
#endif // DEX_EXPORTER_LIQUID_ENABLED

#include "dex/common/errors.h"
#include "dex/common/file-utils.h"
#include "dex/common/logging.h"

#include <json-toolkit/stringify.h>

namespace dex
{

void run_exporter(const std::shared_ptr<dex::Model>& model, const std::filesystem::path& outdirpath, const json::Object& values)
{
  if (!std::filesystem::exists(outdirpath))
    throw std::runtime_error("No such directory " + outdirpath.string());

  std::filesystem::path profile_config_file = outdirpath / "_config.yml";
  json::Json config = dex::read_output_config(profile_config_file);
  std::string engine = config["engine"].toString();

  if (engine == "json")
  {
    auto obj = dex::JsonExporter::serialize(*model);

    std::filesystem::create_directory(outdirpath / "_output");

    dex::file_utils::write_file(outdirpath / "_output" / "dex.json", json::stringify(obj));

    return;
  }
  else if (engine == "liquid")
  {
#ifdef DEX_EXPORTER_LIQUID_ENABLED
    dex::LiquidExporter exporter{ outdirpath.string(), config };

    exporter.setVariables(values);
    exporter.setModel(model);

    exporter.render();

    return;
#else
    log::error() << "dex wasn't compiled with liquid support";
#endif // DEX_EXPORTER_LIQUID_ENABLED

  }
  else
  {
    throw std::runtime_error{ "Unknown export type" };
  }
}

} // namespace dex
