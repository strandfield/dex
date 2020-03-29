// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_CONTENTWRITER_H
#define DEX_INPUT_CONTENTWRITER_H

#include "dex/dex-input.h"

#include <dom/paragraph.h>

#include <vector>

namespace dom
{
using Content = std::vector<std::shared_ptr<Node>>;
} // namespace dom

namespace dex
{

class DEX_INPUT_API ContentWriter
{
private:
  std::shared_ptr<dom::Node> m_output;

public:
  ContentWriter();
  virtual ~ContentWriter();

  virtual void write(char c) = 0;
  virtual void control(const std::string& cs);

  virtual void finish() = 0;

  std::shared_ptr<dom::Node> output() const;

protected:
  void setOutput(std::shared_ptr<dom::Node> out);

private:
  std::vector<std::shared_ptr<dom::Node>> m_nodes;
};

} // namespace dex

#endif // DEX_INPUT_CONTENTWRITER_H
