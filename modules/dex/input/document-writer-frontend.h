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

  // @TODO: remove this bool return value
  bool handle(const FunctionCall& call);

  bool isIdle() const;

  void finish();

  std::shared_ptr<dom::Node> output();

protected:
  void par(const FunctionCall& c);
  void bold(const FunctionCall& c);
  void begintextbf(const FunctionCall& c);
  void endtextbf(const FunctionCall& c);
  void italic(const FunctionCall& c);
  void begintextit(const FunctionCall& c);
  void endtextit(const FunctionCall& c);
  void inlinecode(const FunctionCall& c);
  void begintexttt(const FunctionCall& c);
  void endtexttt(const FunctionCall& c);
  void since(const FunctionCall& c);
  void beginsince(const FunctionCall& c);
  void endsince(const FunctionCall& c);
  void list(const FunctionCall& c);
  void li(const FunctionCall& c);
  void endlist(const FunctionCall& c);
  void href(const FunctionCall& c);
  void image(const FunctionCall& c);
  void displaymath(const FunctionCall& c);
  void enddisplaymath(const FunctionCall& c);
  void makegrouptable(const FunctionCall& c);
  void code(const FunctionCall& c);
  void endcode(const FunctionCall& c);
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

  std::shared_ptr<dom::Node> output();

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

inline std::shared_ptr<dom::Node> DocumentWriterFrontend::output()
{
  return m_writer->output();
}

inline DocumentWriter::State DocumentWriterToolchain::state() const
{
  return m_frontend.state();
}

inline std::shared_ptr<dom::Node> DocumentWriterToolchain::output()
{
  return m_frontend.output();
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_FRONTEND_H
