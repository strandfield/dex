// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/manual.h"

namespace dex
{

const std::string GroupTable::TypeId = "grouptable";

const std::string& GroupTable::type() const
{
  return TypeId;
}

const std::string Sectioning::TypeId = "sectioning";

const std::string& Sectioning::type() const
{
  return TypeId;
}

const std::string Manual::TypeId = "manual";

const std::string& Manual::type() const
{
  return TypeId;
}

GroupTable::GroupTable(std::string gname, std::string tname)
  : groupname(std::move(gname)),
    templatename(std::move(tname))
{

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

Manual::Manual(std::string t)
  : title(std::move(t))
{

}


} // namespace dex
