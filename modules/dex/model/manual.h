// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MANUAL_H
#define DEX_MODEL_MANUAL_H

#include "dex/dex-model.h"

#include <dom/document.h>

namespace dex
{

class DEX_MODEL_API GroupTable : public dom::Element
{
public:
  std::string groupname;

public:
  explicit GroupTable(std::string gname);

  static const std::string TypeId;
  const std::string& className() const override;
};

class DEX_MODEL_API Sectioning : public dom::Element
{
public:

  enum Depth
  {
    Part = -1,
    Chapter,
    Section,
  };

public:
  Depth depth;
  std::string name;
  dom::NodeList content;

public:
  Sectioning(Depth d, std::string n);

  static const std::string TypeId;
  const std::string& className() const override;

  const dom::NodeList& childNodes() const override;
  void appendChild(std::shared_ptr<Node> n) override;
  void removeChild(std::shared_ptr<Node> n) override;

  static std::string depth2str(Depth d);
  static Depth str2depth(const std::string& str);
};

class DEX_MODEL_API Document : public dom::Document
{
public:
  using dom::Document::Document;
};

class DEX_MODEL_API Manual : public Document
{
public:
  explicit Manual(std::string title = "");
};

class DEX_MODEL_API Page : public Document
{
public:
  explicit Page(std::string title = "");
};

} // namespace dex

#endif // DEX_MODEL_MANUAL_H
