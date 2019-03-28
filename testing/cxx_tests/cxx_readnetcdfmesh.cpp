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
#include <cmath>
#include "adcircmodules.h"

int main() {
  using namespace Adcirc::Geometry;
  const double expected_x = -89.488429;
  const double expected_y = 30.174036;
  const double tol = 0.000001;

  std::unique_ptr<Mesh> mesh(new Mesh("test_files/maxele.63.nc"));
  mesh->read();

  if(std::abs(mesh->node(4)->x()-expected_x)>tol){
    std::cout << "Got: " << mesh->node(4)->x() << ", Expected: " << expected_x << std::endl;
    std::cout.flush();
    return 1;
  }
  if(std::abs(mesh->node(4)->y()-expected_y)>tol){
    std::cout << "Got: " << mesh->node(4)->y() << ", Expected: " << expected_y << std::endl;
    std::cout.flush();
    return 1;
  }

  return 0;
}
