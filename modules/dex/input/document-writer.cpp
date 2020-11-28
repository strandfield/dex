// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/document-writer.h"

#include "dex/model/code-block.h"
#include "dex/model/since.h"
#include "dex/model/manual.h"

#include "dex/input/math-writer.h"
#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include <dom/image.h>
#include <dom/list.h>

#include <cassert>
#include <stdexcept>

namespace dex
{

DocumentWriter::DocumentWriter(std::shared_ptr<dom::Node> out)
  : m_state(State::Idle),
    m_result(out)
{
  if (!m_result)
    m_result = std::make_shared<dom::Document>();

  pushNode(m_result);
}

DocumentWriter::~DocumentWriter()
{
}

void DocumentWriter::write(char c)
{
  if (m_state == State::Idle || m_state == State::WritingListItem)
  {
    if (is_space(c))
      return;

    startParagraph();
    paragraphWriter().write(c);
  }
  else if (isWritingParagraph())
  {
    paragraphWriter().write(c);
  }
  else if (isWritingMath())
  {
    currentMath().write(c);
  }
  else if (isWritingCode())
  {
    static_cast<CodeBlock&>(currentNode()).code.push_back(c);
  }
  else
  {
    if (!is_space(c))
      throw std::runtime_error{ "DocumentWriter::write(char)" };
  }
}

void DocumentWriter::write(const std::string& str)
{
  if (isWritingParagraph())
  {
    paragraphWriter().write(str);
  }
  else if (isWritingCode())
  {
    static_cast<CodeBlock&>(currentNode()).code += str;
  }
  else
  {
    for (char c : str)
      write(c);
  }
}

void DocumentWriter::writeCs(const std::string& cs)
{
  switch (m_state)
  {
  case State::WritingParagraph:
    return paragraphWriter().writeCs(cs);
  case State::WritingMath:
    return currentMath().writeControlSequence(cs);
  default:
    throw std::runtime_error{ "DocumentWriter::writeCs()" };
  }
}

void DocumentWriter::bgroup()
{
  switch (m_state)
  {
  case State::WritingMath:
    return currentMath().beginMathList();
  default:
    break;
  }
}

void DocumentWriter::egroup()
{
  switch (m_state)
  {
  case State::WritingMath:
    return currentMath().endMathList();
  default:
    break;
  }
}

void DocumentWriter::mathshift()
{
  paragraphWriter().mathshift();
}

void DocumentWriter::alignmenttab()
{
  switch (m_state)
  {
  case State::WritingParagraph:
    return paragraphWriter().alignmenttab();
  case State::WritingMath:
    return currentMath().alignmentTab();
  default:
    throw std::runtime_error{ "DocumentWriter::alignmenttab()" };
  }
}

void DocumentWriter::superscript()
{
  switch (m_state)
  {
  case State::WritingParagraph:
    return paragraphWriter().superscript();
  case State::WritingMath:
    return currentMath().superscript();
  default:
    throw std::runtime_error{ "DocumentWriter::superscript()" };
  }
}

void DocumentWriter::subscript()
{
  switch (m_state)
  {
  case State::WritingParagraph:
    return paragraphWriter().subscript();
  case State::WritingMath:
    return currentMath().subscript();
  default:
    throw std::runtime_error{ "DocumentWriter::subscript()" };
  }
}

void DocumentWriter::par()
{
  if (isWritingParagraph())
    endParagraph();
}

void DocumentWriter::b(const std::string& text)
{
  paragraphWriter().writeStyledText("bold", text);
}

void DocumentWriter::begintextbf()
{
  paragraphWriter().begintextbf();
}

void DocumentWriter::endtextbf()
{
  paragraphWriter().endtextbf();
}

void DocumentWriter::e(const std::string& text)
{
  paragraphWriter().writeStyledText("italic", text);
}

void DocumentWriter::begintextit()
{
  paragraphWriter().begintextit();
}

void DocumentWriter::endtextit()
{
  paragraphWriter().endtextit();
}

void DocumentWriter::c(const std::string& text)
{
  paragraphWriter().writeStyledText("code", text);
}

void DocumentWriter::begintexttt()
{
  paragraphWriter().begintexttt();
}

void DocumentWriter::endtexttt()
{
  paragraphWriter().endtexttt();
}

void DocumentWriter::since(std::string version, const std::string& text)
{
  // TODO: the following is incorrect and does not handle paragraphs inside a list
  if (!isWritingParagraph())
    throw std::runtime_error{ "DocumentWriter::since()" };

  paragraphWriter().writeSince(version, text);
}

void DocumentWriter::href(std::string link, const std::string& text)
{
  paragraphWriter().writeLink(std::move(link), text);
}

void DocumentWriter::image(std::string src, std::optional<int> width, std::optional<int> height)
{
  if (isWritingParagraph())
    finish();

  auto img = std::make_shared<dom::Image>(std::move(src));
  img->height = height.value_or(img->height);
  img->width = width.value_or(img->width);
  currentNode().appendChild(img);
}

void DocumentWriter::list()
{
  list({}, {}, {});
}

void DocumentWriter::list(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed)
{
  if(m_state == State::WritingList)
    throw std::runtime_error{ "DocumentWriter::list() nested call without \\li" };

  if (isWritingParagraph())
    endParagraph();

  m_state = State::WritingList;

  auto list = std::make_shared<dom::List>();

  list->marker = marker.value_or("");
  list->ordered = ordered.value_or(false);
  list->reversed = reversed.value_or(false);

  currentNode().appendChild(list);
  pushNode(list);
}

void DocumentWriter::li(std::optional<std::string> marker, std::optional<int> value)
{
  if (isWritingParagraph())
    endParagraph();

  if (m_state == State::WritingListItem)
  {
    popNode();
    m_state = State::WritingList;
  }

  auto item = std::make_shared<dom::ListItem>();

  item->marker = marker.value_or("");
  item->value = value.value_or(item->value);

  assert(currentNode().is<dom::List>());
  currentNode().appendChild(item);

  pushNode(item);
  m_state = State::WritingListItem;
}

void DocumentWriter::endlist()
{
  if (isWritingParagraph())
    endParagraph();

  if (m_state == State::WritingListItem)
  {
    popNode();
    m_state = State::WritingList;
  }

  if (!isWritingList())
    throw std::runtime_error{ "DocumentWriter::endlist()" };

  popNode();
  adjustState();
}

void DocumentWriter::displaymath()
{
  if (isWritingParagraph())
    endParagraph();

  m_math_writer = std::make_unique<dex::MathWriter>();
  auto m = m_math_writer->output();

  pushNode(m);
  m_state = State::WritingMath;
}

void DocumentWriter::enddisplaymath()
{
  if(!isWritingMath())
    throw std::runtime_error{ "DocumentWriter::enddisplaymath()" };

  m_math_writer->finish();

  auto mathnode = currentNodeShared();
  popNode();
  currentNode().appendChild(mathnode);

  m_math_writer.reset();

  adjustState();
}

void DocumentWriter::code(const std::string& lang)
{
  if (isWritingParagraph())
    endParagraph();

  if (isWritingMath() || isWritingList())
    throw std::runtime_error{ "DocumentWriter::code() not available in this mode" };

  auto codeblock = std::make_shared<CodeBlock>();
  codeblock->lang = lang;

  currentNode().appendChild(codeblock);
  pushNode(codeblock);

  m_state = State::WritingCode;
}

void DocumentWriter::endcode()
{
  if (!isWritingCode())
    throw std::runtime_error{ "DocumentWriter::endcode()" };

  static_cast<CodeBlock&>(currentNode()).normalize();

  popNode();
  adjustState();
}

void DocumentWriter::makegrouptable(std::string groupname)
{
  if (isWritingParagraph())
    endParagraph();
  
  if (isWritingMath() || isWritingList())
    throw std::runtime_error{ "DocumentWriter::makegrouptable() not available in this mode" };

  auto node = std::make_shared<dex::GroupTable>(std::move(groupname));
  currentNode().appendChild(node);
}

void DocumentWriter::beginSinceBlock(const std::string& version)
{
  if(m_since)
    throw std::runtime_error{ "Cannot have nested since block" };

  if (isWritingParagraph())
    endParagraph();

  m_since = std::make_shared<BeginSince>(version);
  currentNode().appendChild(m_since);
}

void DocumentWriter::endSinceBlock()
{
  assert(m_since != nullptr);

  if (isWritingParagraph())
    endParagraph();

  currentNode().appendChild(std::make_shared<EndSince>(m_since));

  m_since.reset();
}

void DocumentWriter::write(const std::shared_ptr<dom::Node>& node)
{
  if (isWritingParagraph())
    endParagraph();

  currentNode().appendChild(node);
}

void DocumentWriter::setOutput(std::shared_ptr<dom::Node> out)
{
  assert(m_nodes.size() == 1);
  m_nodes.clear();
  m_result = out;
  pushNode(out);
}

void DocumentWriter::startParagraph()
{
  if (isWritingParagraph())
    throw std::runtime_error{ "Already writing a paragraph" };

  m_paragraph_writer = std::make_unique<ParagraphWriter>();

  auto p = m_paragraph_writer->output();
  pushNode(p);

  m_state = State::WritingParagraph;
}

void DocumentWriter::endParagraph()
{
  assert(isWritingParagraph());

  paragraphWriter().finish();
  auto par = paragraphWriter().output();

  popNode();

  currentNode().appendChild(par);

  adjustState();
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

ParagraphWriter& DocumentWriter::paragraphWriter()
{
  if (!isWritingParagraph())
    startParagraph();

  assert(isWritingParagraph());
  return *m_paragraph_writer;
}

bool DocumentWriter::isWritingList() const
{
  return m_state == State::WritingList;
}

bool DocumentWriter::isWritingListItem() const
{
  return m_state == State::WritingListItem;
}

bool DocumentWriter::isWritingMath() const
{
  return m_state == State::WritingMath;
}

bool DocumentWriter::isWritingCode() const
{
  return m_state == State::WritingCode;
}

MathWriter& DocumentWriter::currentMath()
{
  assert(isWritingMath());
  return *m_math_writer;
}

void DocumentWriter::pushNode(std::shared_ptr<dom::Node> n)
{
  m_nodes.push_back(n);
}

void DocumentWriter::popNode()
{
  m_nodes.pop_back();
}

dom::Node& DocumentWriter::currentNode()
{
  return *m_nodes.back();
}

std::shared_ptr<dom::Node> DocumentWriter::currentNodeShared()
{
  return m_nodes.back();
}

void DocumentWriter::adjustState()
{
  if (m_nodes.size() == 1)
  {
    m_state = State::Idle;
  }
  else
  {
    if(currentNode().is<dom::List>())
      m_state = State::WritingList;
    else
      m_state = State::WritingListItem;
  }
}

} // namespace dex
