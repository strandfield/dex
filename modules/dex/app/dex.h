// Copyright (C) 2019-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_DEX_H
#define DEX_APP_DEX_H

#include "dex/dex-app.h"

#include "dex/app/command-line-parser.h"
#include "dex/app/dex-config.h"
#include "dex/app/version.h"

#include "dex/model/model.h"

#include <QDir>

#include <filesystem>

namespace dex
{

class DEX_APP_API Dex
{
public:
  explicit Dex(const CommandLineParserResult& arguments);
  explicit Dex(const QDir& workdir);

  QDir workingDir() const;
  const Config& config() const;
  std::shared_ptr<Model> model() const;

  int exec();

  void readConfig();
  void parseInputs();
  void writeOutput();

protected:

  void work();

  void write_output(const std::shared_ptr<Model>& model, const std::filesystem::path& outdir, json::Object values);
  
private:
  QDir m_workdir;
  Config m_config;
  std::shared_ptr<Model> m_model;
};

} // namespace dex

#endif // DEX_APP_DEX_H
