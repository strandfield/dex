// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/input/manual-parser.h"

#include "dex/input/paragraph-writer.h"
#include "dex/input/parser-errors.h"

#include "dex/common/logging.h"

namespace dex
{

ManualParser::Frame::Frame(FrameType ft)
  : state::Frame<FrameType>(ft)
{

}

ManualParser::Frame::Frame(FrameType ft, std::shared_ptr<dex::Sectioning> sec)
  : state::Frame<FrameType>(ft)
{
  section = sec;
  writer = std::make_shared<DocumentWriter>(sec);
}

ManualParser::ManualParser(std::shared_ptr<Document> doc)
  : m_document(doc)
{
  m_state.enter<FrameType::WritingManual>();
  m_state.current().document = doc;
  m_state.current().writer = std::make_shared<DocumentWriter>(doc);
}

ManualParser::State& ManualParser::state()
{
  return m_state;
}

std::shared_ptr<DocumentWriter> ManualParser::contentWriter()
{
  return currentFrame().writer;
}

void ManualParser::part(std::string name)
{
  exitFrame(Sectioning::Part);

  auto sec = std::make_shared<Sectioning>(dex::Sectioning::Part, std::move(name));

  m_state.enter<FrameType::WritingSection>(sec);
}

void ManualParser::chapter(std::string name)
{
  exitFrame(Sectioning::Chapter);

  auto sec = std::make_shared<Sectioning>(dex::Sectioning::Chapter, std::move(name));

  m_state.enter<FrameType::WritingSection>(sec);
}

void ManualParser::section(std::string name)
{
  exitFrame(Sectioning::Section);

  auto sec = std::make_shared<Sectioning>(dex::Sectioning::Section, std::move(name));

  m_state.enter<FrameType::WritingSection>(sec);
}

void ManualParser::beginFile()
{
  /* no-op */
}

void ManualParser::endFile()
{
  endBlock();
}

void ManualParser::beginBlock()
{
  /* no-op */
}

void ManualParser::endBlock()
{
  while (m_state.frames().size() > 1)
  {
    exitFrame();
  }
}

ManualParser::Frame& ManualParser::currentFrame()
{
  return m_state.current();
}

void ManualParser::exitFrame()
{
  Frame& f = m_state.current();

  if (f.section)
  {
    f.writer->finish();

    auto sec = f.section;
    m_state.leave();
    m_state.current().writer->write(sec);
  }
  else
  {
    m_state.leave();
  }
}

void ManualParser::exitFrame(dex::Sectioning::Depth d)
{
  while (m_state.frames().size() > 1 && currentFrame().section->depth >= d)
  {
    exitFrame();
  }
}

} // namespace dex
