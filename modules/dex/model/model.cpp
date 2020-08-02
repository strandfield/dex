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
      return e->documentation;

    if (e->is<cxx::Namespace>())
    {
      auto ns = std::static_pointer_cast<cxx::Namespace>(e);

      if (path.name == "entities")
        return ns->entities.at(path.index);
    }
    else if (e->is<cxx::Class>())
    {
      auto cla = std::static_pointer_cast<cxx::Class>(e);

      if (path.name == "members")
        return cla->members.at(path.index);
    }
    else if (e->is<cxx::Enum>())
    {
      auto en = std::static_pointer_cast<cxx::Enum>(e);

      if (path.name == "values")
        return en->values.at(path.index);

    }
    else if (e->is<cxx::Function>())
    {
      auto fn = std::static_pointer_cast<cxx::Function>(e);

      if (path.name == "parameters")
        return fn->parameters.at(path.index);
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

std::string Model::to_string(const Path& p)
{
  if (p.empty())
    return "$";

  std::string result = "$";

  for (size_t i(0); i < p.size(); ++i)
  {
    if (p.at(i).index != std::numeric_limits<size_t>::max())
    {
      result += "." + p.at(i).name + "[" + std::to_string(p.at(i).index) + "]";
    }
    else
    {
      result += "." + p.at(i).name;
    }
  }

  return result;
}

inline static std::string next_token(const std::string& path, size_t index)
{
  auto is_delim = [](char c) { return c == '.' || c == '[' || c == ']';  };

  const size_t start = index;

  while (index < path.size() && !is_delim(path.at(index))) ++index;

  return std::string(path.begin() + start, path.begin() + index);
}


Model::Path Model::parse_path(const std::string& path)
{
  assert(!path.empty());

  Model::Path result;

  if (path == "$")
    return result;

  size_t index = 2;
  auto is_num = [](char c) { return c >= '0' && c <= '9'; };

  do
  {
    std::string token = next_token(path, index);

    index += token.size() + 1;

    if (!token.empty())
    {
      if (!is_num(token.front()))
        result.push_back(std::move(token));
      else
        result.back().index = static_cast<size_t>(std::stoi(token));
    }
  } while (index < path.size());

  return result;
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

Model::Path Model::path(const std::shared_ptr<cxx::Entity>& e) const
{
  if (e == m_program->globalNamespace())
    return { PathElement("program"), PathElement("global_namespace") };

  Path result;

  auto current_entity = e;
  auto parent_entity = e->parent();

  while (parent_entity != nullptr)
  {
    if (parent_entity->is<cxx::Class>())
    {
      const cxx::Class& parent_class = static_cast<const cxx::Class&>(*parent_entity);
      auto it = std::find(parent_class.members.begin(), parent_class.members.end(), current_entity);
      result.push_back(PathElement("members", std::distance(parent_class.members.begin(), it)));
    }
    else if (parent_entity->is<cxx::Namespace>())
    {
      const auto& parent_namespace = static_cast<const cxx::Namespace&>(*parent_entity);
      auto it = std::find(parent_namespace.entities.begin(), parent_namespace.entities.end(), current_entity);
      result.push_back(PathElement("entities", std::distance(parent_namespace.entities.begin(), it)));
    }
    else
    {
      throw std::runtime_error{ "Could not compute entitiy's path" };
    }

    current_entity = parent_entity;
    parent_entity = current_entity->parent();
  }

  result.push_back(PathElement("global_namespace"));
  result.push_back(PathElement("program"));

  std::reverse(result.begin(), result.end());

  return result;
}

} // namespace dex
