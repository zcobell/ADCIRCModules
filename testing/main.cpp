//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------//
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
