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
  Adcirc::Geometry::Mesh *mesh = new Adcirc::Geometry::Mesh(string("test_files/ms-riv.grd"));
  int ierr = mesh->read();
  std::cout << "Mesh Read Return Code: " << ierr << "\n";
  if(ierr!=Adcirc::NoError) {
      delete mesh;
      return ierr;
  }

  ierr = mesh->toShapefile("test_files/ms-riv.shp");
  if(ierr!=Adcirc::NoError){
      delete mesh;
      return ierr;
  }
  else {
      delete mesh;
      return 0;
  }

}
