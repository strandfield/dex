// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/manual.h"

namespace dex
{

const std::string GroupTable::TypeId = "grouptable";

const std::string& GroupTable::className() const
{
  return TypeId;
}

const std::string Sectioning::TypeId = "sectioning";

const std::string& Sectioning::className() const
{
  return TypeId;
}

GroupTable::GroupTable(std::string gname)
  : groupname(std::move(gname))
{

}

Sectioning::Sectioning(Depth d, std::string n)
  : depth(d),
    name(std::move(n))
{

}

const dom::NodeList& Sectioning::childNodes() const
{
  return content;
}

void Sectioning::appendChild(std::shared_ptr<Node> n)
{
  removeFromParent(n);
  append(content, n);
  registerChild(n);
}

void Sectioning::removeChild(std::shared_ptr<Node> n)
{
  remove(content, n);
}

std::string Sectioning::depth2str(Depth d)
{
  switch (d)
  {
  case dex::Sectioning::Part:
    return "part";
  case dex::Sectioning::Chapter:
    return "chapter";
  case dex::Sectioning::Section:
    return "section";
  default:
    return "chapter";
  }
}

Sectioning::Depth Sectioning::str2depth(const std::string& str)
{
  if (str == "section")
    return Section;
  else if (str == "part")
    return Part;
  return Chapter;
}

TableOfContents::TableOfContents()
{

}

const std::string TableOfContents::TypeId = "TableOfContents";

const std::string& TableOfContents::className() const
{
  return TypeId;
}

Manual::Manual(std::string t)
  : Document(std::move(t))
{
  doctype = "manual";
}

Page::Page(std::string t)
  : Document(std::move(t))
{
  doctype = "page";
}

} // namespace dex
