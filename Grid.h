#pragma once
#include <iostream>
#include <QtWidgets/QMainWindow>
#include "ui_Gems.h"
#include "Block.h"

#define COLORS 6
#define SIZEX 600
#define SIZEY 720
#define WAIT 1

enum class Bonus {
  COL0,
  COL1,
  COL2,
  COL3,
  COL4,
  COL5,
  BOMB,
  NOTHING
};

class Grid {
public:
  int const rows = 10;
  int const columns = 12;
  bool areNeighbors(int rx, int ry, int tx, int ty);
};

class GridForBonuses : public Grid {
public:
  GridForBonuses();
  ~GridForBonuses() {}
  void setBonus(int i, int j, Bonus bonus) {
    grid[i][j] = bonus;
  }
  Bonus inPos(int i, int j) {
    return grid[i][j];
  }
private:
  Bonus grid[COLUMNS][ROWS];
};

class GridForBlocks : public Grid {
public:
  GridForBlocks();
  ~GridForBlocks(){}
  int startSwap(int iA, int jA, int iB, int jB);
  void stopSwap(int iA, int jA, int iB, int jB);
  void moveY(int i, int j);
  void move(int i, int j);
  void changedy(int i, int j);
  void fall(int row);
  void pickBlock(int i, int j);
  void unpickBlock(int i, int j);
  void dead(int i, int j);
  void alive(int i, int j);
  void bonus(int i, int j);
  void notbonus(int i, int j);
  void twoColors(int i, int j, QColor color);
  void oneColor(int i, int j);
  void swap(int col, std::shared_ptr <Block> temp);
  void moveDown(int row, int col);
  std::shared_ptr <Block> inPos(int i, int j) {
    return grid[i][j + 1];
  }
  void stop();
  int check(int i, int j, int deep);
  void drawBlock(int i, int j, QPainter* qp);
private:
  std::shared_ptr <Block> grid[COLUMNS][ROWS + 1];
};