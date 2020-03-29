// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'dex' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <QTest>

class TestDexInput : public QObject
{
  Q_OBJECT
public:
  TestDexInput() = default;

private Q_SLOTS:
  void argumentParsing();
  void conditionalEvaluator();
  void documentWriterParagraph();
  void documentWriterList();
  void parserMachineList();
  void parserMachineClass();
  void parserMachineFunction();
};
