// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer.h"

#include "dex/model/since.h"

#include "dex/input/list-writer.h"
#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include <dom/image.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

DocumentWriter::DocumentWriter()
  : m_state(State::Idle)
{

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
  else
  {
    if ((isWritingParagraph() || isWritingList()) && m_writer->handle(call))
      return true;

    if (isWritingList() || isWritingParagraph())
      finish();

    if (call.function == Functions::IMAGE)
    {
      std::string src = call.arg<std::string>(0);
      auto img = std::make_shared<dom::Image>(std::move(src));
      img->height = call.opt<int>("height", img->height);
      img->width = call.opt<int>("width", img->width);
      m_nodes.push_back(img);

      return true;
    }
  }

  return false;
}

void DocumentWriter::beginSinceBlock(const std::string& version)
{
  if(m_since.has_value())
    throw std::runtime_error{ "Cannot have nested since block" };

  if (isWritingParagraph())
    endParagraph();

  m_since = version;
}

void DocumentWriter::endSinceBlock()
{
  assert(m_since.has_value());

  if (isWritingParagraph())
    endParagraph();

  m_since.reset();
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

  m_state = State::WritingParagraph;
  m_writer = std::make_shared<ParagraphWriter>();
}

void DocumentWriter::endParagraph()
{
  assert(isWritingParagraph());

  paragraph().finish();
  auto par = paragraph().output();

  if (m_since.has_value())
  {
    par->add<dex::Since>(dom::ParagraphRange(*par), m_since.value());
  }

  m_nodes.push_back(par);
  m_state = State::Idle;
  m_writer = nullptr;
}

void DocumentWriter::startList()
{
  if (isWritingParagraph())
    endParagraph();

  m_state = State::WritingList;
  m_writer = std::make_shared<ListWriter>();
}

void DocumentWriter::endList()
{
  assert(isWritingList());

  list().finish();
  auto l = list().output();

  if (m_since.has_value())
  {
    // TODO: handle since
  }

  m_nodes.push_back(l);
  m_state = State::Idle;
  m_writer = nullptr;
}

void DocumentWriter::finish()
{
  if (isWritingParagraph())
    endParagraph();
  else if (isWritingList())
    endList();
}

bool DocumentWriter::isIdle() const
{
  return m_state == State::Idle;
}

bool DocumentWriter::isWritingParagraph() const
{
  return m_state == State::WritingParagraph;
}

ParagraphWriter& DocumentWriter::paragraph()
{
  assert(isWritingParagraph());
  return *static_cast<ParagraphWriter*>(m_writer.get());
}

bool DocumentWriter::isWritingList() const
{
  return m_state == State::WritingList;
}

ListWriter& DocumentWriter::list()
{
  assert(isWritingList());
  return *static_cast<ListWriter*>(m_writer.get());
}

dom::Paragraph& DocumentWriter::currentParagraph()
{
  return *(paragraph().output());
}

} // namespace dex
