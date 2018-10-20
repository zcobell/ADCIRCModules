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
#include <cmath>
#include <iostream>
#include <memory>
#include "adcirc.h"

int main() {
  using namespace Adcirc::Geometry;
  using namespace Interpolation;

  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  mesh->read();
  double oldx = mesh->node(0)->x();
  double oldy = mesh->node(0)->y();

  mesh->reproject(26915);
  double newx = mesh->node(0)->x();
  double newy = mesh->node(0)->y();

  char buffer1[50], buffer2[50], buffer3[50], buffer4[50];
  sprintf(buffer1, "Original X coordinate: %f", oldx);
  std::cout << buffer1 << std::endl;
  sprintf(buffer2, "Projected X coordinate: %f", newx);
  std::cout << buffer2 << std::endl;
  sprintf(buffer3, "Original Y coordinate: %f", oldy);
  std::cout << buffer3 << std::endl;
  sprintf(buffer4, "Projected Y coordinate: %f", newy);
  std::cout << buffer4 << std::endl;
  if (std::abs(newx - 753922.922116) > 0.000001 ||
      std::abs(newy - 3328065.712818) > 0.000001) {
    std::cout << "Expected: 753922.922116, 3328065.712818" << std::endl;
    printf("Got: %14.2f, %14.2f\n", newx, newy);
    return 1;
  } else {
    return 0;
  }
}
