// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MANUAL_H
#define DEX_MODEL_MANUAL_H

#include "dex/dex-model.h"

#include "dex/model/document.h"

namespace dex
{

class DEX_MODEL_API GroupTable : public DocumentElement
{
public:
  std::string groupname;

public:
  explicit GroupTable(std::string gname);

  static constexpr model::Kind ClassKind = model::Kind::GroupTable;
  model::Kind kind() const override;
};

class DEX_MODEL_API FrontMatter : public DocumentElement
{
public:
  FrontMatter();

  static constexpr model::Kind ClassKind = model::Kind::FrontMatter;
  model::Kind kind() const override;
};

class DEX_MODEL_API MainMatter : public DocumentElement
{
public:
  MainMatter();

  static constexpr model::Kind ClassKind = model::Kind::MainMatter;
  model::Kind kind() const override;
};

class DEX_MODEL_API BackMatter : public DocumentElement
{
public:
  BackMatter();

  static constexpr model::Kind ClassKind = model::Kind::BackMatter;
  model::Kind kind() const override;
};

class DEX_MODEL_API Sectioning : public DocumentElement
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
  DomNodeList content;

public:
  Sectioning(Depth d, std::string n);

  static constexpr model::Kind ClassKind = model::Kind::Sectioning;
  model::Kind kind() const override;

  const DomNodeList& childNodes() const override;
  void appendChild(const DocumentNodePtr& node) override;
 
  static std::string depth2str(Depth d);
  static Depth str2depth(const std::string& str);
};

class DEX_MODEL_API TableOfContents : public DocumentElement
{
public:
  TableOfContents();

  static constexpr model::Kind ClassKind = model::Kind::TableOfContents;
  model::Kind kind() const override;
};

class DEX_MODEL_API Index : public DocumentElement
{
public:
  Index();

  static constexpr model::Kind ClassKind = model::Kind::Index;
  model::Kind kind() const override;
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
