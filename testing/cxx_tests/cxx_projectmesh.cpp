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
#include "adcircmodules.h"

int main() {
  using namespace Adcirc::Geometry;

  Ezproj p;
  std::cout << "PROJ Version: " << p.projVersion() << std::endl;

  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  mesh->read();
  double oldx = mesh->node(0)->x();
  double oldy = mesh->node(0)->y();
  mesh->defineProjection(4326,true);
  
  std::cout << "Transforming to " << p.description(26915) << "...\n";

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
  }

  std::cout << "Reversing transformation...\n";
  mesh->reproject(4326);
  if(std::abs(oldx-mesh->node(0)->x())>0.00001 ||
     std::abs(oldy-mesh->node(0)->y())>0.00001){
      std::cout << "Error returning mesh to 4326\n";
      return 1;
  }

  std::cout << "Transforming to CPP...\n";
  mesh->cpp(-90.0,24.0);
  printf("CPP: %14.2f, %14.2f\n",mesh->node(0)->x(),mesh->node(0)->y());
  mesh->inverseCpp(-90.0,24.0);
  printf("INVCPP: %14.2f, %14.2f\n",mesh->node(0)->x(),mesh->node(0)->y());
  if(std::abs(oldx-mesh->node(0)->x())>0.00001 ||
     std::abs(oldy-mesh->node(0)->y())>0.00001){
      std::cout << "Error during CPP round trip\n";
      return 1;
  }


  return 0;


}
