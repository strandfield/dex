// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-output.h"

#include "dex/model/since.h"

#include "dex/output/json-annotator.h"
#include "dex/output/json-export.h"
#include "dex/output/markdown-export.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>

#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <json-toolkit/stringify.h>

#include <iostream>

// TODO: refactor duplicated block
static std::string read_all(const QFileInfo& info)
{
  QFile file{ info.absoluteFilePath() };

  if (!file.open(QIODevice::ReadOnly))
    throw std::runtime_error{ "Could not open file" };

  return file.readAll().toStdString();
}

static std::shared_ptr<dom::Paragraph> make_par(const std::string& str)
{
  return std::make_shared<dom::Paragraph>(str);
}

static std::shared_ptr<cxx::Program> example_prog_with_class()
{
  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  doc->brief() = "sequence container that encapsulates dynamic size arrays";
  doc->description().push_back(make_par("The elements are stored contiguously, ..."));
  doc->description().push_back(make_par("The storage of the vector is handled automatically, ..."));
  vector->setDocumentation(doc);

  global->entities().push_back(vector);

  return prog;
}

static std::shared_ptr<cxx::Program> example_prog_with_fun()
{
  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto getenv = std::make_shared<cxx::Function>("getenv", global);
  auto doc = std::make_shared<dex::FunctionDocumentation>();
  doc->brief() = "get value from environment variables";
  doc->parameters().push_back("name of the environment variable");
  doc->returnValue() = "value of environment variable";
  doc->since() = dex::Since{ "C++98" };
  doc->description().push_back(make_par("Searches the environment list provided by the host environment..."));
  doc->description().push_back(make_par("Modifying the string returned by getenv invokes undefined behavior."));
  getenv->setDocumentation(doc);

  global->entities().push_back(getenv);

  return prog;
}

static std::shared_ptr<cxx::Program> example_prog_with_class_and_fun()
{
  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto complex = std::make_shared<cxx::Class>("complex", global);
  global->entities().push_back(complex);

  auto real = std::make_shared<cxx::Function>("real", complex);
  complex->members().emplace_back(real, cxx::AccessSpecifier::PUBLIC);

  return prog;
}

void TestDexOutput::jsonExport()
{
  {
    std::shared_ptr<cxx::Program> prog = example_prog_with_class();

    json::Object jexport = dex::JsonExport::serialize(*prog).toObject();

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);
    QVERIFY(jexport.data().at("entities").at(0)["name"] == "vector");
  }
  
  {
    std::shared_ptr<cxx::Program> prog = example_prog_with_fun();

    json::Object jexport = dex::JsonExport::serialize(*prog).toObject();

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);
    QVERIFY(jexport.data().at("entities").at(0)["name"] == "getenv");

    jexport = jexport.data().at("entities").at(0)["documentation"].toObject();
    QVERIFY(jexport["since"] == "C++98");
    QVERIFY(jexport["parameters"].at(0) == "name of the environment variable");
    QVERIFY(jexport["returns"] == "value of environment variable");
  }
}

void TestDexOutput::jsonAnnotator()
{
  {
    std::shared_ptr<cxx::Program> prog = example_prog_with_class();

    json::Object jexport = dex::JsonExport::serialize(*prog).toObject();

    dex::JsonPathAnnotator annotator;
    annotator.annotate(*prog, jexport);

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);

    json::Object vec = jexport.data().at("entities").at(0).toObject();
    QVERIFY(vec["_path"] == "$.entities[0]");

    auto path = dex::JsonPathAnnotator::parse("$.entities[0]");
    auto expected = std::vector<std::variant<size_t, std::string>>{ std::string("entities"), 0 };
    QVERIFY(path == expected);
  }

  {
    std::shared_ptr<cxx::Program> prog = example_prog_with_class_and_fun();

    json::Object jexport = dex::JsonExport::serialize(*prog).toObject();

    dex::JsonPathAnnotator annotator;
    annotator.annotate(*prog, jexport);

    json::Object complex = jexport.data().at("entities").at(0).toObject();
    QVERIFY(complex["_path"] == "$.entities[0]");
    json::Object real = complex.data().at("members").at(0).toObject();
    QVERIFY(real["_path"] == "$.entities[0].members[0]");

    auto path = dex::JsonPathAnnotator::parse("$.entities[0].members[0]");
    auto expected = std::vector<std::variant<size_t, std::string>>{ std::string("entities"), 0, std::string("members"), 0 };
    QVERIFY(path == expected);
  }
}

void TestDexOutput::markdownExport()
{
  {
    std::shared_ptr<cxx::Program> prog = example_prog_with_class();
    dex::MarkdownExport md_export;

    md_export.dump(prog, QDir::current());

    std::string content = read_all(QFileInfo{ "classes/vector.md" });

    const std::string expected =
      "\n# vector Class\n\n**Brief:** sequence container that encapsulates dynamic size arrays\n\n"
      "## Detailed description\n\nThe elements are stored contiguously, ...\n\n"
      "The storage of the vector is handled automatically, ...\n\n"
      "## Members documentation\n\n";

    QVERIFY(content == expected);
  }
}
