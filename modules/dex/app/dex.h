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

class QDir;

namespace dex
{

class ParserMachine;

class DEX_APP_API Dex
{
public:
  explicit Dex(const QStringList& arguments);

  int exec();

protected:
  void process(const QStringList& inputs, QString output, json::Object values);

  void work();

  void feed(ParserMachine& parser, const QString& input);
  void feed(ParserMachine& parser, const QDir& input);

  void write_output(const std::shared_ptr<Model>& model, const QString& outdir, json::Object values);
  
private:
  QStringList m_arguments;
  QStringList m_suffixes;
  CommandLineParserResult m_cli;
  Config m_config;
};

} // namespace dex

#endif // DEX_APP_DEX_H
