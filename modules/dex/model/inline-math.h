// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef DEX_MODEL_INLINE_MATH_H
#define DEX_MODEL_INLINE_MATH_H

#include "dex/dex-model.h"

#include "dex/model/model-base.h"

#include <tex/math/mathlist.h>

#include <string>

namespace dex
{

class DEX_MODEL_API InlineMath
{
public:
  tex::MathList mlist;

public:
  InlineMath();

  static constexpr model::Kind ClassKind = model::Kind::InlineMath;
};

} // namespace dex

#endif // DEX_MODEL_INLINE_MATH_H
