#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_Gems.h"
#include <QRect>
#include <QPainter>

#include <cstdlib>
#include <array>


#define ROWS  10
#define COLUMNS 12
#define SIZEX 600
#define SIZEY 720
#define COLORS 6

int const blockw = (double)SIZEX / (double)COLUMNS;
int const blockh = (double)SIZEY / (double)ROWS;

int colorNumber(QColor color);
QColor colorColor(int i);


class Block {
public:
  Block();
  ~Block() {}
  int posX() {
    return x / blockw;
  }
  int posY() {
    return y / blockh;
  }
  QColor color;
  int dx;
  int dy;
  int x;
  int y;
  virtual void draw(QPainter*) = 0;
  virtual bool isDead() {
    return false;
  }
  virtual void animate(int i = 0) {
    y += 2 * dy;
  }
};

class SimpleBlock : public Block {
public:
  ~SimpleBlock() {}
  SimpleBlock& operator=(const Block& obj) {
    color = obj.color;
    x = obj.x;
    y = obj.y;
    dx = obj.dx;
    dy = obj.dy;
    return *this;
  }
  void draw(QPainter*);
};

class ColorBonusBlock : public Block {
public:
  ColorBonusBlock() : Block() {
    sh = 0; 
    sw = 0;
  }
  ~ColorBonusBlock(){}
  int sh;
  int sw;
  ColorBonusBlock& operator=(const Block& obj) {
    color = obj.color;
    x = obj.x;
    y = obj.y;
    dx = obj.dx;
    dy = obj.dy;
    return *this;
  }
  void draw(QPainter*);
  void animate(int i) {
    sh += blockh/i;
    sw += blockw/i;
  }
};

class AnimatedBlock : public Block {
public:
  QColor secondColor;
  ~AnimatedBlock(){}
  AnimatedBlock(QColor firstColor) : Block() {
    height = 0;
    color = firstColor;
    secondColor = color;
  }
  int height;
  AnimatedBlock& operator=(const Block& obj) {
    secondColor = obj.color;
    x = obj.x;
    y = obj.y;
    dx = obj.dx;
    dy = obj.dy;
    return *this;
  }
  void draw(QPainter*);
  void animate(int i) {
    height += blockh / (i - 1);
  }
};

class ChekedBlock : public Block {
public:
  ChekedBlock() : Block() {
  }
  ~ChekedBlock() {}
  ChekedBlock& operator=(const Block& obj) {
    color = obj.color;
    x = obj.x;
    y = obj.y;
    return *this;
  }
  void draw(QPainter* qp) {
    QPen pen;
    pen.setWidth(2);
    qp->setPen(pen);
    qp->setBrush(color);
    qp->drawRect(x, y, blockw, blockh);
    pen.setWidth(10);
    qp->setPen(pen);
    qp->drawLine(x + 5, y + 5, x + blockw - 5, y + 5);
    qp->drawLine(x + blockw - 5, y + 5, x + blockw - 5, y + blockh - 5);
    qp->drawLine(x + blockw - 5, y + blockh - 5, x + 5, y + blockh - 5);
    qp->drawLine(x + 5, y + blockh - 5, x + 5, y + 5);
  }
};

class DeadBlock : public Block {
public:
  DeadBlock() {}
  ~DeadBlock() {}
  DeadBlock& operator=(const Block& obj) {
    dy = obj.dy;
    x = obj.x;
    y = obj.y;
    color = obj.color;
    return *this;
  }
  bool isDead() {
    return true;
  }
  void draw(QPainter* qp) {
    QPen pen;
    pen.setWidth(2);
    qp->setPen(pen);
    qp->setBrush(QColor("#000000"));
    qp->drawRect(x, y, blockw, blockh);
  }
};