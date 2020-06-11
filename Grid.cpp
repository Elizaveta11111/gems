#include "Grid.h"

bool Grid::areNeighbors(int rx, int ry, int tx, int ty) {
  return ((abs(rx - tx) == 1 && ry == ty) || (abs(ry - ty) == 1 && rx == tx));
}

void GridForBlocks::stop() {
  int i, j;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS + 1; j++)
      grid[i][j]->changeDY(0);
}

void GridForBlocks::moveY(int i, int j) {
  std::shared_ptr <Block> block = inPos(i, j);
  block->changeY(block->getY() + block->getDY());
}

void GridForBlocks::fall(int row) {
  int i, j, k;
  for (i = 0; i < COLUMNS; i++)
    for (j = row; j >= 0; j--)
      if (inPos(i, j)->isDead()) {
        std::shared_ptr <Block> temp = inPos(i, j);
        moveDown(i, j);
        swap(i, temp);
        break;
      }
  stop();
}

void GridForBlocks::moveDown(int row, int col) {
  for (int k = col + 1; k > 0; k--)
    grid[row][k] = grid[row][k - 1];
}

void GridForBlocks::swap(int col, std::shared_ptr <Block> temp) {
  grid[col][0] = temp;
  alive(col, -1);
  grid[col][0]->changeColor(colorColor(rand() % COLORS));
  grid[col][0]->changeY(-blockh);
}

void GridForBlocks::dead(int i, int j) {
  std::shared_ptr<DeadBlock> temp(new DeadBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::alive(int i, int j) {
  std::shared_ptr<Block> temp(new SimpleBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::pickBlock(int i, int j) {
  std::shared_ptr<ChekedBlock> temp(new ChekedBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::unpickBlock(int i, int j) {
  std::shared_ptr<SimpleBlock> temp(new SimpleBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::bonus(int i, int j) {
  std::shared_ptr<ColorBonusBlock> temp(new ColorBonusBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::notbonus(int i, int j) {
  std::shared_ptr<DeadBlock> temp(new DeadBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::twoColors(int i, int j, QColor color) {
  std::shared_ptr<AnimatedBlock> temp(new AnimatedBlock(color));
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::oneColor(int i, int j) {
  std::shared_ptr<SimpleBlock> temp(new SimpleBlock);
  *temp = *inPos(i, j);
  grid[i][j + 1] = temp;
}

void GridForBlocks::changedy(int i, int j) {
  inPos(i, j)->changeDY(1);
}

GridForBlocks::GridForBlocks() {
  int i, j;
  for (i = 0; i < COLUMNS; i++) {
    for (j = 0; j < ROWS + 1; j++) {
      std::shared_ptr<Block> temp(new SimpleBlock);
      grid[i][j] = temp;
      grid[i][j]->changeX(i * blockw);
      grid[i][j]->changeY((j - 1) * blockh);
    }
  }
}

int GridForBlocks::check(int i, int j, int deep) {
  int kills = 1;
  dead(i, j);
  std::shared_ptr <Block> A = inPos(i, j);
  std::shared_ptr <Block> B;
  int killedX = -1;
  int killedY;
  if (i + 1 < COLUMNS) {
    B = inPos(i + 1, j);
    if (!B->isDead() && B->getColor() == A->getColor()) {
      if (deep == 0) {
        killedX = i + 1;
        killedY = j;
      }
      kills += check(i + 1, j, deep + 1);
    }
  }
  if (i - 1 >= 0) {
    B = inPos(i - 1, j);
    if (!B->isDead() && B->getColor() == A->getColor()) {
      if (deep == 0) {
        killedX = i - 1;
        killedY = j;
      }
      kills += check(i - 1, j, deep + 1);
    }
  }
  if (j + 1 < ROWS) {
    B = inPos(i, j + 1);
    if (!B->isDead() && B->getColor() == A->getColor()) {
      if (deep == 0) {
        killedX = i;
        killedY = j + 1;
      }
      kills += check(i, j + 1, deep + 1);
    }
  }
  if (j - 1 >= 0) {
    B = inPos(i, j - 1);
    if (!B->isDead() && B->getColor() == A->getColor()) {
      if (deep == 0) {
        killedX = i;
        killedY = j - 1;
      }
      kills += check(i, j - 1, deep + 1);
    }
  }
  if (deep == 0 && kills < 3) {
    if (killedX != -1)
      alive(killedX, killedY);
    alive(i, j);
    kills = 0;
  }
  return kills;
}

int GridForBlocks::startSwap(int iA, int jA, int iB, int jB) {
  std::shared_ptr <Block> blockA = inPos(iA, jA);
  std::shared_ptr <Block> blockB = inPos(iB, jB);
  int to;
  if (iA == iB) {
    to = blockh;
    if (jA < jB) {
      blockA->changeDY(1);
      blockB->changeDY(-1);
    }
    else {
      blockA->changeDY(-1);
      blockB->changeDY(1);
    }
  }
  else {
    to = blockw;
    if (iA < iB) {
      blockA->changeDX(1);
      blockB->changeDX(-1);
    }
    else {
      blockA->changeDX(-1);
      blockB->changeDX(1);
    }
  }
  return to;
}

void GridForBlocks::stopSwap(int iA, int jA, int iB, int jB) {
  std::shared_ptr <Block> blockA = inPos(iA, jA);
  std::shared_ptr <Block> blockB = inPos(iB, jB);
  blockA->changeDX(0);
  blockA->changeDY(0);
  blockB->changeDX(0);
  blockB->changeDY(0);
  grid[iA][jA + 1] = blockB;
  grid[iB][jB + 1] = blockA;
}

void GridForBlocks::move(int i, int j) {
  std::shared_ptr <Block> block = inPos(i, j);
  block->changeX();
  block->changeY();
}

GridForBonuses::GridForBonuses() {
  int i, j;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS; j++)
      grid[i][j] = Bonus::NOTHING;
}

void GridForBlocks::drawBlock(int i, int j, QPainter* qp) {
  inPos(i, j)->draw(qp);
}
