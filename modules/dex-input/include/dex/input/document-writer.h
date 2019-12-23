// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOCUMENT_WRITER_H
#define DEX_INPUT_DOCUMENT_WRITER_H

#include "dex/dex-input.h"

#include <dom/paragraph.h>

#include <optional>
#include <variant>

namespace dex
{

class DEX_INPUT_API DocumentWriter
{
public:
  explicit DocumentWriter();

  struct State
  {
    State() = default;
    State(const State&) = delete;
    State(State&&) = default;

    enum FrameType
    {
      Idle,
      WritingParagraph,
    };

    enum FrameSubType
    {
      FST_None = 0,
    };

    struct Frame
    {
      Frame(const Frame&) = delete;
      Frame(Frame&& f) = default;
      ~Frame() = default;

      explicit Frame(FrameType ft);

      FrameType type;
      FrameSubType subtype;

      std::variant<std::monostate, std::shared_ptr<dom::Paragraph>> data;
    };

    std::vector<Frame> frames;
    std::optional<std::string> since;
  };
  
  State& state();

  void begin();
  void write(char c);
  void write(const std::string& str);
  void end();

  bool isWritingParagraph();

  void writeLink(std::string url, const std::string& text);
  void writeStyledText(std::string style, const std::string& text);

  void beginSinceBlock(const std::string& version);
  void endSinceBlock();
  void writeSince(const std::string& version, const std::string& text);
  
  void add(const std::shared_ptr<dom::Node>& node);

  std::vector<std::shared_ptr<dom::Node>>& output();

protected:
  State::Frame& currentFrame();

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

inline std::vector<std::shared_ptr<dom::Node>>& DocumentWriter::output()
{
  return m_nodes;
}

} // namespace dex

#endif // DEX_INPUT_DOCUMENT_WRITER_H
