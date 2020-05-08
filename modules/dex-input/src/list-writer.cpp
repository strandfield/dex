// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/list-writer.h"

#include "dex/input/document-writer.h"
#include "dex/input/parser-errors.h"

#include <cassert>
#include <stdexcept>

namespace dex
{

ListWriter::ListWriter()
{
  setOutput(std::make_shared<dom::List>());
}

ListWriter::~ListWriter()
{

}

ListWriter::ListWriter(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed)
{
  auto list = std::make_shared<dom::List>();

  list->marker = marker.value_or("");
  list->ordered = ordered.value_or(false);
  list->reversed = reversed.value_or(false);

  setOutput(list);
}

void ListWriter::write(char c)
{
  if (!m_doc_writer && is_space(c))
    return;

  if (m_doc_writer == nullptr)
    throw std::runtime_error{ "ListWriter::write()" };

  m_doc_writer->write(c);
}

void ListWriter::finish()
{
  if (m_doc_writer)
  {
    m_doc_writer->finish();
    auto list = output();
    list->items.back()->content = m_doc_writer->output();
    m_doc_writer = nullptr;
  }
}

DocumentWriter& ListWriter::content() const
{
  if (!m_doc_writer)
    throw std::runtime_error{ "ListWriter : no active listitem" };

  return *m_doc_writer;
}

void ListWriter::list()
{
  content().list();
}

void ListWriter::list(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed)
{
  content().list(marker, ordered, reversed);
}

void ListWriter::li(std::optional<std::string>& marker, std::optional<int>& value)
{
  if (m_doc_writer)
  {
    if (m_doc_writer->isWritingList())
    {
      m_doc_writer->li(marker, value);
      return;
    }

    finish();
  }

  auto item = std::make_shared<dom::ListItem>();

  item->marker = marker.value_or("");
  item->value = value.value_or(item->value);

  output()->items.push_back(item);

  m_doc_writer = std::make_shared<DocumentWriter>();
}

void ListWriter::endlist()
{
  content().endlist();
}

std::shared_ptr<dom::List> ListWriter::output() const
{
  return std::static_pointer_cast<dom::List>(DomWriter::output());
}

} // namespace dex
