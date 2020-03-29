// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer.h"

#include "dex/model/since.h"

#include "dex/input/list-writer.h"
#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include <cassert>
#include <stdexcept>

namespace dex
{

DocumentWriter::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

DocumentWriter::DocumentWriter()
{
  m_state.enter<FrameType::Idle>();
}

void DocumentWriter::write(char c)
{
  if (isIdle())
  {
    if (is_space(c))
      return;

    startParagraph();
    paragraph().write(c);
  }
  else if (isWritingParagraph())
  {
    paragraph().write(c);

  }
  else if (isWritingList())
  {
    list().write(c);
  }
}

void DocumentWriter::write(const std::string& str)
{
  if (isWritingParagraph())
  {
    paragraph().write(str);
  }
  else
  {
    for (char c : str)
      write(c);
  }
}

bool DocumentWriter::handle(const FunctionCall& call)
{
  if (call.function == Functions::BEGINSINCE)
  {
    std::string version = std::get<std::string>(call.options.at(""));
    beginSinceBlock(std::move(version));
    return true;
  }
  else if (call.function == Functions::ENDSINCE)
  {
    endSinceBlock();
    return true;
  }
  else if (call.function == Functions::LIST)
  {
    if (isIdle() || isWritingParagraph())
    {
      startList();
      return true;
    }
    else
    {
      assert(isWritingList());
      return list().handle(call);
    }
  }
  else if (isWritingParagraph())
  {
    return paragraph().handle(call);
  }
  else if (isWritingList())
  {
    return list().handle(call);
  }
  else
  {
    return false;
  }
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

void DocumentWriter::startList()
{
  if (isWritingParagraph())
    endParagraph();

  m_state.enter<FrameType::WritingList>();
  currentFrame().data = std::make_shared<ListWriter>();
}

void DocumentWriter::endList()
{
  assert(isWritingList());

  list().finish();
  auto l = list().output();

  if (m_state.since.has_value())
  {
    // TODO: handle since
  }

  m_nodes.push_back(l);
  m_state.leave();
}

void DocumentWriter::finish()
{
  if (isWritingParagraph())
    endParagraph();
  else if (isWritingList())
    endList();
}

DocumentWriter::Frame& DocumentWriter::currentFrame()
{
  return m_state.current();
}

const DocumentWriter::Frame& DocumentWriter::currentFrame() const
{
  return m_state.current();
}

bool DocumentWriter::isIdle() const
{
  return currentFrame().type == FrameType::Idle;
}

bool DocumentWriter::isWritingParagraph() const
{
  return currentFrame().type == FrameType::WritingParagraph;
}

ParagraphWriter& DocumentWriter::paragraph()
{
  assert(isWritingParagraph());
  return *(std::get<std::shared_ptr<ParagraphWriter>>(currentFrame().data));
}

bool DocumentWriter::isWritingList() const
{
  return currentFrame().type == FrameType::WritingList;
}

ListWriter& DocumentWriter::list()
{
  assert(isWritingList());
  return *(std::get<std::shared_ptr<ListWriter>>(currentFrame().data));
}

dom::Paragraph& DocumentWriter::currentParagraph()
{
  return *(paragraph().output());
}

} // namespace dex
