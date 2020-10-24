// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_DISPLAYMATH_H
#define DEX_MODEL_DISPLAYMATH_H

#include "dex/dex-model.h"

#include <tex/math/mathlist.h>

#include <dom/element.h>

namespace dex
{

class DEX_MODEL_API DisplayMath : public dom::Element
{

public:
  std::string source;
  tex::MathList mlist;

public:
  DisplayMath();

  static void normalize(std::string& str, size_t offset);
  
  void normalize();

  static const std::string TypeId;
  const std::string& className() const override;
};

} // namespace dex

#endif // DEX_MODEL_DISPLAYMATH_H
