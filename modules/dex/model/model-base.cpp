// Copyright (C) 2021 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model-base.h"

#include <stdexcept>

namespace dex
{

namespace model
{

Object::~Object()
{

}

bool Object::isDocument() const
{
  return false;
}

bool Object::isDocumentNode() const
{
  return false;
}

bool Object::isDocumentElement() const
{
  return false;
}

std::string Object::className() const
{
  return model::to_string(kind());
}

std::string to_string(Kind k)
{
  switch (k)
  {
  case Kind::Model: return "Model";
  case Kind::Document: return "document";
  case Kind::Manual: return "manual";
  case Kind::Page: return "page";
  case Kind::Paragraph: return "paragraph";
  case Kind::Link: return "link";
  case Kind::TextStyle: return "textstyle";
  case Kind::Note: return "note";
  case Kind::List: return "list";
  case Kind::ListItem: return "listitem";
  case Kind::Image: return "image";
  case Kind::GroupTable: return "grouptable";
  case Kind::FrontMatter: return "FrontMatter";
  case Kind::MainMatter: return "MainMatter";
  case Kind::BackMatter: return "BackMatter";
  case Kind::Sectioning: return "sectioning";
  case Kind::TableOfContents: return "TableOfContents";
  case Kind::Index: return "Index";
  case Kind::DisplayMath: return "math";
  case Kind::Since: return "since";
  case Kind::BeginSince: return "BeginSince";
  case Kind::EndSince: return "EndSince";
  case Kind::CodeBlock: return "CodeBlock";
  case Kind::InlineMath: return "inline-math";
  case Kind::IndexEntry: return "ParIndexEntry";
  case Kind::Program: return "Program";
  case Kind::Namespace: return "Namespace";
  case Kind::Enum: return "Enum";
  case Kind::Class: return "Class";
  case Kind::Function: return "Function";
  case Kind::Variable: return "Variable";
  default: return "undefined";
  }
}

} // namespace model

} // namespace dex
