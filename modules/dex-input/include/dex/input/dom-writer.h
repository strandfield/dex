// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_DOMWRITER_H
#define DEX_INPUT_DOMWRITER_H

#include "dex/dex-input.h"

#include <dom/paragraph.h>

#include <optional>

namespace dex
{

inline bool is_space(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

class DEX_INPUT_API DomWriter
{
private:
  std::shared_ptr<dom::Node> m_output;

public:
  DomWriter();
  DomWriter(const DomWriter&) = delete;
  virtual ~DomWriter();

  virtual void write(char c) = 0;

  virtual void finish() = 0;

  std::shared_ptr<dom::Node> output() const;

  DomWriter& operator=(const DomWriter&) = delete;

protected:
  void setOutput(std::shared_ptr<dom::Node> out);
};

} // namespace dex

#endif // DEX_INPUT_DOMWRITER_H
