// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-output.h"

#include "dex/model/since.h"

#include "dex/common/file-utils.h"
#include "dex/common/json-utils.h"

#include "dex/output/json-export.h"
#include "dex/output/markdown-export.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>
#include <cxx/variable.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <json-toolkit/stringify.h>

#include <iostream>

static std::shared_ptr<dom::Paragraph> make_par(const std::string& str)
{
  return std::make_shared<dom::Paragraph>(str);
}

template<typename T, typename...Args>
std::shared_ptr<T> make(Args&&... args)
{
  return std::make_shared<T>(std::forward<Args>(args)...);
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

static std::shared_ptr<dex::Model> example_prog_with_class_image_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  auto img = make<dom::Image>("test.jpg");
  doc->description().push_back(img);
  vector->setDocumentation(doc);

  global->entities().push_back(vector);

  model->setProgram(prog);

  return model;
}

static std::shared_ptr<dex::Model> example_prog_with_class_list_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  auto list = make<dom::List>();
  auto listitem = make<dom::ListItem>();
  listitem->content.push_back(make<dom::Paragraph>("first item"));
  list->items.push_back(listitem);
  listitem = make<dom::ListItem>();
  listitem->content.push_back(make<dom::Paragraph>("second item"));
  list->items.push_back(listitem);
  doc->description().push_back(list);
  vector->setDocumentation(doc);

  global->entities().push_back(vector);

  model->setProgram(prog);

  return model;
}

static std::shared_ptr<cxx::Program> example_prog_with_fun()
{
  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto getenv = std::make_shared<cxx::Function>("getenv", global);
  auto doc = std::make_shared<dex::FunctionDocumentation>();
  doc->brief() = "get value from environment variables";
  doc->returnValue() = "value of environment variable";
  doc->since() = dex::Since{ "C++98" };
  doc->description().push_back(make_par("Searches the environment list provided by the host environment..."));
  doc->description().push_back(make_par("Modifying the string returned by getenv invokes undefined behavior."));
  getenv->setDocumentation(doc);

  getenv->parameters().push_back(make<cxx::FunctionParameter>(cxx::Type("std::string"), "str"));
  getenv->parameters().front()->setDocumentation(make<dex::FunctionParameterDocumentation>("name of the environment variable"));

  global->entities().push_back(getenv);

  return prog;
}

static std::shared_ptr<cxx::Program> example_prog_with_var()
{
  auto prog = std::make_shared<cxx::Program>();
  auto global = prog->globalNamespace();

  auto variable = std::make_shared<cxx::Variable>(cxx::Type("double"), "pi", global);
  auto doc = std::make_shared<dex::VariableDocumentation>();
  doc->brief() = "the math constant pi";
  doc->description().push_back(make_par("This mathematical constant is roughly equal to 3."));
  variable->setDocumentation(doc);

  global->entities().push_back(variable);

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

static std::shared_ptr<dex::Model> example_manual()
{
  auto model = make<dex::Model>();

  auto man = make<dex::Manual>("The manual");
  model->manuals().push_back(man);

  {
    auto part = make<dex::Sectioning>(dex::Sectioning::Part, "First part");
    man->content.push_back(part);

    {
      auto chap = make<dex::Sectioning>(dex::Sectioning::Chapter, "Chapter 1");
      part->content.push_back(chap);

      chap->content.push_back(make_par("This is the first paragraph of the manual."));
      chap->content.push_back(make_par("And here is a second."));
    }

    {
      auto chap = make<dex::Sectioning>(dex::Sectioning::Chapter, "Chapter 2");
      part->content.push_back(chap);

      chap->content.push_back(make_par("Warning! Skip this chapter."));
    }
  }

  {
    auto part = make<dex::Sectioning>(dex::Sectioning::Part, "Second part");
    man->content.push_back(part);

    {
      auto chap = make<dex::Sectioning>(dex::Sectioning::Chapter, "Chapter 3");
      part->content.push_back(chap);

      chap->content.push_back(make<dom::Image>("img.jpg"));
    }
  }

  return model;
}

void TestDexOutput::jsonExport()
{
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(example_prog_with_class());

    json::Object jexport = dex::JsonExport::serialize(*model).toObject();

    QVERIFY(jexport.data().size() == 1);

    jexport = jexport["program"]["global_namespace"].toObject();

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);
    QVERIFY(jexport.data().at("entities").at(0)["name"] == "vector");
  }
  
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(example_prog_with_fun());

    json::Object jexport = dex::JsonExport::serialize(*model).toObject();
    jexport = jexport["program"]["global_namespace"].toObject();

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);
    QVERIFY(jexport.data().at("entities").at(0)["name"] == "getenv");

    jexport = jexport.data().at("entities").at(0).toObject();
    QVERIFY(jexport["parameters"].at(0)["documentation"] == "name of the environment variable");

    jexport = jexport["documentation"].toObject();
    QVERIFY(jexport["since"] == "C++98");
    QVERIFY(jexport["returns"] == "value of environment variable");
  }

  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(example_prog_with_var());

    json::Object jexport = dex::JsonExport::serialize(*model).toObject();
    jexport = jexport["program"]["global_namespace"].toObject();

    QVERIFY(jexport.data().size() == 3);
    QVERIFY(jexport.data().at("entities").length() == 1);
    QVERIFY(jexport.data().at("entities").toArray().length() == 1);
    QVERIFY(jexport.data().at("entities").at(0)["name"] == "pi");
    QVERIFY(jexport.data().at("entities").at(0)["vartype"] == "double");

    jexport = jexport.data().at("entities").at(0)["documentation"].toObject();
    QVERIFY(jexport["description"].at(0)["text"] == "This mathematical constant is roughly equal to 3.");
  }
}

void TestDexOutput::jsonExportManual()
{
  auto model = std::make_shared<dex::Model>();
  auto man = std::make_shared<dex::Manual>("The manual");
  model->manuals().push_back(man);

  auto sec = std::make_shared<dex::Sectioning>(dex::Sectioning::Part, "Part 1");
  man->content.push_back(sec);

  sec->content.push_back(make_par("Hello World!"));

  json::Object jexport = dex::JsonExport::serialize(*model).toObject();

  QVERIFY(jexport.data().size() == 1);

  jexport = jexport["manuals"][0].toObject();

  QVERIFY(jexport.data().at("title").toString() == "The manual");
  QVERIFY(jexport.data().at("content").toArray().length() == 1);

  jexport = jexport["content"][0].toObject();
  QVERIFY(jexport.data().at("depth").toString() == "part");
  QVERIFY(jexport.data().at("name").toString() == "Part 1");

  QVERIFY(jexport.data().at("content").toArray().length() == 1);
  jexport = jexport["content"][0].toObject();
  QVERIFY(jexport["text"].toString() == "Hello World!");
}

void TestDexOutput::markdownExport()
{
  {
    auto model = std::make_shared<dex::Model>();
    model->setProgram(example_prog_with_class());

    dex::MarkdownExport md_export;

    md_export.dump(model, QDir::current());

    std::string content = dex::file_utils::read_all("classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n**Brief:** sequence container that encapsulates dynamic size arrays\n\n"
      "## Detailed description\n\nThe elements are stored contiguously, ...\n\n"
      "The storage of the vector is handled automatically, ...\n\n"
      "## Members documentation\n\n";

    QVERIFY(content == expected);
  }

  {
    auto model = example_prog_with_class_image_description();

    dex::MarkdownExport md_export;

    md_export.dump(model, QDir::current());

    std::string content = dex::file_utils::read_all("classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n"
      "## Detailed description\n\n![image](test.jpg)\n\n"
      "## Members documentation\n\n";

    QVERIFY(content == expected);
  }

  {
    auto model = example_prog_with_class_list_description();

    dex::MarkdownExport md_export;

    md_export.dump(model, QDir::current());

    std::string content = dex::file_utils::read_all("classes/vector.md");

    const std::string expected =
      "\n# vector Class\n\n"
      "## Detailed description\n\n- first item\n- second item\n\n"
      "## Members documentation\n\n";

    QVERIFY(content == expected);
  }
}

void TestDexOutput::markdownExportManual()
{
  auto model = example_manual();

  dex::MarkdownExport md_export;

  md_export.dump(model, QDir::current());

  std::string content = dex::file_utils::read_all("manuals/The manual.md");

  const std::string expected =
    "\n"
    "# The manual\n"
    "\n"
    "# First part\n"
    "\n"
    "## Chapter 1\n"
    "\n"
    "This is the first paragraph of the manual.\n"
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

  QVERIFY(content == expected);
}
