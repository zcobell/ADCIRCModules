#include "adcircmesh.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  AdcircMesh *mesh = new AdcircMesh(
      std::string("/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.grd"));

  int ierr = mesh->read();

  ierr = mesh->reproject(26915);

  ierr = mesh->write("ms-riv-utm15.grd");

  ierr = mesh->buildNodalSearchTree();

  qDebug() << "Return: " << ierr;

  return 0;
}
