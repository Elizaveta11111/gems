#include "Grid.h"
bool Grid::areNeigbors(int rx, int ry, int tx, int ty) {
  if ((abs(rx - tx) == 1 && ry == ty) || (abs(ry - ty) == 1 && rx == tx))
    return true;
  else
    return false;
}

void GridForBlocks::stop() {
  int i, j;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS + 1; j++)
      grid[i][j]->dy = 0;
}

void GridForBlocks::moveY(int i, int j) {
  std::shared_ptr <Block> block = inPos(i, j);
  block->y += block->dy;
}

void GridForBlocks::fall(int row) {
  int i, j, k;
  for (i = 0; i < COLUMNS; i++)
    for (j = row; j >= 0; j--)
      if (inPos(i, j)->isDead()) {
        std::shared_ptr <Block> temp = inPos(i, j);
        for (k = j + 1; k > 0; k--)
          grid[i][k] = grid[i][k - 1];
        grid[i][0] = temp;
        alive(i, -1);
        grid[i][0]->color = colorColor(rand() % COLORS); //!
        grid[i][0]->y = -blockh;
        j = -1;
      }
  stop();
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
  inPos(i, j)->dy = 1;
}

GridForBlocks::GridForBlocks() {
  int i, j;
  for (i = 0; i < COLUMNS; i++) {
    for (j = 0; j < ROWS + 1; j++) {
      std::shared_ptr<Block> temp(new SimpleBlock);
      grid[i][j] = temp;
      grid[i][j]->x = i * blockw;
      grid[i][j]->y = (j - 1) * blockh;
    }
  }
}

int GridForBlocks::check(int i, int j, int deep) {
  int kills = 1;
  //if (deep == 2) 
  //  return kills;
  dead(i, j);
  std::shared_ptr <Block> A = inPos(i, j);
  std::shared_ptr <Block> B;
  int killedX = -1;
  int killedY;
  if (i + 1 < COLUMNS) {
    B = inPos(i + 1, j);
    if (!B->isDead() && B->color == A->color) {
      if (deep == 0) {
        killedX = i + 1;
        killedY = j;
      }
      kills += check(i + 1, j, deep + 1);
    }
  }
  if (i - 1 >= 0) {
    B = inPos(i - 1, j);
    if (!B->isDead() && B->color == A->color) {
      if (deep == 0) {
        killedX = i - 1;
        killedY = j;
      }
      kills += check(i - 1, j, deep + 1);
    }
  }
  if (j + 1 < ROWS) {
    B = inPos(i, j + 1);
    if (!B->isDead() && B->color == A->color) {
      if (deep == 0) {
        killedX = i;
        killedY = j + 1;
      }
      kills += check(i, j + 1, deep + 1);
    }
  }
  if (j - 1 >= 0) {
    B = inPos(i, j - 1);
    if (!B->isDead() && B->color == A->color) {
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
      blockA->dy = 1;
      blockB->dy = -1;
    }
    else {
      blockA->dy = -1;
      blockB->dy = 1;
    }
  }
  else {
    to = blockw;
    if (iA < iB) {
      blockA->dx = 1;
      blockB->dx = -1;
    }
    else {
      blockA->dx = -1;
      blockB->dx = 1;
    }
  }
  return to;
}

void GridForBlocks::stopSwap(int iA, int jA, int iB, int jB) {
  std::shared_ptr <Block> blockA = inPos(iA, jA);
  std::shared_ptr <Block> blockB = inPos(iB, jB);
  blockA->dx = blockA->dy = 0;
  blockB->dx = blockB->dy = 0;
  grid[iA][jA + 1] = blockB;
  grid[iB][jB + 1] = blockA;
}

void GridForBlocks::move(int i, int j) {
  std::shared_ptr <Block> block = inPos(i, j);
  block->x += block->dx;
  block->y += block->dy;
}

GridForBonuses::GridForBonuses() {
  int i, j;
  for (i = 0; i < COLUMNS; i++)
    for (j = 0; j < ROWS; j++)
      grid[i][j] = -1;
}

void GridForBlocks::drawBlock(int i, int j, QPainter* qp) {
  inPos(i, j)->draw(qp);
}
