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
#include <vector>
#include "adcircmodules.h"

int main() {
  using namespace Adcirc::Geometry;

  auto mesh = std::make_unique<Mesh>("test_files/ms-riv.grd");
  mesh->read();

  mesh->topology()->nodeTable()->build();
  mesh->topology()->elementTable()->build();
  mesh->topology()->faceTable()->build();

  const size_t nid = 14417;
  const auto nnode = mesh->topology()->nodeTable()->numNodesAroundNode(nid-1);
  std::cout << "Number of nodes: " << nnode << std::endl;
  const auto nn = mesh->topology()->nodeTable()->nodeList(nid-1);
  std::cout << "Last node id: " << nn[nnode-1]->id() << std::endl;
  if(nn[nnode-1]->id()!=14572){
      std::cout << "Expected: 14572, Got: " << nn[nnode-1]->id() << std::endl;
      return 1;
  }

  const auto nelem = mesh->topology()->elementTable()->numElementsAroundNode(nid-1);
  std::cout << "Number of elements around node: " << nelem << std::endl;
  const auto ee = mesh->topology()->elementTable()->elementTable(nid-1,nelem-1);
  std::cout << "Last element id: " << ee->id() << std::endl;
  if(ee->id()!=23121){
      std::cout << "Expected: 23121, Got: " << ee->id() << std::endl;
      return 1;
  }


  const size_t eid = 22902;
  const auto nface = mesh->topology()->faceTable()->numSharedFaces(eid-1);
  std::cout << "Number of faces around element: " << nface << std::endl;
  const auto ff = mesh->topology()->faceTable()->neighbors(eid-1);
  std::cout << "Last shared face around element: " << ff[nface-1]->id() << std::endl;
  if(ff[nface-1]->id()!=22911){
      std::cout << "Expected: 22911, Got: " << ff[nface-1] << std::endl;
      return 1;
  }


  return 0;
}
