// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"
#include "dex/app/message-handler.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
  QCoreApplication qapp{ argc, argv };
  qapp.setApplicationName("dex");
  qapp.setApplicationVersion(dex::versionstr());

  dex::log::install_message_handler(&dex::app_message_handler);

  dex::Dex dexapp{ qapp.arguments() };
  return dexapp.exec();
}