#include "Field.h"

#include <QApplication>


int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  Field window;

  window.setFixedSize(SIZEX, SIZEY);
  window.show();

  return app.exec();
}