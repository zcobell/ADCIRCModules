#include "adcircmesh.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  AdcircMesh *mesh = new AdcircMesh(
      "/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.grd",
      &a);

  int ierr = mesh->read();

  qDebug() << "Return: " << ierr;

  return 0;
}
