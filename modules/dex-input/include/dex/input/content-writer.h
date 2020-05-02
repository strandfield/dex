// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_INPUT_CONTENTWRITER_H
#define DEX_INPUT_CONTENTWRITER_H

#include "dex/dex-input.h"

#include <dom/paragraph.h>

#include <optional>

namespace dex
{

inline bool is_space(char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

class DEX_INPUT_API ContentWriter
{
private:
  std::shared_ptr<dom::Node> m_output;

public:
  ContentWriter();
  ContentWriter(const ContentWriter&) = delete;
  virtual ~ContentWriter();

  virtual void write(char c) = 0;

  virtual bool image(std::string& src, std::optional<int>& width, std::optional<int>& height);

  virtual bool li(std::optional<std::string>& marker, std::optional<int>& value);
  virtual bool endlist();

  virtual void finish() = 0;

  std::shared_ptr<dom::Node> output() const;

  ContentWriter& operator=(const ContentWriter&) = delete;

protected:
  void setOutput(std::shared_ptr<dom::Node> out);

private:
  std::vector<std::shared_ptr<dom::Node>> m_nodes;
};

} // namespace dex

#endif // DEX_INPUT_CONTENTWRITER_H
