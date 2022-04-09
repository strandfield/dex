// Copyright (C) 2020-2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex-parsing-resources.h"

#include "dex/common/file-utils.h"
#include "dex/common/string-utils.h"

#include "dex/input/parser-machine.h"

#include "dex/output/json/json-export.h"

#include <json-toolkit/stringify.h>

#include "catch.hpp"

#include <iostream>

TEST_CASE("Parse test dataset", "[parsing]")
{
  std::string datasets = dex::file_utils::read_all(std::string(dex_parsing_resources_path()) + "data/datasets.txt");
  datasets = dex::StdStringCRef(datasets).replace("\r\n", "\n");

  std::vector<std::string> list = dex::str_split(datasets, '\n');

  int num_failure = 0;

  for (const std::string& entry : list)
  {
    std::string input_file = std::string(dex_parsing_resources_path()) + "data/" + entry + ".txt";
    std::string expected_file = std::string(dex_parsing_resources_path()) + "data/" + entry + ".json";

    dex::ParserMachine machine;

    machine.process(input_file);

    std::shared_ptr<dex::Model> parse_result = machine.output();

    json::Object jexport = dex::JsonExporter::serialize(*parse_result);
    
    std::string serialized_result = json::stringify(jexport);

    std::string expected = dex::file_utils::read_all(expected_file);
    dex::file_utils::crlf2lf(expected);

    if (expected != serialized_result)
    {
      ++num_failure;

      std::cout << "FAIL!" << "\n";
      std::cout << "File: " << entry  << "\n";
      std::cout << "Expected:" << "\n";
      std::cout << expected << std::endl;
      std::cout << "Got:" << "\n";
      std::cout << serialized_result << std::endl;
    }
  }

  REQUIRE(num_failure == 0);
}
