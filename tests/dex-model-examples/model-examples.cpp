// Copyright (C) 2020-2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "model-examples.h"

#include "dex/model/program.h"
#include "dex/model/since.h"

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

  auto vector = std::make_shared<dex::Class>("vector", global);
  vector->brief = "sequence container that encapsulates dynamic size arrays";
  vector->description = std::make_shared<dex::Document>();
  vector->description->appendChild(make_par("The elements are stored contiguously, ..."));
  vector->description->appendChild(make_par("The storage of the vector is handled automatically, ..."));

  global->entities.push_back(vector);

  return prog;
}

std::shared_ptr<dex::Model> prog_with_class_image_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<dex::Class>("vector", global);
  auto img = make<dex::Image>("test.jpg");
  vector->description = std::make_shared<dex::Document>();
  vector->description->appendChild(img);

  global->entities.push_back(vector);

  model->setProgram(prog);

  return model;
}

std::shared_ptr<dex::Model> prog_with_class_list_description()
{
  auto model = std::make_shared<dex::Model>();

  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto vector = std::make_shared<dex::Class>("vector", global);
  auto list = make<dex::List>();
  auto listitem = make<dex::ListItem>();
  listitem->content.push_back(make<dex::Paragraph>("first item"));
  list->items.push_back(listitem);
  listitem = make<dex::ListItem>();
  listitem->content.push_back(make<dex::Paragraph>("second item"));
  list->items.push_back(listitem);
  vector->description = std::make_shared<dex::Document>();
  vector->description->appendChild(list);

  global->entities.push_back(vector);

  model->setProgram(prog);

  return model;
}

std::shared_ptr<dex::Program> prog_with_fun()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto getenv = std::make_shared<dex::Function>("getenv", global);
  getenv->brief = "get value from environment variables";
  getenv->return_type.brief = "value of environment variable";
  getenv->since = dex::Since{ "C++98" };
  getenv->description = std::make_shared<dex::Document>();
  getenv->description->appendChild(make_par("Searches the environment list provided by the host environment..."));
  getenv->description->appendChild(make_par("Modifying the string returned by getenv invokes undefined behavior."));

  getenv->parameters.push_back(make<dex::FunctionParameter>(dex::Type("std::string"), "str"));
  getenv->parameters.front()->brief = "name of the environment variable";

  global->entities.push_back(getenv);

  return prog;
}

std::shared_ptr<dex::Program> prog_with_var()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto variable = std::make_shared<dex::Variable>(dex::Type("double"), "pi", global);
  variable->brief = "the math constant pi";
  variable->description = std::make_shared<dex::Document>();
  variable->description->appendChild(make_par("This mathematical constant is roughly equal to 3."));

  global->entities.push_back(variable);

  return prog;
}

std::shared_ptr<dex::Program> prog_with_class_and_fun()
{
  auto prog = std::make_shared<dex::Program>();
  auto global = prog->globalNamespace();

  auto complex = std::make_shared<dex::Class>("complex", global);
  global->entities.push_back(complex);

  auto real = std::make_shared<dex::Function>("real", complex);
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
