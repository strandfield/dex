// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/document.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace dex
{

Document::Document(std::string title)
  : title(std::move(title))
{

}

model::Kind Document::kind() const
{
  return ClassKind;
}

bool Document::isDocument() const
{
  return true;
}

std::string Document::className() const
{
  return "document";
}

const DomNodeList& Document::childNodes() const
{
  return nodes;
}

void Document::appendChild(const DocumentNodePtr& node)
{
  nodes.push_back(node);
}

bool DocumentNode::isDocumentNode() const
{
  return true;
}

const std::vector<std::shared_ptr<DocumentNode>>& DocumentNode::childNodes() const
{
  static const DomNodeList static_instance = {};
  return static_instance;
}

void DocumentNode::appendChild(const std::shared_ptr<DocumentNode>& node)
{
  throw std::runtime_error{ "DocumentNode::appendChild()" };
}

bool DocumentElement::isDocumentElement() const
{
  return true;
}

/******************************************************************************************************
* Paragraph
*******************************************************************************************************/

static size_t find_iterator_end(const Paragraph& p, size_t self_begin)
{
  const size_t par_metadata_end = p.metadata().at(self_begin)->range().end();
  size_t self_end = self_begin;

  while (self_end < p.metadata().size() && p.metadata().at(self_end)->range().end() <= par_metadata_end)
    ++self_end;

  return self_end;
}

ParagraphIterator::ParagraphIterator(const Paragraph& p)
  : m_par(&p),
  m_parent_index(p.metadata().size()),
  m_index(0)
{
  m_text = p.metadata().empty() || p.metadata().front()->range().begin() > 0;
}

ParagraphIterator::ParagraphIterator(const Paragraph& p, size_t parent_index)
  : m_par(&p),
  m_parent_index(parent_index),
  m_index(parent_index + 1),
  m_text(true)
{
  const ParagraphRange r = range();
  m_text = (r.end() - r.begin()) > 0;
}

ParagraphIterator::ParagraphIterator(const Paragraph& p, size_t parent_index, size_t index, bool text)
  : m_par(&p),
  m_parent_index(parent_index),
  m_index(index),
  m_text(text)
{

}

const Paragraph& ParagraphIterator::paragraph() const
{
  return *m_par;
}

ParagraphRange ParagraphIterator::range() const
{
  if (paragraph().metadata().empty())
    return ParagraphRange{ paragraph() };

  if (!isChild())
  {
    if (m_text)
    {
      if (m_index == 0)
      {
        return ParagraphRange{ paragraph(), 0, paragraph().metadata().at(0)->range().begin() };
      }
      else if (m_index == paragraph().metadata().size())
      {
        const size_t begin = paragraph().metadata().at(m_index - 1)->range().end();
        const size_t end = paragraph().length();
        return ParagraphRange{ paragraph(), begin, end };
      }
      else
      {
        const size_t begin = paragraph().metadata().at(m_index - 1)->range().end();
        const size_t end = paragraph().metadata().at(m_index)->range().begin();
        return ParagraphRange{ paragraph(), begin, end };
      }
    }
    else
    {
      return paragraph().metadata().at(m_index)->range();
    }
  }
  else
  {
    if (m_text)
    {
      const size_t end_index = find_iterator_end(paragraph(), m_parent_index);

      if (m_index == m_parent_index + 1)
      {
        const size_t begin = paragraph().metadata().at(m_parent_index)->range().begin();
        const size_t end = paragraph().metadata().at(m_index)->range().begin();
        return ParagraphRange{ paragraph(), begin, end };
      }
      else if (m_index == end_index)
      {
        const size_t begin = paragraph().metadata().at(m_index - 1)->range().end();
        const size_t end = paragraph().metadata().at(m_parent_index)->range().end();;
        return ParagraphRange{ paragraph(), begin, end };
      }
      else
      {
        const size_t begin = paragraph().metadata().at(m_index - 1)->range().end();
        const size_t end = paragraph().metadata().at(m_index)->range().begin();
        return ParagraphRange{ paragraph(), begin, end };
      }
    }
    else
    {
      return paragraph().metadata().at(m_index)->range();
    }
  }
}

bool ParagraphIterator::isText() const
{
  return m_text;
}

bool ParagraphIterator::hasChild() const
{
  const size_t end_index = find_iterator_end(paragraph(), m_index);
  return (end_index - m_index) > 1;
}

bool ParagraphIterator::isChild() const
{
  return m_parent_index != paragraph().metadata().size();
}

bool ParagraphIterator::isEnd() const
{
  const size_t end_index = m_parent_index < paragraph().metadata().size() ?
    find_iterator_end(paragraph(), m_parent_index) : m_parent_index;

  return m_index == end_index && !m_text;
}

ParagraphIterator ParagraphIterator::begin() const
{
  return ParagraphIterator{ paragraph(), m_index };
}

ParagraphIterator ParagraphIterator::end() const
{
  ParagraphIterator it{ paragraph() };
  it.m_text = false;
  it.m_parent_index = m_index;
  it.m_index = find_iterator_end(paragraph(), m_index);
  return it;
}

ParagraphIterator& ParagraphIterator::operator++()
{
  assert(!isEnd());

  if (m_text)
  {
    m_text = false;
  }
  else
  {
    m_text = true;
    m_index = find_iterator_end(paragraph(), m_index);

    if (range().end() - range().begin() == 0)
    {
      ++(*this);
    }
  }

  return *this;
}

ParagraphIterator ParagraphIterator::operator++(int)
{
  ParagraphIterator copy = *(this);
  ++(*this);
  return copy;
}

std::shared_ptr<ParagraphMetaData> ParagraphIterator::operator*() const
{
  return m_text ? nullptr : paragraph().metadata().at(m_index);
}

bool ParagraphIterator::operator==(const ParagraphIterator& other) const
{
  return m_par == other.m_par
    && m_parent_index == other.m_parent_index
    && m_index == other.m_index
    && m_text == other.m_text;
}

bool ParagraphIterator::operator!=(const ParagraphIterator& other) const
{
  return !(*this == other);
}


ParagraphRange::ParagraphRange(const Paragraph& p, size_t begin, size_t end)
  : m_par(&p),
  m_begin(begin),
  m_end(end)
{
  if (m_end == std::numeric_limits<size_t>::max())
  {
    m_end = p.text().size();
  }
}

const Paragraph& ParagraphRange::paragraph() const
{
  return *m_par;
}

size_t ParagraphRange::begin() const
{
  return m_begin;
}

size_t ParagraphRange::end() const
{
  return m_end;
}

std::string ParagraphRange::text() const
{
  return std::string(paragraph().text().begin() + begin(), paragraph().text().begin() + end());
}

ParagraphMetaData::ParagraphMetaData(const ParagraphRange& parrange)
  : m_range(parrange)
{

}

ParagraphRange& ParagraphMetaData::range()
{
  return m_range;
}

const ParagraphRange& ParagraphMetaData::range() const
{
  return m_range;
}


model::Kind Paragraph::kind() const
{
  return ClassKind;
}

ParagraphRange Paragraph::range(size_t begin, size_t end) const
{
  return ParagraphRange{ *this, begin, end };
}

ParagraphIterator Paragraph::begin() const
{
  return ParagraphIterator{ *this };
}

ParagraphIterator Paragraph::end() const
{
  return ParagraphIterator{ *this, m_metadata.size(), m_metadata.size(), false };
}

void Paragraph::setText(std::string text)
{
  m_metadata.clear();

  m_text = std::move(text);
}

void Paragraph::addChar(char c)
{
  m_text.push_back(c);
}

void Paragraph::addText(const std::string& text)
{
  m_text.insert(m_text.end(), text.begin(), text.end());
}

void Paragraph::addMetaData(const std::shared_ptr<ParagraphMetaData>& md)
{
  using PMD = std::shared_ptr<ParagraphMetaData>;

  auto it = std::upper_bound(m_metadata.begin(), m_metadata.end(), md, [](const PMD& lhs, const PMD& rhs) {
    return lhs->range().begin() < rhs->range().begin() ||
      (lhs->range().begin() == rhs->range().begin() && lhs->range().end() > rhs->range().end());
    });

  m_metadata.insert(it, md);
}

model::Kind Link::kind() const
{
  return ClassKind;
}

model::Kind TextStyle::kind() const
{
  return ClassKind;
}

model::Kind Note::kind() const
{
  return ClassKind;
}

/******************************************************************************************************
* List
*******************************************************************************************************/

ListItem::ListItem()
{

}

model::Kind ListItem::kind() const
{
  return ClassKind;
}

const DomNodeList& ListItem::childNodes() const
{
  return content;
}

void ListItem::appendChild(const DocumentNodePtr& node)
{
  content.push_back(node);
}

List::List(std::string mark)
  : marker(std::move(mark))
{

}

model::Kind List::kind() const
{
  return ClassKind;
}

const DomNodeList& List::childNodes() const
{
  return items;
}

void List::appendChild(const DocumentNodePtr& node)
{
  items.push_back(node);
}

/******************************************************************************************************
* Image
*******************************************************************************************************/

Image::Image(std::string source)
  : src(std::move(source))
{

}

model::Kind Image::kind() const
{
  return ClassKind;
}

} // namespace dex
