// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-common.h"

#include "dex/common/file-utils.h"
#include "dex/common/json-utils.h"

#include <json-toolkit/stringify.h>

#include <iostream>

void TestDexCommon::jsonAnnotator()
{
  {
    json::Object obj;
    obj["object"]["nested"]["value"] = 5;

    dex::JsonPathAnnotator annotator;
    annotator.annotate(obj);

    QVERIFY(obj["object"]["_path"] == "$.object");
    QVERIFY(obj["object"]["nested"]["_path"] == "$.object.nested");

    auto path = dex::JsonPathAnnotator::parse("$.object.nested");
    auto expected = std::vector<std::variant<size_t, std::string>>{ std::string("object"), std::string("nested") };
    QVERIFY(path == expected);
  }

  {
    json::Object obj;
    obj["list"] = json::Array();
    obj["list"].push(json::Object());
    obj["list"].push(json::Object());
    obj["list"][1]["subobject"] = json::Object();

    dex::JsonPathAnnotator annotator;
    annotator.annotate(obj);

    QVERIFY(obj["list"][0]["_path"] == "$.list[0]");
    QVERIFY(obj["list"][1]["subobject"]["_path"] == "$.list[1].subobject");

    auto path = dex::JsonPathAnnotator::parse("$.list[1].subobject");
    auto expected = std::vector<std::variant<size_t, std::string>>{ std::string("list"), 1, std::string("subobject") };
    QVERIFY(path == expected);
  }
}

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
