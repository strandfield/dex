// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_MODELBASE_H
#define DEX_MODEL_MODELBASE_H

#include "dex/dex-model.h"

#include <memory>
#include <string>

namespace dex
{

namespace model
{

enum class Kind
{
  Model,
  /* Document */
  Document,
  Manual,
  Page,
  Paragraph,
  Link,
  TextStyle,
  Note,
  List,
  ListItem,
  Image,
  GroupTable,
  FrontMatter,
  MainMatter,
  BackMatter,
  Sectioning,
  TableOfContents,
  Index,
  DisplayMath,
  Since,
  BeginSince,
  EndSince,
  CodeBlock,
  InlineMath,
  IndexEntry,
  /* Program */
  Program,
  Namespace,
  Enum,
  Class,
  Function,
  Variable,
};

class DEX_MODEL_API Object : public std::enable_shared_from_this<Object>
{
public:
  virtual ~Object();

  virtual Kind kind() const = 0;

  template<typename T>
  bool is() const
  {
    return kind() == T::ClassKind;
  }

  virtual bool isDocument() const;
  virtual bool isDocumentNode() const;
  virtual bool isDocumentElement() const;

  virtual std::string className() const;
};

DEX_MODEL_API std::string to_string(Kind k);

} // namespace model

} // namespace dex

#endif // DEX_MODEL_MODELBASE_H
