#include "Field.h"

#include <QApplication>
#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QMouseEvent>
#include <QThread>

enum class BonusType{
  COLOR,
  BOMB
};

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
  BonusType bonus;
  int const probability = 80;
  if (row < 0)
    return false;
  int i, j, k;
  bool notEmpty = false;
  for (i = 0; i < COLUMNS; i++)
    for (j = row; j >= 0; j--)
      if (blocks->inPos(i, j)->isDead()) {
        bonus = static_cast<BonusType>(rand() % probability);
        if (bonus == BonusType::COLOR) {
          colorBonus(i, j);
          animateBonus(i, j);
        }
        if (bonus == BonusType::BOMB) {
          bombBonus(i, j);
          animateBonus(i, j);
        }
        for (k = -1; k <= j; k++) 
          blocks->changedy(i, k);
        notEmpty = true;
        break;
      }
  if (notEmpty)
    animateFall(row);
  return notEmpty;
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
    else if (blocks->areNeighbors(i, j, pickedX, pickedY)) {
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
  int const distance = 3;
  int pos = s + (rand() - rand()) % distance;
  if (pos < 0)
    pos = -pos;
  if (pos > limit)
    pos = pos - limit;
  return pos;
}

void Field::colorBonus(int i, int j) {
  int rx, ry;
  int tx, ty;
  int sx, sy;
  rx = randClose(i, COLUMNS - 1);
  ry = randClose(j, ROWS - 1);
  bonuses->setBonus(rx, ry, static_cast<Bonus>((colorNumber(blocks->inPos(i, j)->getColor()))));
  do {
    tx = rand() % COLUMNS;
    ty = rand() % ROWS;
  } while (bonuses->areNeighbors(rx, ry, tx, ty));
  bonuses->setBonus(tx, ty, static_cast<Bonus>(colorNumber(blocks->inPos(i, j)->getColor())));
  do {
    sx = rand() % COLUMNS;
    sy = rand() % ROWS;
  } while (bonuses->areNeighbors(sx, sy, tx, ty) || bonuses->areNeighbors(rx, ry, sx, sy));
  bonuses->setBonus(sx, sy, static_cast<Bonus>(colorNumber(blocks->inPos(i, j)->getColor())));
}

void Field::bombBonus(int i, int j) {
  int rx, ry;
  int s;
  int const bomsNum = 4;
  rx = randClose(i, COLUMNS - 1);
  ry = randClose(j, ROWS - 1);
  bonuses->setBonus(rx, ry, Bonus::BOMB);
  for (s = 0; s < bomsNum; ) {
    rx = rand() % COLUMNS;
    ry = rand() % ROWS;
    if (bonuses->inPos(rx, ry) == Bonus::NOTHING) {
      bonuses->setBonus(rx, ry, Bonus::BOMB);
      s++;
    }
  }
}

bool Field::doBonuses() {
  int i, j;
  bool didSmth = false;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS; j++) {
      if (bonuses->inPos(i, j) != Bonus::NOTHING && bonuses->inPos(i, j) != Bonus::BOMB) {
        didSmth = true;
        animateColorChange(i, j, colorColor(static_cast<int>(bonuses->inPos(i, j))));
        bonuses->setBonus(i, j, Bonus::NOTHING);
      }
      if (bonuses->inPos(i, j) == Bonus::BOMB) {
        didSmth = true;
        blocks->dead(i, j);
        bonuses->setBonus(i, j, Bonus::NOTHING);
      }
    }
  return didSmth;
}

void Field::animateBonus(int i, int j) {
  int m;
  const int steps = 50;
  blocks->bonus(i, j);
  for (m = 0; m < steps; m++) {
    blocks->inPos(i, j)->animate(steps);
    QThread::msleep(2 * wait);
    repaint();
  }
  for (m = 0; m < steps; m++) {
    blocks->inPos(i, j)->animate(-steps);
    QThread::msleep(2 * wait);
    repaint();
  }
  blocks->notbonus(i, j);
}

void Field::animateColorChange(int i, int j, QColor color) {
  int m;
  const int steps = 50;
  const int sleepFor = 2;
  blocks->twoColors(i, j, color);
  for (m = 0; m < 50; m++) {
    blocks->inPos(i, j)->animate(steps);
    QThread::msleep(sleepFor);
    repaint();
  }
  blocks->oneColor(i, j);
}