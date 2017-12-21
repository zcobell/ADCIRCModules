#include "adcircmesh.h"
#include <iostream>

int main(int argc, char *argv[]) {
  AdcircMesh *mesh = new AdcircMesh(
      std::string("tx2008_r35a_GCCPRD_v04c_Alt16_SecondAlternative_v01a_chk.grd"));

  int ierr = mesh->read();
  std::cout << "Return Code: " << ierr << "\n";
/*
  ierr = mesh->reproject(26915);

  ierr = mesh->write("ms-riv-utm15.grd");

  ierr = mesh->buildNodalSearchTree();
*/
  return 0;
}
