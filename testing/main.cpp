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
#include "adcirc.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Adcirc::Geometry::Mesh *mesh = new Adcirc::Geometry::Mesh(
      string("/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.grd"));

  int ierr = mesh->read();
  std::cout << "Mesh Read Return Code: " << ierr << "\n";

  Adcirc::ModelParameters::NodalAttributes *f13 = new Adcirc::ModelParameters::NodalAttributes(string("/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.13"),mesh);
  //AdcircNodalParameters *f13 = new AdcircNodalParameters(string("/home/zcobell/Development/QADCModules/testing/test_files/ms-riv.13"));
  ierr = f13->read();
  std::cout << "Fort13 Read Return Code: " << ierr << "\n";
  std::cout << f13->attribute(0,0)->node()->id() << "\n";
  std::cout << f13->attribute("mannings_n_at_sea_floor",0)->value(0) << "\n";

/*
  ierr = mesh->reproject(26915);

  ierr = mesh->write("ms-riv-utm15.grd");

  ierr = mesh->buildNodalSearchTree();
*/
  return 0;
}
