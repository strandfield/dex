// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer-frontend.h"

#include "dex/model/since.h"

#include "dex/input/list-writer.h"
#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include <dom/image.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

DocumentWriterFrontend::DocumentWriterFrontend()
{

}

void DocumentWriterFrontend::write(char c)
{
  m_writer.write(c);
}

void DocumentWriterFrontend::write(const std::string& str)
{
  m_writer.write(str);
}

bool DocumentWriterFrontend::handle(const FunctionCall& call)
{
  if (call.function == Functions::SINCE)
  {
    std::string version = std::get<std::string>(call.options.at(""));
    const std::string& text = std::get<std::string>(call.arguments.front());

    m_writer.since(std::move(version), text);
    return true;
  }
  else if (call.function == Functions::BEGINSINCE)
  {
    std::string version = std::get<std::string>(call.options.at(""));
    m_writer.beginSinceBlock(std::move(version));
    return true;
  }
  else if (call.function == Functions::ENDSINCE)
  {
    m_writer.endSinceBlock();
    return true;
  }
  else if (call.function == Functions::LIST)
  {
    std::optional<std::string> marker = call.opt<std::string>("marker");
    std::optional<bool> ordered = call.opt<bool>("ordered");
    std::optional<bool> reversed = call.opt<bool>("reversed");

    m_writer.list(marker, ordered, reversed);
    return true;
  }
  else if (call.function == Functions::LI)
  {
    std::optional<std::string> marker = call.opt<std::string>("marker");
    std::optional<int> value = call.opt<int>("value");
    m_writer.li(marker, value);
    return true;
  }
  else if (call.function == Functions::ENDLIST)
  {
    m_writer.endlist();
    return true;
  }
  else if (call.function == Functions::IMAGE)
  {
    std::string src = call.arg<std::string>(0);
    std::optional<int> width = call.opt<int>("width");
    std::optional<int> height = call.opt<int>("height");
    m_writer.image(std::move(src), width, height);
    return true;
  }

  return false;
}

void DocumentWriterFrontend::beginSinceBlock(const std::string& version)
{
  m_writer.beginSinceBlock(version);
}

void DocumentWriterFrontend::endSinceBlock()
{
  m_writer.endSinceBlock();
}

void DocumentWriterFrontend::write(const std::shared_ptr<dom::Node>& node)
{
  m_writer.write(node);
}

void DocumentWriterFrontend::startParagraph()
{
  m_writer.startParagraph();
}

void DocumentWriterFrontend::endParagraph()
{
  m_writer.endParagraph();
}

void DocumentWriterFrontend::startList()
{
  m_writer.startList();
}

void DocumentWriterFrontend::endList()
{
  m_writer.endList();
}

void DocumentWriterFrontend::finish()
{
  m_writer.finish();
}

bool DocumentWriterFrontend::isIdle() const
{
  return m_writer.isIdle();
}

bool DocumentWriterFrontend::isWritingParagraph() const
{
  return m_writer.isWritingParagraph();
}

ParagraphWriter& DocumentWriterFrontend::paragraph()
{
  return m_writer.paragraph();
}

bool DocumentWriterFrontend::isWritingList() const
{
  return m_writer.isWritingList();
}

} // namespace dex