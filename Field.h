#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Gems.h"
#include "Grid.h"

int const wait = 1;

class Field : public QWidget {
public:
  Field(QWidget* parent = 0);
  ~Field() {
    delete bonuses;
  }
protected:
  void paintEvent(QPaintEvent* event);
  void mousePressEvent(QMouseEvent* e);
private:
  int pickedX;
  int pickedY;
  std::shared_ptr<GridForBlocks> blocks;
  GridForBonuses* bonuses;
  void colorBonus(int i, int j);
  void bombBonus(int i, int j);
  bool doBonuses();
  void animateBonus(int i, int j);
  void animateColorChange(int i, int j, QColor color);
  bool ClearAndDoBonuses();
  void swapBlocks(int iA, int jA, int iB, int jB);
  void animateFall(int row);
  bool killRow(int row);
  bool Clear();
};


