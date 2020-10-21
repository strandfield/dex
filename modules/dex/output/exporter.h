// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_OUTPUT_EXPORTER_H
#define DEX_OUTPUT_EXPORTER_H

#include "dex/dex-output.h"

#include "dex/model/model.h"

#include <json-toolkit/json.h>

#include <QString>

namespace dex
{

class DEX_OUTPUT_API Exporter
{
public:
  Exporter();

  void process(const std::shared_ptr<dex::Model>& model, const QString& name, const json::Object& values);
};

} // namespace dex

#endif // DEX_OUTPUT_EXPORTER_H
