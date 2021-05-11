// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/manual.h"

namespace dex
{

model::Kind GroupTable::kind() const
{
  return ClassKind;
}

model::Kind Sectioning::kind() const
{
  return ClassKind;
}

const DomNodeList& Sectioning::childNodes() const
{
  return content;
}

void Sectioning::appendChild(const DocumentNodePtr& node)
{
  content.push_back(node);
}

GroupTable::GroupTable(std::string gname)
  : groupname(std::move(gname))
{

}

FrontMatter::FrontMatter()
{

}

model::Kind FrontMatter::kind() const
{
  return ClassKind;
}

MainMatter::MainMatter()
{

}

model::Kind MainMatter::kind() const
{
  return ClassKind;
}

BackMatter::BackMatter()
{

}

model::Kind BackMatter::kind() const
{
  return ClassKind;
}

Sectioning::Sectioning(Depth d, std::string n)
  : depth(d),
    name(std::move(n))
{

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

model::Kind TableOfContents::kind() const
{
  return ClassKind;
}

Index::Index()
{

}

model::Kind Index::kind() const
{
  return ClassKind;
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
