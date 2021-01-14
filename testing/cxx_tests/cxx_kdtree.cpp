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

int main() {
  using namespace Adcirc::Geometry;
  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  mesh->read();

  //...Using the mesh as a dataset but in vector form
  std::vector<double> x = mesh->x();
  std::vector<double> y = mesh->y();

  Adcirc::Kdtree k;
  k.build(x, y);

  const double searchX = -90.766116;
  const double searchY = 30.002113;

  const size_t known_nearest = 14493;
  size_t nearest = k.findNearest(searchX, searchY);
  if (known_nearest != nearest) return 1;

  std::vector<size_t> nearest5 = k.findXNearest(searchX, searchY, 5);
  if (nearest5[0] != nearest) return 1;

  std::vector<size_t> nearest500 = k.findWithinRadius(searchX, searchY, 0.005);
  if (nearest500[0] != known_nearest) return 1;

  return 0;
}
