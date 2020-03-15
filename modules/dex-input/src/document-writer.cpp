// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer.h"

#include "dex/model/since.h"

#include <dom/paragraph/link.h>
#include <dom/paragraph/textstyle.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

inline static bool is_space(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

DocumentWriter::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

DocumentWriter::DocumentWriter()
{
  m_state.enter<FrameType::Idle>();
}

void DocumentWriter::begin()
{
  if (!isWritingParagraph())
    startParagraph();
}

void DocumentWriter::write(char c)
{
  if (!isWritingParagraph() && is_space(c))
    return;

  if (!isWritingParagraph())
    startParagraph();

  currentParagraph().addChar(c);
}

void DocumentWriter::write(const std::string& str)
{
  if (!isWritingParagraph())
    startParagraph();

  currentParagraph().addText(str);
}

void DocumentWriter::end()
{
  if (isWritingParagraph())
    endParagraph();
}

void DocumentWriter::writeLink(std::string url, const std::string& text)
{
  if (!isWritingParagraph())
    startParagraph();

  dom::Paragraph& par = currentParagraph();
  size_t start = par.length();
  par.addText(text);

  auto link = std::make_shared<dom::Link>(dom::ParagraphRange(par, start, par.length()), std::move(url));
  currentParagraph().addMetaData(link);
}

void DocumentWriter::writeStyledText(std::string style_name, const std::string& text)
{
  if (!isWritingParagraph())
    startParagraph();

  dom::Paragraph& par = currentParagraph();
  size_t start = par.length();
  par.addText(std::move(text));

  auto style = std::make_shared<dom::TextStyle>(dom::ParagraphRange(par, start, par.length()), std::move(style_name));
  currentParagraph().addMetaData(style);
}

void DocumentWriter::beginSinceBlock(const std::string& version)
{
  if (state().since.has_value())
    throw std::runtime_error{ "Cannot have nested since block" };

  if (isWritingParagraph())
    endParagraph();

  state().since = version;
}

void DocumentWriter::endSinceBlock()
{
  assert(state().since.has_value());

  if (isWritingParagraph())
    endParagraph();

  state().since.reset();
}

void DocumentWriter::writeSince(const std::string& version, const std::string& text)
{
  if (!isWritingParagraph())
    startParagraph();

  dom::Paragraph& par = currentParagraph();
  size_t start = par.length();
  par.addText(text);

  currentParagraph().add<dex::Since>(dom::ParagraphRange(par, start, par.length()), version);
}

void DocumentWriter::add(const std::shared_ptr<dom::Node>& node)
{
  if (isWritingParagraph())
    endParagraph();

  m_nodes.push_back(node);
}

void DocumentWriter::startParagraph()
{
  if (isWritingParagraph())
    throw std::runtime_error{ "Already writing a paragraph" };

  auto par = std::make_shared<dom::Paragraph>();
  m_state.enter<FrameType::WritingParagraph>();
  currentFrame().data = par;
}

void DocumentWriter::endParagraph()
{
  assert(isWritingParagraph());

  auto par = std::get<std::shared_ptr<dom::Paragraph>>(currentFrame().data);

  if (m_state.since.has_value())
  {
    par->add<dex::Since>(dom::ParagraphRange(*par), m_state.since.value());
  }

  m_nodes.push_back(par);
  m_state.leave();
}

DocumentWriter::Frame& DocumentWriter::currentFrame()
{
  return m_state.current();
}

bool DocumentWriter::isWritingParagraph()
{
  return currentFrame().type == FrameType::WritingParagraph;
}

dom::Paragraph& DocumentWriter::currentParagraph()
{
  return *std::get<std::shared_ptr<dom::Paragraph>>(currentFrame().data);
}

} // namespace dex
