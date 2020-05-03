// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/model.h"

#include <cxx/class.h>
#include <cxx/enum.h>
#include <cxx/function.h>
#include <cxx/namespace.h>

#include <dom/image.h>
#include <dom/list.h>
#include <dom/paragraph.h>

#include <stdexcept>

namespace dex
{

struct ChildNodeGetter
{
  const Model::PathElement& path;

  Model::Node operator()(std::shared_ptr<cxx::Program> prog)
  {
    if (path.name == "global_namespace")
      return prog->globalNamespace();

    throw std::runtime_error{ "Invalid model path" };
  }

  Model::Node operator()(std::shared_ptr<cxx::Entity> e)
  {
    if (path.name == "documentation")
      return e->documentation();

    if (e->is<cxx::Namespace>())
    {
      auto ns = std::static_pointer_cast<cxx::Namespace>(e);

      if (path.name == "entities")
        return ns->entities().at(path.index);
    }
    else if (e->is<cxx::Class>())
    {
      auto cla = std::static_pointer_cast<cxx::Class>(e);

      if (path.name == "members")
        return cla->members().at(path.index).first;
    }
    else if (e->is<cxx::Enum>())
    {
      auto en = std::static_pointer_cast<cxx::Enum>(e);

      if (path.name == "values")
        return en->values().at(path.index);

    }
    else if (e->is<cxx::Function>())
    {
      auto fn = std::static_pointer_cast<cxx::Function>(e);

      if (path.name == "parameters")
        return fn->parameters().at(path.index);
    }

    throw std::runtime_error{ "Invalid model path" };
  }

  Model::Node operator()(std::shared_ptr<cxx::Documentation> doc)
  {

    if (std::dynamic_pointer_cast<dex::EntityDocumentation>(doc))
    {
      auto entdoc = std::static_pointer_cast<dex::EntityDocumentation>(doc);

      if (path.name == "description")
        return entdoc->description().at(path.index);
    }

    throw std::runtime_error{ "Invalid model path" };
  }

  Model::Node operator()(std::shared_ptr<dom::Node> node)
  {
    if (node->is<dom::List>())
    {
      auto list = std::static_pointer_cast<dom::List>(node);

      if (path.name == "items")
        return list->items.at(path.index);
    }
    else if (node->is<dom::ListItem>())
    {
      auto listitem = std::static_pointer_cast<dom::ListItem>(node);

      if (path.name == "content")
        return listitem->content.at(path.index);
    }
    else if (node->is<dex::Manual>())
    {
      auto man = std::static_pointer_cast<dex::Manual>(node);

      if (path.name == "content")
        return man->content.at(path.index);
    }
    else if (node->is<dex::Sectioning>())
    {
      auto sec = std::static_pointer_cast<dex::Sectioning>(node);

      if (path.name == "content")
        return sec->content.at(path.index);
    }

    throw std::runtime_error{ "Invalid model path" };
  }
};

static Model::Node get_child_node(const Model::Node& node, const Model::PathElement& pe)
{
  ChildNodeGetter getter{ pe };
  return std::visit(getter, node);
}


Model::PathElement::PathElement(std::string n)
  : name(std::move(n))
{

}

Model::PathElement::PathElement(std::string n, size_t i)
  : name(std::move(n)),
  index(i)
{

}

bool Model::empty() const
{
  return m_program == nullptr && m_manuals.empty();
}

Model::Node Model::get(const Path& path) const
{
  Model::Node result;

  if (path.empty())
    return Model::Node();

  if (path.front().name == "program")
  {
    result = program();
  }
  else if (path.front().name == "manuals")
  {
    result = manuals().at(path.front().index);
  }
  
  for (size_t i(1); i < path.size(); ++i)
  {
    result = get_child_node(result, path.at(i));
  }

  return result;
}

} // namespace dex
