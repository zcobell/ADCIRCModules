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
#include <iostream>
#include <memory>

#include "AdcircModules.h"

int main() {
  using namespace Adcirc::Geometry;
  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv_quad.2dm"));
  mesh->read();
  mesh->write("test_files/ms-riv_quad_net.nc");

  mesh.reset(new Mesh("test_files/ms-riv_quad_net.nc"));
  mesh->read();

  return 0;
}
