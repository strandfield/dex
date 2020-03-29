// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-input.h"

#include "dex/model/model.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-machine.h"

#include "dex/input/function-caller.h"
#include "dex/input/conditional-evaluator.h"
#include "dex/input/document-writer.h"

#include <cxx/class.h>
#include <cxx/documentation.h>
#include <cxx/function.h>
#include <cxx/namespace.h>

#include <dom/list.h>
#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <QFile>

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

void TestDexInput::argumentParsing()
{
  dex::ParserMachine machine;
  dex::FunctionCaller parser{ machine };

  write_cs(parser, "p@rseint", "c@ll", "f@@");
  write_chars(parser, "123 ");

  QVERIFY(parser.hasPendingCall());
  QVERIFY(parser.call().function == "f@@");
  QVERIFY(parser.call().arguments.size() == 1);
  QVERIFY(std::holds_alternative<int>(parser.call().arguments.front()));
  QVERIFY(std::get<int>(parser.call().arguments.front()) == 123);

  parser.clearPendingCall();
    
  write_cs(parser, "p@rsebool", "p@rseword", "c@ll", "b@r");
  write_chars(parser, "1 hello ");

  QVERIFY(parser.hasPendingCall());
  QVERIFY(parser.call().function == "b@r");
  QVERIFY(parser.call().arguments.size() == 2);
  QVERIFY(std::holds_alternative<bool>(parser.call().arguments.front()));
  QVERIFY(std::get<bool>(parser.call().arguments.front()));
  QVERIFY(std::holds_alternative<std::string>(parser.call().arguments.back()));
  QVERIFY(std::get<std::string>(parser.call().arguments.back()) == "hello");

  parser.clearPendingCall();

  write_cs(parser, "p@rseword", "p@rseline", "c@ll", "p@r@m");
  write_chars(parser, "there General Kenobi!");
  parser.write(tex::parsing::CharacterToken{ '\n', tex::parsing::CharCategory::Active });

  QVERIFY(parser.hasPendingCall());
  QVERIFY(parser.call().function == "p@r@m");
  QVERIFY(parser.call().arguments.size() == 2);
  QVERIFY(std::holds_alternative<std::string>(parser.call().arguments.front()));
  QVERIFY(std::get<std::string>(parser.call().arguments.front()) == "there");
  QVERIFY(std::holds_alternative<std::string>(parser.call().arguments.back()));
  QVERIFY(std::get<std::string>(parser.call().arguments.back()) == "General Kenobi!");

  parser.clearPendingCall();

  write_cs(parser, "p@rseoptions", "c@ll", "@pts");
  write_chars(parser, "[standalone, key=value]");

  QVERIFY(parser.hasPendingCall());
  QVERIFY(parser.call().function == "@pts");
  QVERIFY(parser.call().arguments.empty());
  QVERIFY(parser.call().options.size() == 2);
  QVERIFY(std::get<std::string>(parser.call().options.at("")) == "standalone");
  QVERIFY(std::get<std::string>(parser.call().options.at("key")) == "value");
}

void TestDexInput::conditionalEvaluator()
{
  tex::parsing::Registers registers;
  dex::InputStream istream;
  dex::ConditionalEvaluator parser{ registers, istream };

  istream.inject("{[");

  parser.write(tok('a'));
  QVERIFY(parser.output().size() == 1);
  parser.write(tok("c@ll"));
  QVERIFY(parser.output().size() == 2);

  parser.write(tok("testleftbr@ce"));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(registers.br);

  QVERIFY(istream.readChar() == '{');

  parser.write(tok("testnextch@r"));
  parser.write(tok(']'));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(!registers.br);

  parser.write(tok("testnextch@r"));
  parser.write(tok('['));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(registers.br);
}

void TestDexInput::documentWriterParagraph()
{
  dex::DocumentWriter writer;

  writer.startParagraph();

  writer.paragraph().writeStyledText("code", "std::vector");
  writer.write(" is a sequence container that encapsulates dynamic size arrays");
  writer.write('.');
  writer.endParagraph();

  writer.beginSinceBlock("C++03");
  writer.write("The elements are stored contiguously, ");
  writer.paragraph().writeLink("#more", "...");
  writer.endSinceBlock();

  QVERIFY(writer.output().size() == 2);

  QVERIFY(writer.output().front()->is<dom::Paragraph>());
  QVERIFY(writer.output().back()->is<dom::Paragraph>());

  auto par = std::static_pointer_cast<dom::Paragraph>(writer.output().front());
  QVERIFY(par->text() == "std::vector is a sequence container that encapsulates dynamic size arrays.");
  QVERIFY(par->metadata().size() == 1);
  QVERIFY(par->metadata().front()->is<dom::TextStyle>());
  QVERIFY(std::static_pointer_cast<dom::TextStyle>(par->metadata().front())->style() == "code");

  par = std::static_pointer_cast<dom::Paragraph>(writer.output().back());
  QVERIFY(par->text() == "The elements are stored contiguously, ...");
  QVERIFY(par->metadata().size() == 2);
  QVERIFY(par->metadata().front()->is<dex::Since>());
  QVERIFY(par->metadata().front()->range().text() == "The elements are stored contiguously, ...");
  QVERIFY(par->metadata().back()->is<dom::Link>());
  QVERIFY(par->metadata().back()->range().text() == "...");
}

void TestDexInput::documentWriterList()
{
  auto li = []() -> dex::FunctionCall {
    dex::FunctionCall ret;
    ret.function = dex::Functions::LI;
    return ret;
  };

  dex::DocumentWriter writer;

  writer.startList();
  writer.handle(li());
  writer.write("List item number 1");
  writer.handle(li());
  writer.write("Number 2");
  writer.endList();

  QVERIFY(writer.output().size() == 1);
  QVERIFY(writer.output().at(0)->is<dom::List>());

  auto list = std::static_pointer_cast<dom::List>(writer.output().at(0));

  QVERIFY(list->items.size() == 2);

  QVERIFY(list->items.front()->content.size() == 1);
  QVERIFY(list->items.back()->content.size() == 1);

  QVERIFY(list->items.front()->content.front()->is<dom::Paragraph>());

  auto par = std::static_pointer_cast<dom::Paragraph>(list->items.front()->content.front());
  QVERIFY(par->text() == "List item number 1");

  par = std::static_pointer_cast<dom::Paragraph>(list->items.back()->content.front());
  QVERIFY(par->text() == "Number 2");
}

void TestDexInput::parserMachineList()
{
  dex::ParserMachine parser;

  QFile file{ "test.cpp" };
  QVERIFY(file.open(QIODevice::WriteOnly));

  file.write(
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

  file.close();

  parser.process(QFileInfo{ "test.cpp" });

  QFile::remove("test.cpp");

  std::shared_ptr<cxx::Namespace> ns = parser.output()->program()->globalNamespace();

  QVERIFY(ns->entities().size() > 0);
  QVERIFY(ns->entities().front()->is<cxx::Class>());
  auto vec = std::static_pointer_cast<cxx::Class>(ns->entities().front());
  QVERIFY(vec->documentation()->is<dex::ClassDocumentation>());
  auto doc = std::static_pointer_cast<dex::ClassDocumentation>(vec->documentation());
  QVERIFY(doc->description().size() == 1);
  QVERIFY(doc->description().front()->is<dom::List>());

  auto lst = std::static_pointer_cast<dom::List>(doc->description().front());
  
  QVERIFY(lst->items.size() == 2);
  QVERIFY(lst->items.back()->content.size() == 2);
  QVERIFY(lst->items.back()->content.back()->is<dom::List>());

  lst = std::static_pointer_cast<dom::List>(lst->items.back()->content.back());
  QVERIFY(lst->items.size() == 1);
  QVERIFY(lst->items.front()->content.size() == 1);
  QVERIFY(lst->items.front()->content.front()->is<dom::Paragraph>());

  auto par = std::static_pointer_cast<dom::Paragraph>(lst->items.front()->content.front());
  QVERIFY(par->text() == "nested item ");
}

void TestDexInput::parserMachineClass()
{
  dex::ParserMachine parser;

  QFile file{ "test.cpp" };
  QVERIFY(file.open(QIODevice::WriteOnly));

  file.write(
    "// The following block is recognized by dex\n"
    "/*!\n"
    " * \\class vector\n"
    " * \\brief sequence container that encapsulates dynamic size arrays\n"
    " *\n"
    " * The elements are stored contiguously, ...\n"
    " */\n"
  );

  file.close();

  parser.process(QFileInfo{ "test.cpp" });

  std::shared_ptr<cxx::Namespace> ns = parser.output()->program()->globalNamespace();

  QVERIFY(ns->entities().size() > 0);
  QVERIFY(ns->entities().front()->is<cxx::Class>());
  auto vec = std::static_pointer_cast<cxx::Class>(ns->entities().front());
  QVERIFY(vec->name() == "vector");
  QVERIFY(vec->documentation()->is<dex::ClassDocumentation>());
  auto doc = std::static_pointer_cast<dex::ClassDocumentation>(vec->documentation());
  QVERIFY(doc->brief().value() == "sequence container that encapsulates dynamic size arrays");
  QVERIFY(doc->description().size() == 1);
  QVERIFY(doc->description().front()->is<dom::Paragraph>());
  auto paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().front());
  QVERIFY(paragraph->text() == "The elements are stored contiguously, ... ");

  QFile::remove("test.cpp");
}

void TestDexInput::parserMachineFunction()
{
  dex::ParserMachine parser;

  QFile file{ "test.cpp" };
  QVERIFY(file.open(QIODevice::WriteOnly));

  file.write(
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

  file.close();

  parser.process(QFileInfo{ "test.cpp" });

  std::shared_ptr<cxx::Namespace> ns = parser.output()->program()->globalNamespace();

  QVERIFY(ns->entities().size() > 0);
  QVERIFY(ns->entities().front()->is<cxx::Function>());
  auto getenv = std::static_pointer_cast<cxx::Function>(ns->entities().front());
  QVERIFY(getenv->name() == "char* getenv(const char* env_var);");
  QVERIFY(getenv->documentation()->is<dex::FunctionDocumentation>());
  auto doc = std::static_pointer_cast<dex::FunctionDocumentation>(getenv->documentation());
  QVERIFY(doc->brief().value() == "get value from environment variables");
  QVERIFY(doc->since().value().version() == "C++98");
  QVERIFY(doc->parameters().size() == 1);
  QVERIFY(doc->parameters().front() == "name of the environment variable");
  QVERIFY(doc->returnValue().value_or("") == "value of environment variable");
  QVERIFY(doc->description().size() == 2);
  QVERIFY(doc->description().front()->is<dom::Paragraph>());
  QVERIFY(doc->description().back()->is<dom::Paragraph>());
  auto paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().front());
  QVERIFY(paragraph->text() == "Searches the environment list provided by the host environment... ");
  paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().back());
  QVERIFY(paragraph->text() == "Modifying the string returned by getenv invokes undefined behavior. ");

  QFile::remove("test.cpp");
}
