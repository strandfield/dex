// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer-frontend.h"

#include "dex/input/parser-errors.h"

#include <cassert>
#include <stdexcept>

namespace dex
{


DocumentWriterFrontend::DocumentWriterFrontend(DocumentWriter& writer)
  : m_writer(&writer)
{

}

void DocumentWriterFrontend::rewire(DocumentWriter& writer)
{
  m_writer = &writer;
}

void DocumentWriterFrontend::write(char c)
{
  m_writer->write(c);
}

void DocumentWriterFrontend::write(const std::string& str)
{
  m_writer->write(str);
}

bool DocumentWriterFrontend::handle(const FunctionCall& call)
{
  typedef void(DocumentWriterFrontend::*Callback)(const FunctionCall&);

  static const std::map<std::string, Callback> fn_map = {
    {Functions::PAR, &DocumentWriterFrontend::par},
    {Functions::BOLD, &DocumentWriterFrontend::bold},
    {Functions::BEGINTEXTBF, &DocumentWriterFrontend::begintextbf},
    {Functions::ENDTEXTBF, &DocumentWriterFrontend::endtextbf},
    {Functions::ITALIC, &DocumentWriterFrontend::italic},
    {Functions::BEGINTEXTIT, &DocumentWriterFrontend::begintextit},
    {Functions::ENDTEXTIT, &DocumentWriterFrontend::endtextit},
    {Functions::INLINECODE, &DocumentWriterFrontend::inlinecode},
    {Functions::BEGINTEXTTT, &DocumentWriterFrontend::begintexttt},
    {Functions::ENDTEXTTT, &DocumentWriterFrontend::endtexttt},
    {Functions::SINCE, &DocumentWriterFrontend::since},
    {Functions::BEGINSINCE, &DocumentWriterFrontend::beginsince},
    {Functions::ENDSINCE, &DocumentWriterFrontend::endsince},
    {Functions::LIST, &DocumentWriterFrontend::list},
    {Functions::LI, &DocumentWriterFrontend::li},
    {Functions::ENDLIST, &DocumentWriterFrontend::endlist},
    {Functions::HREF, &DocumentWriterFrontend::href},
    {Functions::IMAGE, &DocumentWriterFrontend::image},
    {Functions::BACKSLASH_LBRACKET, &DocumentWriterFrontend::displaymath},
    {Functions::BACKSLASH_RBRACKET, &DocumentWriterFrontend::enddisplaymath},
    {Functions::MAKEGROUPTABLE, &DocumentWriterFrontend::makegrouptable},
    {Functions::CODE, &DocumentWriterFrontend::code},
    {Functions::ENDCODE, &DocumentWriterFrontend::endcode},
  };

  auto it = fn_map.find(call.function);

  if (it == fn_map.end())
  {
    m_writer->writeCs(call.function);

    return true;
  }
  else
  {
    Callback fun = it->second;

    ((*this).*fun)(call);

    return true;
  }
}

void DocumentWriterFrontend::finish()
{
  m_writer->finish();
}

bool DocumentWriterFrontend::isIdle() const
{
  return m_writer->isIdle();
}

void DocumentWriterFrontend::par(const FunctionCall&)
{
  m_writer->par();
}

void DocumentWriterFrontend::bold(const FunctionCall& c)
{
  const std::string& word = c.arg<std::string>(0);
  m_writer->b(word);
}

void DocumentWriterFrontend::begintextbf(const FunctionCall&)
{
  m_writer->begintextbf();
}

void DocumentWriterFrontend::endtextbf(const FunctionCall&)
{
  m_writer->endtextbf();
}

void DocumentWriterFrontend::italic(const FunctionCall& c)
{
  const std::string& word = c.arg<std::string>(0);
  m_writer->e(word);
}

void DocumentWriterFrontend::begintextit(const FunctionCall& c)
{
  m_writer->begintextit();
}

void DocumentWriterFrontend::endtextit(const FunctionCall& c)
{
  m_writer->endtextit();
}

void DocumentWriterFrontend::inlinecode(const FunctionCall& c)
{
  const std::string& word = c.arg<std::string>(0);
  m_writer->c(word);
}

void DocumentWriterFrontend::begintexttt(const FunctionCall& c)
{
  m_writer->begintexttt();
}

void DocumentWriterFrontend::endtexttt(const FunctionCall& c)
{
  m_writer->endtexttt();
}

void DocumentWriterFrontend::since(const FunctionCall& c)
{
  std::string version = std::get<std::string>(c.options.at(""));
  const std::string& text = std::get<std::string>(c.arguments.front());
  m_writer->since(std::move(version), text);
}

void DocumentWriterFrontend::beginsince(const FunctionCall& c)
{
  std::string version = std::get<std::string>(c.options.at(""));
  m_writer->beginSinceBlock(std::move(version));
}

void DocumentWriterFrontend::endsince(const FunctionCall&)
{
  m_writer->endSinceBlock();
}

void DocumentWriterFrontend::list(const FunctionCall& c)
{
  std::optional<std::string> marker = c.opt<std::string>("marker");
  std::optional<bool> ordered = c.opt<bool>("ordered");
  std::optional<bool> reversed = c.opt<bool>("reversed");

  m_writer->list(marker, ordered, reversed);
}

void DocumentWriterFrontend::li(const FunctionCall& c)
{
  std::optional<std::string> marker = c.opt<std::string>("marker");
  std::optional<int> value = c.opt<int>("value");
  m_writer->li(marker, value);
}

void DocumentWriterFrontend::endlist(const FunctionCall&)
{
  m_writer->endlist();
}

void DocumentWriterFrontend::href(const FunctionCall& c)
{
  std::string link = c.arg<std::string>(0);
  std::string text = c.arg<std::string>(1);
  m_writer->href(std::move(link), text);
}

void DocumentWriterFrontend::image(const FunctionCall& c)
{
  std::string src = c.arg<std::string>(0);
  std::optional<int> width = c.opt<int>("width");
  std::optional<int> height = c.opt<int>("height");
  m_writer->image(std::move(src), width, height);
}

void DocumentWriterFrontend::displaymath(const FunctionCall&)
{
  m_writer->displaymath();
}

void DocumentWriterFrontend::enddisplaymath(const FunctionCall&)
{
  m_writer->enddisplaymath();
}

void DocumentWriterFrontend::makegrouptable(const FunctionCall& c)
{
  std::string groupname = c.arg<std::string>(0);
  m_writer->makegrouptable(groupname);
}

void DocumentWriterFrontend::code(const FunctionCall& c)
{
  std::string lang = c.opt<std::string>("", "");
  m_writer->code(lang);
}

void DocumentWriterFrontend::endcode(const FunctionCall& c)
{
  m_writer->endcode();
}


DocumentWriterToolchain::DocumentWriterToolchain()
  : m_frontend(m_backend)
{

}

void DocumentWriterToolchain::write(char c)
{
  m_frontend.write(c);
}

void DocumentWriterToolchain::write(const std::string& str)
{
  m_frontend.write(str);
}

bool DocumentWriterToolchain::handle(const FunctionCall& call)
{
  return m_frontend.handle(call);
}

void DocumentWriterToolchain::finish()
{
  m_frontend.finish();
}

bool DocumentWriterToolchain::isIdle() const
{
  return m_frontend.isIdle();
}

} // namespace dex
