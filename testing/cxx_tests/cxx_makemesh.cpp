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
#include "adcircmodules.h"
#include <vector>

int main() {
  Adcirc::Geometry::Mesh m;
  m.resizeMesh(3,1,0,0);
  m.addNode(0,Adcirc::Geometry::Node(1,-90.0,20.0,0.0));
  m.addNode(1,Adcirc::Geometry::Node(2,-95.0,25.0,0.0));
  m.addNode(2,Adcirc::Geometry::Node(3,-88.0,30.0,0.0));
  m.addElement(0,Adcirc::Geometry::Element(1,m.node(0),m.node(1),m.node(2)));
  m.setMeshHeaderString("Manually generated mesh");

  m.write("../testing/test_files/manualmesh.grd");

  double a = m.element(0)->area();
  bool inside = m.element(0)->isInside(-89.0,26.0);

  std::cout << "Element Area: " << a << std::endl;
  std::cout << "Is inside: " << inside << std::endl;
 
  return 0;
}
