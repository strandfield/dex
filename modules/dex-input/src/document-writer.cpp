// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer.h"

#include "dex/model/since.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

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

void DocumentWriter::handle(const FunctionCall& call)
{
  if (call.function == Functions::SINCE)
  {
    paragraph().handle(call);
  }
  else if (call.function == Functions::BEGINSINCE)
  {
    std::string version = std::get<std::string>(call.options.at(""));
    beginSinceBlock(std::move(version));
  }
  else
  {
    throw BadControlSequence{ call.function };
  }
}

void DocumentWriter::end()
{
  if (isWritingParagraph())
    endParagraph();
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

void DocumentWriter::write(const std::shared_ptr<dom::Node>& node)
{
  if (isWritingParagraph())
    endParagraph();

  m_nodes.push_back(node);
}

void DocumentWriter::startParagraph()
{
  if (isWritingParagraph())
    throw std::runtime_error{ "Already writing a paragraph" };

  m_state.enter<FrameType::WritingParagraph>();
  currentFrame().data = std::make_shared<ParagraphWriter>();
}

void DocumentWriter::endParagraph()
{
  assert(isWritingParagraph());

  paragraph().finish();
  auto par = paragraph().output();

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

ParagraphWriter& DocumentWriter::paragraph()
{
  assert(isWritingParagraph());
  return *(std::get<std::shared_ptr<ParagraphWriter>>(currentFrame().data));
}

dom::Paragraph& DocumentWriter::currentParagraph()
{
  return *(paragraph().output());
}

} // namespace dex
