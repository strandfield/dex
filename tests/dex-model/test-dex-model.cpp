// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "test-dex-model.h"

#include "dex/model/display-math.h"

void TestDexModel::mathNormalization()
{
  std::string src = "\\alpha x + \\gamma \\frac {1}{2}";

  dex::DisplayMath::normalize(src, 0);

  QVERIFY(src == "\\alpha x + \\gamma\\frac{1}{2}");
}
