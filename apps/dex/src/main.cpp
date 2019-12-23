// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/app/dex.h"

int main(int argc, char *argv[])
{
  dex::Dex app(argc, argv);
  return app.exec();
}