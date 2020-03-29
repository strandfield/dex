// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_PARSER_MODE_H
#define DEX_INPUT_PARSER_MODE_H

#include "dex/dex-input.h"

#include "dex/model/model.h"

#include "dex/input/functional.h"

#include <tex/token.h>

namespace dex
{

class ParserMachine;

class DEX_INPUT_API ParserMode
{
public:

  ParserMode() = delete;
  ParserMode(const ParserMode&) = delete;
  virtual ~ParserMode() = default;

  explicit ParserMode(ParserMachine& machine, ParserMode* parent = nullptr);

  ParserMachine& machine() const;
  FunctionCall& call() const;
  ParserMode* parent() const;

  bool done() const;

  virtual void beginFile() = 0;
  virtual void endFile() = 0;
  virtual void beginBlock() = 0;
  virtual void endBlock() = 0;

  virtual bool write(tex::parsing::Token&& tok) = 0;

  virtual void childFinished(ParserMode& mode);

  virtual const std::shared_ptr<dom::Node>& output() const;

  ParserMode& operator=(const ParserMode&) = delete;

protected:
  void setDone(bool done = true);

private:
  ParserMachine& m_machine;
  ParserMode* m_parent = nullptr;
  bool m_done = false;
};

} // namespace dex

#endif // DEX_INPUT_PARSER_MODE_H
