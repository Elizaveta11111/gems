#include "Block.h"
#include <QRect>
#include <QPainter>

std::array<QColor, 6> colors = { QColor("#ff2b39"), QColor("#ed2bff"), QColor("#2be6ff"), QColor("#fbff2b"), QColor("#3dff2b"), QColor("#2b59ff") };

QColor colorColor(int i) {
  return colors[i];
}

int colorNumber(QColor color) {
  for (int i = 0; i < COLORS; i++)
    if (colors[i] == color)
      return i;
  return -1;
}

Block::Block() {
  color = colors[rand() % COLORS];
  dx = 0;
  dy = 0;
  x = 0;
  y = 0;
}

void SimpleBlock::draw(QPainter* qp) {
  QPen pen;
  pen.setWidth(2);
  qp->setPen(pen);
  qp->setBrush(color);
  qp->drawRect(x, y, blockw, blockh);
}

void ColorBonusBlock::draw(QPainter* qp) {
  QPen pen;
  pen.setWidth(2);
  qp->setPen(pen);
  qp->setBrush(QColor("#000000"));
  qp->drawRect(x, y, blockw, blockh);
  qp->setBrush(color);
  qp->drawRect(x + (blockw - sw)/2, y + (blockh - sh) / 2, sw, sh);
}

void AnimatedBlock::draw(QPainter* qp) {
  QPen pen;
  pen.setWidth(2);
  qp->setPen(pen);
  qp->setBrush(color);
  qp->drawRect(x, y, blockw, height);
  qp->setBrush(secondColor);
  qp->drawRect(x, y + height, blockw, blockh - height);
}