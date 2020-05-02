// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H
#define DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H

#include "dex/input/document-writer.h"
#include "dex/input/functional.h"

namespace dex
{

class DEX_INPUT_API DocumentWriterFrontend
{
private:
  DocumentWriter* m_writer;

public:
  explicit DocumentWriterFrontend(DocumentWriter& writer);

  void rewire(DocumentWriter& writer);

  DocumentWriter::State state() const;

  void write(char c);
  void write(const std::string& str);
  bool handle(const FunctionCall& call);

  bool isIdle() const;

  void finish();

  dom::Content& output();
};

class DEX_INPUT_API DocumentWriterToolchain
{
public:
  DocumentWriterToolchain();

  DocumentWriter::State state() const;

  void write(char c);
  void write(const std::string& str);
  bool handle(const FunctionCall& call);

  bool isIdle() const;

  void finish();

  dom::Content& output();

private:
  DocumentWriter m_backend;
  DocumentWriterFrontend m_frontend;
};

} // namespace dex

namespace dex
{

inline DocumentWriter::State DocumentWriterFrontend::state() const
{
  return m_writer->state();
}

inline dom::Content& DocumentWriterFrontend::output()
{
  return m_writer->output();
}

inline DocumentWriter::State DocumentWriterToolchain::state() const
{
  return m_frontend.state();
}

inline dom::Content& DocumentWriterToolchain::output()
{
  return m_frontend.output();
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H
