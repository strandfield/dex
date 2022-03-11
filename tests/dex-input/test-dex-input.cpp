// Copyright (C) 2019-2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-machine.h"

#include "dex/input/function-caller.h"
#include "dex/input/conditional-evaluator.h"
#include "dex/input/document-writer.h"

#include "dex/common/file-utils.h"

#include "catch.hpp"

tex::parsing::Token tok(std::string str)
{
  return tex::parsing::Token{ std::move(str) };
}

tex::parsing::Token tok(char c)
{
  return tex::parsing::Token{ tex::parsing::CharacterToken{c, tex::parsing::Lexer::DefaultCatCodes[static_cast<unsigned char>(c)] } };
}

void write_chars(dex::FunctionCaller& parser, const std::string& str)
{
  for (char c : str)
  {
    parser.write(tok(c));
  }
}

void write_chars_lexer(dex::FunctionCaller& parser, const std::string& str)
{
  for (char c : str)
  {
    parser.machine().lexer().write(c);

    if (!parser.machine().lexer().output().empty())
    {
      tex::parsing::Token tok = tex::parsing::read(parser.machine().lexer().output());
      parser.write(std::move(tok));
    }
  }
}

void write_cs(dex::FunctionCaller& parser, const std::string& cs)
{
  parser.write(tok(cs));
}

template<typename...Str>
void write_cs(dex::FunctionCaller& parser, const std::string& cs, const Str& ...strs)
{
  parser.write(tok(cs));
  write_cs(parser, strs...);
}

TEST_CASE("Argument parsing works", "[input]")
{
  dex::ParserMachine machine;
  dex::FunctionCaller parser{ machine };

  write_cs(parser, "p@rseint", "c@ll", "f@@");
  write_chars(parser, "123 ");

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "f@@");
  REQUIRE(parser.call().arguments.size() == 1);
  REQUIRE(std::holds_alternative<int>(parser.call().arguments.front()));
  REQUIRE(std::get<int>(parser.call().arguments.front()) == 123);

  parser.clearPendingCall();
    
  write_cs(parser, "p@rsebool", "p@rseword", "c@ll", "b@r");
  write_chars(parser, "1 hello ");

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "b@r");
  REQUIRE(parser.call().arguments.size() == 2);
  REQUIRE(std::holds_alternative<bool>(parser.call().arguments.front()));
  REQUIRE(std::get<bool>(parser.call().arguments.front()));
  REQUIRE(std::holds_alternative<std::string>(parser.call().arguments.back()));
  REQUIRE(std::get<std::string>(parser.call().arguments.back()) == "hello");

  parser.clearPendingCall();

  write_cs(parser, "p@rseword", "p@rseline", "c@ll", "p@r@m");
  write_chars(parser, "there General Kenobi!");
  parser.write(tex::parsing::CharacterToken{ '\n', tex::parsing::CharCategory::Active });

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "p@r@m");
  REQUIRE(parser.call().arguments.size() == 2);
  REQUIRE(std::holds_alternative<std::string>(parser.call().arguments.front()));
  REQUIRE(std::get<std::string>(parser.call().arguments.front()) == "there");
  REQUIRE(std::holds_alternative<std::string>(parser.call().arguments.back()));
  REQUIRE(std::get<std::string>(parser.call().arguments.back()) == "General Kenobi!");

  parser.clearPendingCall();

  write_cs(parser, "p@rseoptions", "c@ll", "@pts");
  write_chars(parser, "[standalone, key=value]");

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "@pts");
  REQUIRE(parser.call().arguments.empty());
  REQUIRE(parser.call().options.size() == 2);
  REQUIRE(std::get<std::string>(parser.call().options.at("")) == "standalone");
  REQUIRE(std::get<std::string>(parser.call().options.at("key")) == "value");

  parser.clearPendingCall();

  write_cs(parser, "p@rseword", "c@ll", "im@ge");
  write_chars(parser, "{test-image.jpg}");

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "im@ge");
  REQUIRE(parser.call().arguments.size() == 1);
  REQUIRE(std::get<std::string>(parser.call().arguments.at(0)) == "test-image.jpg");

  parser.clearPendingCall();

  write_cs(parser, "p@rseline", "c@ll", "foo");
  write_chars_lexer(parser, "{This one extends after\n the end of the line}");

  REQUIRE(parser.hasPendingCall());
  REQUIRE(parser.call().function == "foo");
  REQUIRE(parser.call().arguments.size() == 1);
  REQUIRE(std::get<std::string>(parser.call().arguments.at(0)) == "This one extends after the end of the line");
}

TEST_CASE("Conditions are correctly evaluated", "[input]")
{
  dex::InputStream istream;
  tex::parsing::Lexer lexer;
  tex::parsing::Preprocessor preproc;
  dex::ConditionalEvaluator parser{ istream, lexer, preproc };

  istream.inject("{[");

  parser.write(tok('a'));
  REQUIRE(parser.output().size() == 1);
  parser.write(tok("c@ll"));
  REQUIRE(parser.output().size() == 2);

  parser.write(tok("testleftbr@ce"));
  REQUIRE(parser.output().size() == 2);
  REQUIRE(preproc.br);

  REQUIRE(istream.readChar() == '{');

  parser.write(tok("testnextch@r"));
  parser.write(tok(']'));
  REQUIRE(parser.output().size() == 2);
  REQUIRE(!preproc.br);

  parser.write(tok("testnextch@r"));
  parser.write(tok('['));
  REQUIRE(parser.output().size() == 2);
  REQUIRE(preproc.br);
}

TEST_CASE("Paragraphs can be written", "[input]")
{
  dex::DocumentWriter writer;

  //writer.startParagraph();

  writer.c("std::vector");
  writer.write(" is a sequence container that encapsulates dynamic size arrays");
  writer.write('.');
  writer.par();

  writer.beginSinceBlock("C++03");
  writer.write("The elements are stored contiguously, ");
  writer.paragraphWriter().writeLink("#more", "...");
  writer.endSinceBlock();

  REQUIRE(writer.output()->childNodes().size() == 4);

  REQUIRE(writer.output()->childNodes().at(0)->is<dex::Paragraph>());
  REQUIRE(writer.output()->childNodes().at(2)->is<dex::Paragraph>());

  auto par = std::static_pointer_cast<dex::Paragraph>(writer.output()->childNodes().at(0));
  REQUIRE(par->text() == "std::vector is a sequence container that encapsulates dynamic size arrays.");
  REQUIRE(par->metadata().size() == 1);
  REQUIRE(par->metadata().front()->is<dex::TextStyle>());
  REQUIRE(std::static_pointer_cast<dex::TextStyle>(par->metadata().front())->style() == "code");

  par = std::static_pointer_cast<dex::Paragraph>(writer.output()->childNodes().at(2));
  REQUIRE(par->text() == "The elements are stored contiguously, ...");
  REQUIRE(par->metadata().size() == 1);
  REQUIRE(par->metadata().back()->is<dex::Link>());
  REQUIRE(par->metadata().back()->range().text() == "...");
}

TEST_CASE("Lists can be created", "[input]")
{
  dex::DocumentWriter writer;

  writer.list();
  writer.li({}, {});
  writer.write("List item number 1");
  writer.li({}, {});
  writer.write("Number 2");
  writer.endlist();

  REQUIRE(writer.output()->childNodes().size() == 1);
  REQUIRE(writer.output()->childNodes().at(0)->is<dex::List>());

  auto list = std::static_pointer_cast<dex::List>(writer.output()->childNodes().at(0));

  REQUIRE(list->items.size() == 2);

  REQUIRE(list->items.front()->childNodes().size() == 1);
  REQUIRE(list->items.back()->childNodes().size() == 1);

  REQUIRE(list->items.front()->childNodes().front()->is<dex::Paragraph>());

  auto par = std::static_pointer_cast<dex::Paragraph>(list->items.front()->childNodes().front());
  REQUIRE(par->text() == "List item number 1");

  par = std::static_pointer_cast<dex::Paragraph>(list->items.back()->childNodes().front());
  REQUIRE(par->text() == "Number 2");
}

TEST_CASE("Lists can be inserted in a document", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "/*!\n"
    " * \\class vector\n"
    " *\n"
    " * \\list\n"
    " *   \\li first item\n"
    " *   \\li second item:\n"
    " *     \\list\n"
    " *       \\li nested item\n"
    " *     \\endlist\n"
    " * \\endlist\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Class>());
  auto vec = std::static_pointer_cast<dex::Class>(ns->entities.front());
  REQUIRE(vec->description->childNodes().size() == 1);
  REQUIRE(vec->description->childNodes().front()->is<dex::List>());

  auto lst = std::static_pointer_cast<dex::List>(vec->description->childNodes().front());

  REQUIRE(lst->items.size() == 2);
  REQUIRE(lst->items.back()->childNodes().size() == 2);
  REQUIRE(lst->items.back()->childNodes().back()->is<dex::List>());

  lst = std::static_pointer_cast<dex::List>(lst->items.back()->childNodes().back());
  REQUIRE(lst->items.size() == 1);
  REQUIRE(lst->items.front()->childNodes().size() == 1);
  REQUIRE(lst->items.front()->childNodes().front()->is<dex::Paragraph>());

  auto par = std::static_pointer_cast<dex::Paragraph>(lst->items.front()->childNodes().front());
  REQUIRE(par->text() == "nested item");
}

TEST_CASE("Images can be inserted in a document", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "/*!\n"
    " * \\class vector\n"
    " *\n"
    " * This is a first paragraph.\n"
    " * \\image[width=66]{test.png}\n"
    " * This is a second paragraph.\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Class>());
  auto vec = std::static_pointer_cast<dex::Class>(ns->entities.front());
  REQUIRE(vec->description->childNodes().size() == 3);

  REQUIRE(vec->description->childNodes().at(0)->is<dex::Paragraph>());
  auto par = std::static_pointer_cast<dex::Paragraph>(vec->description->childNodes().at(0));
  REQUIRE(par->text() == "This is a first paragraph.");

  REQUIRE(vec->description->childNodes().at(1)->is<dex::Image>());
  auto img = std::static_pointer_cast<dex::Image>(vec->description->childNodes().at(1));
  REQUIRE(img->src == "test.png");
  REQUIRE(img->width == 66);

  REQUIRE(vec->description->childNodes().at(2)->is<dex::Paragraph>());
  par = std::static_pointer_cast<dex::Paragraph>(vec->description->childNodes().at(2));
  REQUIRE(par->text() == "This is a second paragraph.");
}

TEST_CASE("Testing 'class' block", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "// The following block is recognized by dex\n"
    "/*!\n"
    " * \\class vector\n"
    " * \\brief sequence container that encapsulates dynamic size arrays\n"
    " *\n"
    " * The elements are stored contiguously, ...\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Class>());
  auto vec = std::static_pointer_cast<dex::Class>(ns->entities.front());
  REQUIRE(vec->name == "vector");
  REQUIRE(vec->brief.value() == "sequence container that encapsulates dynamic size arrays");
  REQUIRE(vec->description->childNodes().size() == 1);
  REQUIRE(vec->description->childNodes().front()->is<dex::Paragraph>());
  auto paragraph = std::static_pointer_cast<dex::Paragraph>(vec->description->childNodes().front());
  REQUIRE(paragraph->text() == "The elements are stored contiguously, ...");
}

TEST_CASE("Testing 'fn' block", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "// The following block is recognized by dex\n"
    "/*!\n"
    " * \\fn char* getenv(const char* env_var);\n"
    " * \\brief get value from environment variables\n"
    " * \\param name of the environment variable\n"
    " * \\returns value of environment variable\n"
    " * \\since C++98\n"
    " *\n"
    " * Searches the environment list provided by the host environment...\n"
    " *\n"
    " * Modifying the string returned by getenv invokes undefined behavior.\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Function>());
  auto getenv = std::static_pointer_cast<dex::Function>(ns->entities.front());
  REQUIRE(getenv->name == "getenv");
  REQUIRE(getenv->brief.value() == "get value from environment variables");
  REQUIRE(getenv->since.value().version() == "C++98");
  REQUIRE(getenv->parameters.size() == 1);
  auto funparam = getenv->parameters.front();
  REQUIRE(funparam->brief == "name of the environment variable");
  REQUIRE(getenv->return_type.brief.value_or("") == "value of environment variable");
  REQUIRE(getenv->description->childNodes().size() == 2);
  REQUIRE(getenv->description->childNodes().front()->is<dex::Paragraph>());
  REQUIRE(getenv->description->childNodes().back()->is<dex::Paragraph>());
  auto paragraph = std::static_pointer_cast<dex::Paragraph>(getenv->description->childNodes().front());
  REQUIRE(paragraph->text() == "Searches the environment list provided by the host environment...");
  paragraph = std::static_pointer_cast<dex::Paragraph>(getenv->description->childNodes().back());
  REQUIRE(paragraph->text() == "Modifying the string returned by getenv invokes undefined behavior.");
}

TEST_CASE("Testing 'enum' block", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "/*!\n"
    " * \\enum Corner\n"
    " * \\brief describes a corner\n"
    " *\n"
    " * This is not that useful.\n"
    " *\n"
    " * \\value TopLeft the top left corner\n"
    " * \\value TopRight the top right corner\n"
    " * \\value BottomLeft the bottom left corner\n"
    " * \\value BottomRight the bottom right corner\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Enum>());
  auto corner = std::static_pointer_cast<dex::Enum>(ns->entities.front());
  REQUIRE(corner->name == "Corner");
  REQUIRE(corner->values.size() == 4);
  REQUIRE(corner->brief.value() == "describes a corner");
  REQUIRE(corner->description->childNodes().size() == 1);
  REQUIRE(corner->description->childNodes().front()->is<dex::Paragraph>());
  auto paragraph = std::static_pointer_cast<dex::Paragraph>(corner->description->childNodes().front());
  REQUIRE(paragraph->text() == "This is not that useful.");

  auto top_left = corner->values.at(0);
  REQUIRE(top_left->name == "TopLeft");
  REQUIRE(top_left->description->childNodes().size() == 1);
  REQUIRE(top_left->description->childNodes().front()->is<dex::Paragraph>());
  paragraph = std::static_pointer_cast<dex::Paragraph>(top_left->description->childNodes().front());
  REQUIRE(paragraph->text() == "the top left corner");

  auto bottom_right = corner->values.at(3);
  REQUIRE(bottom_right->name == "BottomRight");
  REQUIRE(bottom_right->description->childNodes().size() == 1);
  REQUIRE(bottom_right->description->childNodes().front()->is<dex::Paragraph>());
  paragraph = std::static_pointer_cast<dex::Paragraph>(bottom_right->description->childNodes().front());
  REQUIRE(paragraph->text() == "the bottom right corner");
}

TEST_CASE("Testing 'variable' block", "[input]")
{
  dex::ParserMachine parser;

  dex::file_utils::write_file("test.cpp",
    "/*!\n"
    " * \\variable std::string name = \"dex\";\n"
    " * \\brief the name of the program\n"
    " * \\since 2020\n"
    " * Stores the name of the program.\n"
    " */\n"
  );

  parser.process(std::filesystem::path("test.cpp"));

  dex::file_utils::remove("test.cpp");

  std::shared_ptr<dex::Namespace> ns = parser.output()->program()->globalNamespace();

  REQUIRE(ns->entities.size() > 0);
  REQUIRE(ns->entities.front()->is<dex::Variable>());
  auto variable = std::static_pointer_cast<dex::Variable>(ns->entities.front());
  REQUIRE(variable->name == "name");
  REQUIRE(variable->brief.value() == "the name of the program");
  REQUIRE(variable->since.value().version() == "2020");
 
  REQUIRE(variable->description->childNodes().size() == 1);
  REQUIRE(variable->description->childNodes().front()->is<dex::Paragraph>());
  auto paragraph = std::static_pointer_cast<dex::Paragraph>(variable->description->childNodes().front());
  REQUIRE(paragraph->text() == "Stores the name of the program.");
}

TEST_CASE("Testing 'manual' block", "[input]")
{
  {
    dex::file_utils::write_file("test.dex",
      "\n"
      "\\manual Manual's title\n"
      "\n"
      "\\part First part\n"
      "\n"
      "\\input{toast}"
      "\n"
      "\\chapter{Second chapter}\n"
      "This is the content of the second chapter.\n"
      "\n"
    );
  }

  {
    dex::file_utils::write_file("toast.dex",
      "\\chapter First chapter\n"
      "This is the content of the first chapter.\n"
    );
  }

  dex::ParserMachine parser;
  parser.process(std::filesystem::path("test.dex"));

  dex::file_utils::remove("test.dex");
  dex::file_utils::remove("toast.dex");

  REQUIRE(parser.output()->documents.size() == 1);

  std::shared_ptr<dex::Document> man = parser.output()->documents.front();

  REQUIRE(man->childNodes().size() == 1);
  REQUIRE(man->childNodes().front()->is<dex::Sectioning>());

  auto part = std::dynamic_pointer_cast<dex::Sectioning>(man->childNodes().front());
  REQUIRE(part->depth == dex::Sectioning::Part);
  REQUIRE(part->content.size() == 2);

  auto first_chapter = std::dynamic_pointer_cast<dex::Sectioning>(part->content.front());
  REQUIRE((first_chapter != nullptr && first_chapter->depth == dex::Sectioning::Chapter));

  {
    REQUIRE(first_chapter->name == "First chapter");
    REQUIRE(first_chapter->content.size() == 1);

    auto par = std::dynamic_pointer_cast<dex::Paragraph>(first_chapter->content.front());
    REQUIRE((par != nullptr && par->text() == "This is the content of the first chapter."));
  }

  auto second_chapter = std::dynamic_pointer_cast<dex::Sectioning>(part->content.back());
  REQUIRE((second_chapter != nullptr && second_chapter->depth == dex::Sectioning::Chapter));

  {
    REQUIRE(second_chapter->name == "Second chapter");
    REQUIRE(second_chapter->content.size() == 1);

    auto par = std::dynamic_pointer_cast<dex::Paragraph>(second_chapter->content.front());
    REQUIRE((par != nullptr && par->text() == "This is the content of the second chapter."));
  }
}
