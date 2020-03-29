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

ListWriter::ListWriter(const FunctionCall& call)
{
  auto list = std::make_shared<dom::List>();

  list->marker = call.opt<std::string>("marker", "");
  list->ordered = call.opt<bool>("ordered", false);
  list->reversed = call.opt<bool>("reversed", false);

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

void ListWriter::handle(const FunctionCall& call)
{
  if (call.function == Functions::LI)
  {
    if (m_doc_writer && m_doc_writer->isWritingList())
    {
      m_doc_writer->handle(call);
    }
    else
    {
      if (m_doc_writer)
        finish();

      auto item = std::make_shared<dom::ListItem>();

      item->marker = call.opt<std::string>("marker", "");
      item->value = call.opt<int>("value", item->value);

      output()->items.push_back(item);

      m_doc_writer = std::make_shared<DocumentWriter>();
    }
  }
  else if (call.function == Functions::ENDLIST)
  {
    if (m_doc_writer && m_doc_writer->isWritingList())
    {
      m_doc_writer->handle(call);
    }
    else
    {
      finish();
    }
  }
  else
  {
    if (m_doc_writer)
    {
      m_doc_writer->handle(call);
    }
    else
    {
      throw BadControlSequence{ call.function };
    }
  }
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

std::shared_ptr<dom::List> ListWriter::output() const
{
  return std::static_pointer_cast<dom::List>(ContentWriter::output());
}

} // namespace dex
