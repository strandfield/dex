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

  struct Template
  {
    liquid::Template model;
    std::string outdir;
    std::string filesuffix;
  };

public:
  std::string profile_path;
  Template class_template;
  Template document_template;
  std::vector<std::pair<std::string, liquid::Template>> files;
  std::vector<std::pair<std::string, liquid::Template>> liquid_templates;

public:

  void load(const QDir& dir);

};

} // namespace dex

namespace dex
{

} // namespace dex

#endif // DEX_OUTPUT_LIQUID_EXPORTER_PROFILE_H
