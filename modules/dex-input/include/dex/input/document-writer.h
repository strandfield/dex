// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_H
#define DEX_INPUT_DOCUMENT_WRITER_H

#include "dex/dex-input.h"

#include "dex/common/state.h"

#include "dex/input/content-writer.h"

#include <dom/paragraph.h>

#include <optional>
#include <variant>

namespace dex
{

class ParagraphWriter;

class DEX_INPUT_API DocumentWriter
{
public:
  explicit DocumentWriter();

  enum class FrameType
  {
    Idle,
    WritingParagraph,
  };

  struct Frame : state::Frame<FrameType>
  {
    Frame(const Frame&) = delete;
    Frame(Frame&& f) = default;
    ~Frame() = default;

    explicit Frame(FrameType ft);

    std::variant<std::monostate, std::shared_ptr<ParagraphWriter>> data;
  };

  struct State : public state::State<Frame>
  {
    using state::State<Frame>::State;

    std::optional<std::string> since;
  };
  
  State& state();

  void begin();
  void write(char c);
  void write(const std::string& str);
  void end();

  bool isWritingParagraph();
  ParagraphWriter& paragraph();

  void beginSinceBlock(const std::string& version);
  void endSinceBlock();
  
  void write(const std::shared_ptr<dom::Node>& node);

  dom::Content& output();

protected:
  Frame& currentFrame();

  dom::Paragraph& currentParagraph();

  void startParagraph();
  void endParagraph();

private:
  State m_state;
  std::vector<std::shared_ptr<dom::Node>> m_nodes;
};

} // namespace dex

namespace dex
{

inline DocumentWriter::State& DocumentWriter::state()
{
  return m_state;
}

inline dom::Content& DocumentWriter::output()
{
  return m_nodes;
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_H
