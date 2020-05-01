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
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>

#include <dom/image.h>
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

  parser.clearPendingCall();

  write_cs(parser, "p@rseword", "c@ll", "im@ge");
  write_chars(parser, "{test-image.jpg}");

  QVERIFY(parser.hasPendingCall());
  QVERIFY(parser.call().function == "im@ge");
  QVERIFY(parser.call().arguments.size() == 1);
  QVERIFY(std::get<std::string>(parser.call().arguments.at(0)) == "test-image.jpg");
}

void TestDexInput::conditionalEvaluator()
{
  dex::InputStream istream;
  tex::parsing::Lexer lexer;
  tex::parsing::Preprocessor preproc;
  dex::ConditionalEvaluator parser{ istream, lexer, preproc };

  istream.inject("{[");

  parser.write(tok('a'));
  QVERIFY(parser.output().size() == 1);
  parser.write(tok("c@ll"));
  QVERIFY(parser.output().size() == 2);

  parser.write(tok("testleftbr@ce"));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(preproc.br);

  QVERIFY(istream.readChar() == '{');

  parser.write(tok("testnextch@r"));
  parser.write(tok(']'));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(!preproc.br);

  parser.write(tok("testnextch@r"));
  parser.write(tok('['));
  QVERIFY(parser.output().size() == 2);
  QVERIFY(preproc.br);
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

void TestDexInput::parserMachineImage()
{
  dex::ParserMachine parser;

  QFile file{ "test.cpp" };
  QVERIFY(file.open(QIODevice::WriteOnly));

  file.write(
    "/*!\n"
    " * \\class vector\n"
    " *\n"
    " * This is a first paragraph.\n"
    " * \\image[width=66]{test.png}\n"
    " * This is a second paragraph.\n"
    " */\n"
  );

  file.close();

  parser.process(QFileInfo{ "test.cpp" });

  QFile::remove("test.cpp");

  std::shared_ptr<cxx::Namespace> ns = parser.output()->program()->globalNamespace();

  QVERIFY(ns->entities().size() > 0);
  QVERIFY(ns->entities().front()->is<cxx::Class>());
  auto vec = std::static_pointer_cast<cxx::Class>(ns->entities().front());
  QVERIFY(std::dynamic_pointer_cast<dex::ClassDocumentation>(vec->documentation()) != nullptr);
  auto doc = std::static_pointer_cast<dex::ClassDocumentation>(vec->documentation());
  QVERIFY(doc->description().size() == 3);

  QVERIFY(doc->description().at(0)->is<dom::Paragraph>());
  auto par = std::static_pointer_cast<dom::Paragraph>(doc->description().at(0));
  QVERIFY(par->text() == "This is a first paragraph.");

  QVERIFY(doc->description().at(1)->is<dom::Image>());
  auto img = std::static_pointer_cast<dom::Image>(doc->description().at(1));
  QVERIFY(img->src == "test.png");
  QVERIFY(img->width == 66);

  QVERIFY(doc->description().at(2)->is<dom::Paragraph>());
  par = std::static_pointer_cast<dom::Paragraph>(doc->description().at(2));
  QVERIFY(par->text() == "This is a second paragraph.");
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
  QVERIFY(std::dynamic_pointer_cast<dex::ClassDocumentation>(vec->documentation()) != nullptr);
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
  QVERIFY(par->text() == "nested item");
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
  QVERIFY(std::dynamic_pointer_cast<dex::ClassDocumentation>(vec->documentation()) != nullptr);
  auto doc = std::static_pointer_cast<dex::ClassDocumentation>(vec->documentation());
  QVERIFY(doc->brief().value() == "sequence container that encapsulates dynamic size arrays");
  QVERIFY(doc->description().size() == 1);
  QVERIFY(doc->description().front()->is<dom::Paragraph>());
  auto paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().front());
  QVERIFY(paragraph->text() == "The elements are stored contiguously, ...");

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
  QVERIFY(getenv->name() == "getenv");
  QVERIFY(std::dynamic_pointer_cast<dex::FunctionDocumentation>(getenv->documentation()) != nullptr);
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
  QVERIFY(paragraph->text() == "Searches the environment list provided by the host environment...");
  paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().back());
  QVERIFY(paragraph->text() == "Modifying the string returned by getenv invokes undefined behavior.");

  QFile::remove("test.cpp");
}

void TestDexInput::parserMachineEnum()
{
  dex::ParserMachine parser;

  QFile file{ "test.cpp" };
  QVERIFY(file.open(QIODevice::WriteOnly));

  file.write(
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

  file.close();

  parser.process(QFileInfo{ "test.cpp" });

  QFile::remove("test.cpp");

  std::shared_ptr<cxx::Namespace> ns = parser.output()->program()->globalNamespace();

  QVERIFY(ns->entities().size() > 0);
  QVERIFY(ns->entities().front()->is<cxx::Enum>());
  auto corner = std::static_pointer_cast<cxx::Enum>(ns->entities().front());
  QVERIFY(corner->name() == "Corner");
  QVERIFY(corner->values().size() == 4);
  QVERIFY(std::dynamic_pointer_cast<dex::EnumDocumentation>(corner->documentation()) != nullptr);
  auto doc = std::static_pointer_cast<dex::EnumDocumentation>(corner->documentation());
  QVERIFY(doc->brief().value() == "describes a corner");
  QVERIFY(doc->description().size() == 1);
  QVERIFY(doc->description().front()->is<dom::Paragraph>());
  auto paragraph = std::static_pointer_cast<dom::Paragraph>(doc->description().front());
  QVERIFY(paragraph->text() == "This is not that useful.");

  auto top_left = corner->values().at(0);
  auto valdoc = std::static_pointer_cast<dex::EnumValueDocumentation>(top_left->documentation());
  QVERIFY(top_left->name() == "TopLeft");
  QVERIFY(valdoc->description().size() == 1);
  QVERIFY(valdoc->description().front()->is<dom::Paragraph>());
  paragraph = std::static_pointer_cast<dom::Paragraph>(valdoc->description().front());
  QVERIFY(paragraph->text() == "the top left corner");

  auto bottom_right = corner->values().at(3);
  valdoc = std::static_pointer_cast<dex::EnumValueDocumentation>(bottom_right->documentation());
  QVERIFY(bottom_right->name() == "BottomRight");
  QVERIFY(valdoc->description().size() == 1);
  QVERIFY(valdoc->description().front()->is<dom::Paragraph>());
  paragraph = std::static_pointer_cast<dom::Paragraph>(valdoc->description().front());
  QVERIFY(paragraph->text() == "the bottom right corner");
}

void TestDexInput::modelPath()
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

  std::shared_ptr<dex::Model> model = parser.output();

  dex::Model::Node model_node = model->get({ {"program"}, {"global_namespace"}, {"entities", 0}, {"documentation"}, {"description", 0}, {"items", 0} });

  QVERIFY(std::holds_alternative<std::shared_ptr<dom::Node>>(model_node));

  auto dom_node = std::get<std::shared_ptr<dom::Node>>(model_node);

  QVERIFY(dom_node->is<dom::ListItem>());

  auto list_item = std::static_pointer_cast<dom::ListItem>(dom_node);
  auto par = std::static_pointer_cast<dom::Paragraph>(list_item->content.front());
  QVERIFY(par->text() == "first item");
}
