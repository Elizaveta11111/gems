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
  void changeX(int s) {
    x = s;
  }
  void changeX() {
    x = x + dx;
  }
  int getX() const {
    return x;
  }
  void changeY(int s) {
    y = s;
  }
  void changeY() {
    y = y + dy;
  }
  int getY() const {
    return y;
  }
  void changeDX(int s) {
    dx = s;
  }
  int getDX() const {
    return dx;
  }
  void changeDY(int s) {
    dy = s;
  }
  int getDY() const {
    return dy;
  }
  void changeColor(QColor s) {
    color = s;
  }
  QColor getColor() const {
    return color;
  }
  int posX() {
    return x / blockw;
  }
  int posY() {
    return y / blockh;
  }
  virtual bool isDead() {
    return false;
  }
  virtual void animate(int i = 0) {
    y += 2 * dy;
  }
  virtual void draw(QPainter*) = 0;
private:
  QColor color;
  int dx;
  int dy;
  int y;
  int x;
};

class SimpleBlock : public Block {
public:
  ~SimpleBlock() {}
  SimpleBlock& operator=(const Block& obj) {
    this->changeColor(obj.getColor());
    this->changeX(obj.getX());
    this->changeY(obj.getY());
    this->changeDX(obj.getDX());
    this->changeDY(obj.getDY());
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
  ColorBonusBlock& operator=(const Block& obj) {
    this->changeColor(obj.getColor());
    this->changeX(obj.getX());
    this->changeY(obj.getY());
    this->changeDX(obj.getDX());
    this->changeDY(obj.getDY());
    return *this;
  }
  void draw(QPainter*);
  void animate(int i) {
    sh += blockh/i;
    sw += blockw/i;
  }
private:
  int sh;
  int sw;
};

class AnimatedBlock : public Block {
public:
  QColor secondColor;
  ~AnimatedBlock(){}
  AnimatedBlock(QColor firstColor) : Block() {
    height = 0;
    this->changeColor(firstColor);
    secondColor = getColor();
  }
  AnimatedBlock& operator=(const Block& obj) {
    secondColor = obj.getColor();
    this->changeX(obj.getX());
    this->changeY(obj.getY());
    this->changeDX(obj.getDX());
    this->changeDY(obj.getDY());
    return *this;
  }
  void draw(QPainter*);
  void animate(int i) {
    height += blockh / (i - 1);
  }
private:
  int height;
};

class ChekedBlock : public Block {
public:
  ChekedBlock() : Block() {
  }
  ~ChekedBlock() {}
  ChekedBlock& operator=(const Block& obj) {
    this->changeColor(obj.getColor());
    this->changeX(obj.getX());
    this->changeY(obj.getY());
    return *this;
  }
  void draw(QPainter* qp) {
    QPen pen;
    pen.setWidth(2);
    qp->setPen(pen);
    qp->setBrush(getColor());
    qp->drawRect(getX(), getY(), blockw, blockh);
    pen.setWidth(10);
    qp->setPen(pen);
    qp->drawLine(getX() + 5, getY() + 5, getX() + blockw - 5, getY() + 5);
    qp->drawLine(getX() + blockw - 5, getY() + 5, getX() + blockw - 5, getY() + blockh - 5);
    qp->drawLine(getX() + blockw - 5, getY() + blockh - 5, getX() + 5, getY() + blockh - 5);
    qp->drawLine(getX() + 5, getY() + blockh - 5, getX() + 5, getY() + 5);
  }
};

class DeadBlock : public Block {
public:
  DeadBlock() {}
  ~DeadBlock() {}
  DeadBlock& operator=(const Block& obj) {
    this->changeDY(obj.getDY());
    this->changeX(obj.getX());
    this->changeY(obj.getY());
    this->changeColor(obj.getColor());
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
    qp->drawRect(getX(), getY(), blockw, blockh);
  }
};