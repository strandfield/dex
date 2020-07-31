// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_PROGRAM_H
#define DEX_MODEL_PROGRAM_H

#include "dex/dex-model.h"

#include <cxx/class.h>
#include <cxx/function.h>
#include <cxx/program.h>

#include <unordered_map>

namespace dex
{

class DEX_MODEL_API RelatedNonMembers
{
public:

  struct Entry
  {
    std::shared_ptr<cxx::Class> the_class;
    std::vector<std::shared_ptr<cxx::Function>> non_members;

    Entry(const std::shared_ptr<cxx::Class>& c)
      : the_class(c)
    {

    }
  };

  std::unordered_map<std::shared_ptr<cxx::Class>, std::shared_ptr<Entry>> class_map;
  std::unordered_map<std::shared_ptr<cxx::Function>, std::shared_ptr<Entry>> functions_map;

  bool empty() const;

  void relates(const std::shared_ptr<cxx::Function>& f, const std::shared_ptr<cxx::Class>& c);

  std::shared_ptr<Entry> getRelated(const std::shared_ptr<cxx::Class>& c) const;
  std::shared_ptr<Entry> getRelated(const std::shared_ptr<cxx::Function>& f) const;
};

class DEX_MODEL_API Program : public cxx::Program
{
public:
  using cxx::Program::Program;

public:
  RelatedNonMembers related;
};

} // namespace dex

#endif // DEX_MODEL_PROGRAM_H
