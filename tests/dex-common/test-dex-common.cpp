// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-common.h"

#include "dex/common/file-utils.h"
#include "dex/common/json-utils.h"

#include <json-toolkit/stringify.h>

#include <iostream>

void TestDexCommon::jsonBuilder()
{
  dex::SettingsMap values;
  values["project.name"] = std::string("dex");
  values["author.planet"] = std::string("Coruscant");
  values["current_year"] = 1984;

  json::Json val = dex::build_json(values);

  QVERIFY(val["project"]["name"] == "dex");
  QVERIFY(val["author"]["planet"] == "Coruscant");
  QVERIFY(val["current_year"] == 1984);
}
