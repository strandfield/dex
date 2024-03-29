// Copyright (C) 2019-2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex-output-resources.h"

#include "model-examples.h"

#include "dex/model/since.h"

#include "dex/common/file-utils.h"

#include "dex/output/config.h"
#include "dex/output/dir-copy.h"
#include "dex/output/json/json-export.h"

#ifdef DEX_EXPORTER_LIQUID_ENABLED
#include "dex/output/liquid/liquid-exporter.h"
#endif // DEX_EXPORTER_LIQUID_ENABLED

#include <json-toolkit/json.h>
#include <json-toolkit/stringify.h>

#include "catch.hpp"

#include <filesystem>
#include <iostream>

static std::shared_ptr<dex::Paragraph> make_par(const std::string& str)
{
  return std::make_shared<dex::Paragraph>(str);
}


std::string get_folder_path()
{
  std::filesystem::path dest = std::filesystem::temp_directory_path();
  dest = dest / "dex-test-markdown";

  if (!std::filesystem::exists(dest))
    dex::recursive_copy(std::filesystem::path(dex_output_resources_path()) / "test-templates" / "markdown", dest);

  return dest.string();
}

#ifdef DEX_EXPORTER_LIQUID_ENABLED

class MarkdownExport : public dex::LiquidExporter
{
public:
  explicit MarkdownExport(std::shared_ptr<dex::Model> m)
    : LiquidExporter(get_folder_path())
  {
    setModel(m);
  }

  ~MarkdownExport()
  {
    std::filesystem::remove_all(folderPath());
  }
};

#endif // DEX_EXPORTER_LIQUID_ENABLED

TEST_CASE("Test JSON export of a program", "[output]")
{
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(dex::examples::prog_with_class());

    json::Object jexport = dex::JsonExporter::serialize(*model).toObject();

    REQUIRE(jexport.data().size() == 1);

    jexport = jexport["program"]["global_namespace"].toObject();

    REQUIRE(jexport.data().size() == 4);
    REQUIRE(jexport.data().at("entities").length() == 1);
    REQUIRE(jexport.data().at("entities").toArray().length() == 1);
    REQUIRE(jexport.data().at("entities").at(0)["name"] == "vector");
  }
  
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(dex::examples::prog_with_fun());

    json::Object jexport = dex::JsonExporter::serialize(*model).toObject();
    jexport = jexport["program"]["global_namespace"].toObject();

    REQUIRE(jexport.data().size() == 4);
    REQUIRE(jexport.data().at("entities").length() == 1);
    REQUIRE(jexport.data().at("entities").toArray().length() == 1);
    REQUIRE(jexport.data().at("entities").at(0)["name"] == "getenv");

    jexport = jexport.data().at("entities").at(0).toObject();
    REQUIRE(jexport["parameters"].at(0)["documentation"] == "name of the environment variable");

    jexport = jexport["documentation"].toObject();
    REQUIRE(jexport["since"] == "C++98");
    REQUIRE(jexport["returns"] == "value of environment variable");
  }

  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(dex::examples::prog_with_var());

    json::Object jexport = dex::JsonExporter::serialize(*model).toObject();
    jexport = jexport["program"]["global_namespace"].toObject();

    REQUIRE(jexport.data().size() == 4);
    REQUIRE(jexport.data().at("entities").length() == 1);
    REQUIRE(jexport.data().at("entities").toArray().length() == 1);
    REQUIRE(jexport.data().at("entities").at(0)["name"] == "pi");
    REQUIRE(jexport.data().at("entities").at(0)["vartype"] == "double");

    jexport = jexport.data().at("entities").at(0)["documentation"].toObject();
    REQUIRE(jexport["description"].at(0)["text"] == "This mathematical constant is roughly equal to 3.");
  }
}

TEST_CASE("Test JSON export of a manual", "[output]")
{
  auto model = std::make_shared<dex::Model>();
  auto man = std::make_shared<dex::Manual>("The manual");
  model->documents.push_back(man);

  auto sec = std::make_shared<dex::Sectioning>(dex::Sectioning::Part, "Part 1");
  man->appendChild(sec);

  sec->content.push_back(make_par("Hello World!"));

  json::Object jexport = dex::JsonExporter::serialize(*model).toObject();

  REQUIRE(jexport.data().size() == 1);

  jexport = jexport["documents"][0].toObject();

  REQUIRE(jexport.data().at("title").toString() == "The manual");
  REQUIRE(jexport.data().at("content").toArray().length() == 1);

  jexport = jexport["content"][0].toObject();
  REQUIRE(jexport.data().at("depth").toString() == "part");
  REQUIRE(jexport.data().at("name").toString() == "Part 1");

  REQUIRE(jexport.data().at("content").toArray().length() == 1);
  jexport = jexport["content"][0].toObject();
  REQUIRE(jexport["text"].toString() == "Hello World!");
}

#ifdef DEX_EXPORTER_LIQUID_ENABLED

TEST_CASE("Test Markdown export", "[output]")
{
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(dex::examples::prog_with_class());

    MarkdownExport md_export{ model };
    md_export.render();

    std::string content = dex::file_utils::read_all(md_export.outputDir() / "classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n**Brief:** sequence container that encapsulates dynamic size arrays\n\n"
      "## Detailed description\n\nThe elements are stored contiguously, ...\n\n"
      "The storage of the vector is handled automatically, ...\n\n"
      "## Members documentation\n\n";

    REQUIRE(content == expected);
  }

  {
    auto model = dex::examples::prog_with_class_image_description();

    MarkdownExport md_export{ model };
    md_export.render();

    std::string content = dex::file_utils::read_all(md_export.outputDir() / "classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n"
      "## Detailed description\n\n![image](test.jpg)\n\n"
      "## Members documentation\n\n";

    REQUIRE(content == expected);
  }

  {
    auto model = dex::examples::prog_with_class_list_description();

    MarkdownExport md_export{ model };
    md_export.render();

    std::string content = dex::file_utils::read_all(md_export.outputDir() / "classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n"
      "## Detailed description\n\n- first item\n- second item\n\n"
      "## Members documentation\n\n";

    REQUIRE(content == expected);
  }
}

#endif // DEX_EXPORTER_LIQUID_ENABLED

#ifdef DEX_EXPORTER_LIQUID_ENABLED

TEST_CASE("Test Markdown export manual", "[output]")
{
  auto model = dex::examples::manual();

  MarkdownExport md_export{ model };
  md_export.render();

  std::string content = dex::file_utils::read_all(md_export.outputDir() / "documents/The manual.md");

  const std::string expected =
    "\n"
    "# The manual\n"
    "\n"
    "# First part\n"
    "\n"
    "## Chapter 1\n"
    "\n"
    "This is the first paragraph of the manual.\n"
    "\n"
    "And here is a second.\n"
    "\n"
    "## Chapter 2\n"
    "\n"
    "Warning! Skip this chapter.\n"
    "\n"
    "# Second part\n"
    "\n"
    "## Chapter 3\n"
    "\n"
    "![image](img.jpg)\n"
    "\n";

  REQUIRE(content == expected);
}

#endif // DEX_EXPORTER_LIQUID_ENABLED
