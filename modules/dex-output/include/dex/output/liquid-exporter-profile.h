// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_LIQUID_EXPORTER_PROFILE_H
#define DEX_OUTPUT_LIQUID_EXPORTER_PROFILE_H

#include "dex/dex-output.h"

#include <liquid/renderer.h>

#include <QDir>

namespace dex
{

class DEX_OUTPUT_API LiquidExporterProfile
{
public:
  std::string profile_path;
  liquid::Template class_template;
  liquid::Template namespace_template;
  liquid::Template function_template;
  std::string class_outdir;
  std::string namespace_outdir;
  std::string function_outdir;
  std::vector<std::pair<std::string, liquid::Template>> files;

public:

  void load(const QDir& dir);
};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_PROFILE_H
