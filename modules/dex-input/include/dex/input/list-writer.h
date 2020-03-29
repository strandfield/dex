// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_LISTWRITER_H
#define DEX_INPUT_LISTWRITER_H

#include "dex/dex-input.h"

#include "dex/input/content-writer.h"

#include <dom/list.h>

namespace dex
{

class DocumentWriter;

class DEX_INPUT_API ListWriter : public ContentWriter
{
public:
  ListWriter();
  ~ListWriter();

  explicit ListWriter(const FunctionCall& call);

  void write(char c) override;
  void handle(const FunctionCall& call) override;
  void finish() override;

  std::shared_ptr<dom::List> output() const;
  
private:
  std::shared_ptr<DocumentWriter> m_doc_writer;
};

} // namespace dex

#endif // DEX_INPUT_LISTWRITER_H
