// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_MANUAL_PARSER_H
#define DEX_INPUT_MANUAL_PARSER_H

#include "dex/dex-input.h"

#include "dex/input/document-writer.h"

#include "dex/common/state.h"

#include "dex/model/manual.h"

namespace dex
{

class DEX_INPUT_API ManualParser
{
public:
  explicit ManualParser(std::shared_ptr<Manual> man);

  enum class FrameType
  {
    Idle, // unused
    WritingManual,
    WritingSection,
  };

  struct Frame : state::Frame<FrameType>
  {
    Frame(const Frame&) = delete;
    Frame(Frame&& f) = default;
    ~Frame() = default;

    explicit Frame(FrameType ft);
    Frame(FrameType ft, std::shared_ptr<dex::Sectioning> sec);

    std::shared_ptr<dex::Manual> manual;
    std::shared_ptr<dex::Sectioning> section;
    std::shared_ptr<DocumentWriter> writer;
  };

  using State = state::State<Frame>;
  
  State& state();
  
  // @TODO: rework these, they may not belong here
  void beginFile();
  void endFile();
  void beginBlock();
  void endBlock();

  std::shared_ptr<DocumentWriter> contentWriter();

  void part(std::string name);
  void chapter(std::string name);
  void section(std::string name);

protected:
  Frame& currentFrame();
  void exitFrame();
  void exitFrame(dex::Sectioning::Depth d);

private:
  State m_state;
  std::shared_ptr<Manual> m_manual;
};

} // namespace dex

#endif // DEX_INPUT_MANUAL_PARSER_H