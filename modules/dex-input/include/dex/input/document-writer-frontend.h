// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H
#define DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H

#include "dex/input/document-writer.h"
#include "dex/input/functional.h"

namespace dex
{

class ContentWriter;
class ListWriter;
class ParagraphWriter;

class DEX_INPUT_API DocumentWriterFrontend
{
public:
  explicit DocumentWriterFrontend();

  DocumentWriter::State state() const;

  void write(char c);
  void write(const std::string& str);
  bool handle(const FunctionCall& call);

  bool isIdle() const;

  bool isWritingParagraph() const;
  ParagraphWriter& paragraph();

  bool isWritingList() const;

  void beginSinceBlock(const std::string& version);
  void endSinceBlock();
  
  void startParagraph();
  void endParagraph();

  void startList();
  void endList();

  void finish();

  void write(const std::shared_ptr<dom::Node>& node);

  dom::Content& output();

private:
  DocumentWriter m_writer;
};

} // namespace dex

namespace dex
{

inline DocumentWriter::State DocumentWriterFrontend::state() const
{
  return m_writer.state();
}

inline dom::Content& DocumentWriterFrontend::output()
{
  return m_writer.output();
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H
