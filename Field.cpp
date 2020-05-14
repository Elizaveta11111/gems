#include "Field.h"

#include <QApplication>
#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QMouseEvent>
#include <QThread>

void Field::animateFall(int row) {
  int i, j, k;
  for (k = 0; k < blockh/2; k++) {
    for (i = 0; i < COLUMNS; i++) {
      for (j = -1; j <= row; j++)
        blocks->inPos(i,j)->animate();
    }
    repaint();
    QThread::msleep(wait);
  }
  blocks->fall(row);
}

bool Field::killRow(int row) {
  int bonus;
  if (row < 0)
    return false;
  int i, j, k;
  bool notEmpthy = false;
  for (i = 0; i < COLUMNS; i++)
    for (j = row; j >= 0; j--)
      if (blocks->inPos(i, j)->isDead()) {
        bonus = rand() % 80;
        if (bonus == 1) {
          colorBonus(i, j);
          animateBonus(i, j);
        }
        if (bonus == 2) {
          bombBonus(i, j);
          animateBonus(i, j);
        }
        for (k = -1; k <= j; k++) 
          blocks->changedy(i, k);
        notEmpthy = true;
        j = -1;
      }
  if (notEmpthy)
    animateFall(row);
  return notEmpthy;
}

void Field::swapBlocks(int iA, int jA, int iB, int jB) {
  int to = blocks->startSwap(iA, jA, iB, jB);
  for (int j = 0; j < to; j++) {
    blocks->move(iA, jA);
    blocks->move(iB, jB);
    QThread::msleep(wait);
    repaint();
  }
  blocks->stopSwap(iA, jA, iB, jB);
}

void Field::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    int x = e->x();
    int y = e->y();
    int i = x / blockw;
    int j = y / blockh;
    if (pickedX < 0) {
      blocks->pickBlock(i, j);
      pickedX = i;
      pickedY = j;
    }
    else if (blocks->areNeigbors(i, j, pickedX, pickedY)) {
      int dead = 0;
      blocks->unpickBlock(pickedX, pickedY);
      swapBlocks(i, j, pickedX, pickedY);
      dead += blocks->check(i, j, 0);
      dead += blocks->check(pickedX, pickedY, 0);
      if (dead == 0)
        swapBlocks(pickedX, pickedY, i, j);
      j += 2;
      if (j >= ROWS)
        j = ROWS - 1;
      while (killRow(j));
      while (ClearAndDoBonuses());
      pickedX = -1;
    }
    else {
      blocks->unpickBlock(pickedX, pickedY);
      pickedX = -1;
    }
    repaint();
  }
}

Field::Field(QWidget* parent) : QWidget(parent) {
  pickedX = -1;
  pickedY = -1;
  std::shared_ptr<GridForBlocks> temp(new GridForBlocks);
  blocks = temp;
  bonuses = new(GridForBonuses);
  while (Clear());
}

bool Field::Clear() {
  bool didSmth = false;
  bool foundSmth = true;
  int i, j;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS; j++)
      if (!blocks->inPos(i, j)->isDead()) {
        foundSmth = blocks->check(i, j, 0);
        if (!didSmth)
          didSmth = foundSmth;
      }
  for (i = ROWS - 1; i >= 0; i--)
    while (killRow(i));
  return didSmth;
}

bool Field::ClearAndDoBonuses() {
  bool a = Clear();
  bool b = doBonuses();
  return a || b;
}

void Field::paintEvent(QPaintEvent* e) {
  Q_UNUSED(e);
  QPainter qp(this);
  for (int i = 0; i < COLUMNS; i++)
    for (int j = -1; j < ROWS; j++)
      blocks->drawBlock(i, j, &qp);
}

int randClose(int s, int limit) {
  int a = s + (rand() - rand()) % 3;
  if (a < 0)
    a = -a;
  if (a > limit)
    a = a - limit;
  return a;
}

void Field::colorBonus(int i, int j) {
  int rx, ry;
  int tx, ty;
  int sx, sy;
  rx = randClose(i, COLUMNS - 1);
  ry = randClose(j, ROWS - 1);
  bonuses->setBonus(rx, ry, colorNumber(blocks->inPos(i, j)->color));
  do {
    tx = rand() % COLUMNS;
    ty = rand() % ROWS;
  } while (bonuses->areNeigbors(rx, ry, tx, ty));
  bonuses->setBonus(tx, ty, colorNumber(blocks->inPos(i, j)->color));
  do {
    sx = rand() % COLUMNS;
    sy = rand() % ROWS;
  } while (bonuses->areNeigbors(sx, sy, tx, ty) || bonuses->areNeigbors(rx, ry, sx, sy));
  bonuses->setBonus(sx, sy, colorNumber(blocks->inPos(i, j)->color));
}

void Field::bombBonus(int i, int j) {
  int rx, ry;
  int s;
  rx = randClose(i, COLUMNS - 1);
  ry = randClose(j, ROWS - 1);
  bonuses->setBonus(rx, ry, -2);
  for (s = 0; s < 4; ) {
    rx = rand() % COLUMNS;
    ry = rand() % ROWS;
    if (bonuses->inPos(rx, ry == -1)) {
      bonuses->setBonus(rx, rx, -2);
      s++;
    }
  }
}

bool Field::doBonuses() {
  int i, j;
  bool didSmth = false;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS; j++) {
      if (bonuses->inPos(i, j) > 0) {
        didSmth = true;
        animateColorChange(i, j, colorColor(bonuses->inPos(i, j)));
        bonuses->setBonus(i, j, -1);
      }
      if (bonuses->inPos(i, j) == -2) {
        didSmth = true;
        blocks->dead(i, j);
        bonuses->setBonus(i, j, -1);
      }
    }
  return didSmth;
}

void Field::animateBonus(int i, int j) {
  int m;
  blocks->bonus(i, j);
  for (m = 0; m < 50; m++) {
    blocks->inPos(i, j)->animate(50);
    QThread::msleep(2 * wait);
    repaint();
  }
  for (m = 0; m < 50; m++) {
    blocks->inPos(i, j)->animate(-50);
    QThread::msleep(2 * wait);
    repaint();
  }
  blocks->notbonus(i, j);
}

void Field::animateColorChange(int i, int j, QColor color) {
  int m;
  blocks->twoColors(i, j, color);
  for (m = 0; m < 50; m++) {
    blocks->inPos(i, j)->animate(50);
    QThread::msleep(2);
    repaint();
  }
  blocks->oneColor(i, j);
}