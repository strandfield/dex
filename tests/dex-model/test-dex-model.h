// Copyright (C) 2020 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <QTest>

class TestDexModel : public QObject
{
  Q_OBJECT
public:
  TestDexModel() = default;

private Q_SLOTS:
  void mathNormalization();
};
