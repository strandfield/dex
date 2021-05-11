// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "model-examples.h"

#include "dex/model/since.h"
#include "dex/model/class-documentation.h"
#include "dex/model/function-documentation.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>
#include <cxx/program.h>
#include <cxx/variable.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <json-toolkit/stringify.h>

#include <iostream>

static std::shared_ptr<dex::Paragraph> make_par(const std::string& str)
{
  return std::make_shared<dex::Paragraph>(str);
}

template<typename T, typename...Args>
std::shared_ptr<T> make(Args&&... args)
{
  return std::make_shared<T>(std::forward<Args>(args)...);
}

namespace dex
{

namespace examples
{


std::shared_ptr<dex::Program> prog_with_class()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  doc->brief() = "sequence container that encapsulates dynamic size arrays"; 
  doc->description = std::make_shared<dex::Document>();
  doc->description->appendChild(make_par("The elements are stored contiguously, ..."));
  doc->description->appendChild(make_par("The storage of the vector is handled automatically, ..."));
  vector->documentation = doc;

  global->entities.push_back(vector);

  return prog;
}

std::shared_ptr<dex::Model> prog_with_class_image_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  auto img = make<dex::Image>("test.jpg");
  doc->description = std::make_shared<dex::Document>();
  doc->description->appendChild(img);
  vector->documentation = doc;

  global->entities.push_back(vector);

  model->setProgram(prog);

  return model;
}

std::shared_ptr<dex::Model> prog_with_class_list_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<cxx::Class>("vector", global);
  auto doc = std::make_shared<dex::ClassDocumentation>();
  auto list = make<dex::List>();
  auto listitem = make<dex::ListItem>();
  listitem->content.push_back(make<dex::Paragraph>("first item"));
  list->items.push_back(listitem);
  listitem = make<dex::ListItem>();
  listitem->content.push_back(make<dex::Paragraph>("second item"));
  list->items.push_back(listitem);
  doc->description = std::make_shared<dex::Document>();
  doc->description->appendChild(list);
  vector->documentation = doc;

  global->entities.push_back(vector);

  model->setProgram(prog);

  return model;
}

std::shared_ptr<dex::Program> prog_with_fun()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto getenv = std::make_shared<cxx::Function>("getenv", global);
  auto doc = std::make_shared<dex::FunctionDocumentation>();
  doc->brief() = "get value from environment variables";
  doc->returnValue() = "value of environment variable";
  doc->since() = dex::Since{ "C++98" };
  doc->description = std::make_shared<dex::Document>();
  doc->description->appendChild(make_par("Searches the environment list provided by the host environment..."));
  doc->description->appendChild(make_par("Modifying the string returned by getenv invokes undefined behavior."));
  getenv->documentation = doc;

  getenv->parameters.push_back(make<cxx::FunctionParameter>(cxx::Type("std::string"), "str"));
  getenv->parameters.front()->documentation = make<dex::FunctionParameterDocumentation>("name of the environment variable");

  global->entities.push_back(getenv);

  return prog;
}

std::shared_ptr<dex::Program> prog_with_var()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto variable = std::make_shared<cxx::Variable>(cxx::Type("double"), "pi", global);
  auto doc = std::make_shared<dex::VariableDocumentation>();
  doc->brief() = "the math constant pi";
  doc->description = std::make_shared<dex::Document>();
  doc->description->appendChild(make_par("This mathematical constant is roughly equal to 3."));
  variable->documentation = doc;

  global->entities.push_back(variable);

  return prog;
}

std::shared_ptr<dex::Program> prog_with_class_and_fun()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto complex = std::make_shared<cxx::Class>("complex", global);
  global->entities.push_back(complex);

  auto real = std::make_shared<cxx::Function>("real", complex);
  complex->members.emplace_back(real);

  return prog;
}

std::shared_ptr<dex::Model> manual()
{
  auto model = make<dex::Model>();

  auto man = make<dex::Manual>("The manual");
  model->documents.push_back(man);

  {
    auto part = make<dex::Sectioning>(dex::Sectioning::Part, "First part");
    man->appendChild(part);

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
    man->appendChild(part);

    {
      auto chap = make<dex::Sectioning>(dex::Sectioning::Chapter, "Chapter 3");
      part->content.push_back(chap);

      chap->content.push_back(make<dex::Image>("img.jpg"));
    }
  }

  return model;
}

} // namespace examples

} // namespace dex
