// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_H
#define DEX_INPUT_DOCUMENT_WRITER_H

#include "dex/dex-input.h"

#include "dex/common/state.h"

#include "dex/input/content-writer.h"

#include <dom/content.h>
#include <dom/paragraph.h>

#include <optional>
#include <variant>

namespace dex
{

class ContentWriter;
class ListWriter;
class MathWriter;
class ParagraphWriter;

class DEX_INPUT_API DocumentWriter
{
public:
  explicit DocumentWriter();

  enum class State
  {
    Idle,
    WritingParagraph,
    WritingList,
    WritingMath,
  };

  State state() const;

  void write(char c);
  void write(const std::string& str);

  void writeCs(const std::string& cs);

  void bgroup();
  void egroup();

  void mathshift();
  void alignmenttab();
  void superscript();
  void subscript();

  void par();

  void b(const std::string& text);
  void begintextbf();
  void endtextbf();

  void e(const std::string& text);
  void begintextit();
  void endtextit();

  void c(const std::string& text);
  void begintexttt();
  void endtexttt();

  void since(std::string version, const std::string& text);

  void image(std::string src, std::optional<int> width, std::optional<int> height);

  void list();
  void list(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed);
  void li(std::optional<std::string> marker, std::optional<int> value);
  void endlist();

  void displaymath();
  void enddisplaymath();

  bool isIdle() const;

  bool isWritingParagraph() const;
  ParagraphWriter& paragraph();

  bool isWritingList() const;
  bool isWritingMath() const;
  
  void beginSinceBlock(const std::string& version);
  void endSinceBlock();
  
  void startParagraph();
  void endParagraph();

  void finish();

  void write(const std::shared_ptr<dom::Node>& node);

  dom::Content& output();

protected:
  ListWriter& currentList();
  dom::Paragraph& currentParagraph();
  MathWriter& currentMath();

  bool hasActiveNestedWriter(DocumentWriter** out);

private:
  State m_state = State::Idle;
  std::shared_ptr<ContentWriter> m_writer;
  std::optional<std::string> m_since;
  std::vector<std::shared_ptr<dom::Node>> m_nodes;
};

} // namespace dex

namespace dex
{

inline DocumentWriter::State DocumentWriter::state() const
{
  return m_state;
}

inline dom::Content& DocumentWriter::output()
{
  return m_nodes;
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_H
