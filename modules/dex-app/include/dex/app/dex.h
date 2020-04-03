// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_APP_DEX_H
#define DEX_APP_DEX_H

#include "dex/dex-app.h"

#include "dex/model/model.h"

#include <QCoreApplication>

class QDir;

namespace dex
{

class ParserMachine;

class DEX_APP_API Dex : public QCoreApplication
{
public:
  Dex(int argc, char* argv[]);

  int exec();

protected:
  std::string libClangVersion() const;

  void process(const QStringList& inputs, QString output);

  void feed(ParserMachine& parser, const QString& input);
  void feed(ParserMachine& parser, const QDir& input);

  void write_output(const std::shared_ptr<Model>& model, const QString& name);
  
private:
  QStringList m_suffixes;
};

} // namespace dex

#endif // DEX_APP_DEX_H
