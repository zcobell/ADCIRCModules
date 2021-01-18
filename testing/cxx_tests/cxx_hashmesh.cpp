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

#include "AdcircModules.h"

int main() {
  using namespace Adcirc::Geometry;

  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  mesh->read();

  const size_t checkNodeNum = 23;
  const size_t checkElemNum = 20;
  const size_t checkBndNum = 2;

  const std::string expectedMeshHash =
      "850a6b8af2b2846caba239633b7fb826f9aa52a8";
  const std::string expectedNodeHash =
      "79a55ffd9fc7212ad231c6c6dae6eebdf04138bf";
  const std::string expectedElementHash =
      "05d26fea84ab0652e2345d92ab09cc5c98059edc";
  const std::string expectedBoundaryHash =
      "09e05db32137f51dc82a228f5ca38ed78a0b3322";

  if (mesh->hash() != expectedMeshHash) return 1;
  if (mesh->node(checkNodeNum)->hash() != expectedNodeHash) return 1;
  if (mesh->element(checkElemNum)->hash() != expectedElementHash) return 1;
  if (mesh->landBoundary(checkBndNum)->hash() != expectedBoundaryHash) return 1;

  return 0;
}
