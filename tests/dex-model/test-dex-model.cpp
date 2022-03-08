// Copyright (C) 2020-2022 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "dex/model/display-math.h"

#include "catch.hpp"

TEST_CASE("Testing math normalization", "[model]")
{
  std::string src = "\\alpha x + \\gamma \\frac {1}{2}";

  dex::DisplayMath::normalize(src, 0);

  REQUIRE(src == "\\alpha x + \\gamma \\frac{1}{2}");
}
