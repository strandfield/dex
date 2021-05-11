// Copyright (C) 2019-2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_H
#define DEX_INPUT_DOCUMENT_WRITER_H

#include "dex/dex-input.h"

#include "dex/common/state.h"

#include "dex/input/dom-writer.h"

#include "dex/model/document.h"

#include <optional>
#include <variant>

namespace dex
{

class DocumentNode;

class BeginSince;

class DomWriter;
class MathWriter;
class ParagraphWriter;

class DEX_INPUT_API DocumentWriter
{
public:
  explicit DocumentWriter(std::shared_ptr<dex::DocumentNode> out = nullptr);
  DocumentWriter(DocumentWriter&&) = delete;
  ~DocumentWriter();

  enum class State
  {
    Idle,
    WritingParagraph,
    WritingList,
    WritingListItem,
    WritingMath,
    WritingCode,
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

  void href(std::string link, const std::string& text);

  void index(std::string key);

  void image(std::string src, std::optional<int> width, std::optional<int> height);

  void list();
  void list(const std::optional<std::string>& marker, std::optional<bool> ordered, std::optional<bool> reversed);
  void li(std::optional<std::string> marker, std::optional<int> value);
  void endlist();

  void displaymath();
  void enddisplaymath();

  void makegrouptable(std::string groupname);

  void code(const std::string& lang = "");
  void endcode();

  bool isIdle() const;

  bool isWritingParagraph() const;
  ParagraphWriter& paragraphWriter();

  bool isWritingList() const;
  bool isWritingListItem() const;
  bool isWritingMath() const;
  bool isWritingCode() const;

  void beginSinceBlock(const std::string& version);
  void endSinceBlock();

  void finish();

  void write(const std::shared_ptr<dex::DocumentNode>& node);

  const std::shared_ptr<dex::DocumentNode>& output();
  void setOutput(std::shared_ptr<dex::DocumentNode> out);

protected:

  void startParagraph();
  void endParagraph();

  MathWriter& currentMath();

  dex::DocumentNode& currentNode();
  std::shared_ptr<dex::DocumentNode> currentNodeShared();

  void adjustState();

  void pushNode(std::shared_ptr<dex::DocumentNode> n);
  void popNode();

private:
  State m_state = State::Idle;
  std::unique_ptr<ParagraphWriter> m_paragraph_writer;
  std::unique_ptr<MathWriter> m_math_writer;
  std::shared_ptr<BeginSince> m_since;
  std::shared_ptr<dex::DocumentNode> m_result;
  std::vector<std::shared_ptr<dex::DocumentNode>> m_nodes;
};

} // namespace dex

namespace dex
{

inline DocumentWriter::State DocumentWriter::state() const
{
  return m_state;
}

inline const std::shared_ptr<dex::DocumentNode>& DocumentWriter::output()
{
  return m_result;
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_H
