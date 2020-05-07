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
    currentList().write(c);
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

void DocumentWriter::bgroup()
{
  /* no-op */
}

void DocumentWriter::egroup()
{
  /* no-op */
}

void DocumentWriter::par()
{
  if (isWritingParagraph())
    endParagraph();
}

void DocumentWriter::b(const std::string& text)
{
  DocumentWriter* target = nullptr;
  
  if (hasActiveNestedWriter(&target))
  {
    target->b(text);
  }
  else
  {
    paragraph().writeStyledText("bold", text);
  }
}

void DocumentWriter::begintextbf()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->begintextbf();
  }
  else
  {
    paragraph().begintextbf();
  }
}

void DocumentWriter::endtextbf()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->endtextbf();
  }
  else
  {
    paragraph().endtextbf();
  }
}

void DocumentWriter::e(const std::string& text)
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->b(text);
  }
  else
  {
    paragraph().writeStyledText("italic", text);
  }
}

void DocumentWriter::begintextit()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->begintextit();
  }
  else
  {
    paragraph().begintextit();
  }
}

void DocumentWriter::endtextit()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->endtextit();
  }
  else
  {
    paragraph().endtextit();
  }
}

void DocumentWriter::c(const std::string& text)
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->b(text);
  }
  else
  {
    paragraph().writeStyledText("code", text);
  }
}

void DocumentWriter::begintexttt()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->begintexttt();
  }
  else
  {
    paragraph().begintexttt();
  }
}

void DocumentWriter::endtexttt()
{
  DocumentWriter* target = nullptr;

  if (hasActiveNestedWriter(&target))
  {
    target->endtexttt();
  }
  else
  {
    paragraph().endtexttt();
  }
}

void DocumentWriter::since(std::string version, const std::string& text)
{
  // TODO: the following is incorrect and does not handle paragraphs inside a list
  if (!isWritingParagraph())
    throw std::runtime_error{ "DocumentWriter::since()" };

  paragraph().writeSince(version, text);
}

void DocumentWriter::image(std::string src, std::optional<int> width, std::optional<int> height)
{
  if (isWritingList())
  {
    currentList().content().image(std::move(src), width, height);
    return;
  }

  if (isWritingParagraph())
    finish();

  auto img = std::make_shared<dom::Image>(std::move(src));
  img->height = height.value_or(img->height);
  img->width = width.value_or(img->width);
  m_nodes.push_back(img);
}

void DocumentWriter::list()
{
  list({}, {}, {});
}

void DocumentWriter::list(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed)
{
  if (isIdle() || isWritingParagraph())
  {
    if (isWritingParagraph())
      endParagraph();

    m_state = State::WritingList;
    m_writer = std::make_shared<ListWriter>(marker, ordered, reversed);
  }
  else
  {
    assert(isWritingList());
    currentList().list(marker, ordered, reversed);
  }
}

void DocumentWriter::li(std::optional<std::string> marker, std::optional<int> value)
{
  if (!isWritingList())
    throw std::runtime_error{"DocumentWriter::li()"};

  currentList().li(marker, value);
}

void DocumentWriter::endlist()
{
  if (!isWritingList())
    throw std::runtime_error{ "DocumentWriter::endlist()" };

  if (currentList().content().isWritingList())
  {
    currentList().endlist();
  }
  else
  {
    currentList().finish();
    auto l = currentList().output();

    if (m_since.has_value())
    {
      // TODO: handle since
    }

    m_nodes.push_back(l);
    m_state = State::Idle;
    m_writer = nullptr;
  }
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

void DocumentWriter::finish()
{
  if (isWritingParagraph())
  {
    endParagraph();
  }
  else if (isWritingList())
  {
    // @TODO: should we issue a warning for a missing \endlist
    endlist();
  }
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

ListWriter& DocumentWriter::currentList()
{
  assert(isWritingList());
  return *static_cast<ListWriter*>(m_writer.get());
}

dom::Paragraph& DocumentWriter::currentParagraph()
{
  return *(paragraph().output());
}

bool DocumentWriter::hasActiveNestedWriter(DocumentWriter** out)
{
  if (!isWritingList())
    return false;

  *out = &(currentList().content());
  return true;
}

} // namespace dex
