// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
  QCoreApplication qapp{ argc, argv };
  qapp.setApplicationName("dex");
  qapp.setApplicationVersion(dex::versionstr());

  dex::Dex dexapp{ qapp.arguments() };
  return dexapp.exec();
}