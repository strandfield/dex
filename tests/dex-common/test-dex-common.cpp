// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-common.h"

#include "dex/common/file-utils.h"
#include "dex/common/json-utils.h"

#include <json-toolkit/stringify.h>

#include <iostream>

void TestDexCommon::jsonPathAnnotator()
{
  json::Object root;

  {
    json::Object user;
    user["name"] = "Bob";
    user["age"] = 99;

    json::Array users;
    users.push(user);

    root["users"] = users;
  }
  
  dex::JsonPathAnnotator annotator;
  annotator.field_name = "__path";
  annotator.annotate(root);

  QVERIFY(root["users"][0]["__path"] == "$.users[0]");
}
