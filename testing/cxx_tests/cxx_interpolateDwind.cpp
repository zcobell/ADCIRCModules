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
  using namespace Interpolation;

  std::unique_ptr<Mesh> m(new Mesh("test_files/ms-riv.grd"));
  m->read();
  m->defineProjection(4326, true);
  m->reproject(26915);

  std::unique_ptr<Griddata> g(
      new Griddata(m.get(), "test_files/lulc_samplelulcraster.tif"));
  // Multithreading::disable();

#if 1
  for (size_t i = 0; i < m->numNodes(); ++i) {
    if (i < 100) {
      g->setInterpolationFlag(i, Interpolation::Average);
    } else {
      g->setInterpolationFlag(i, Interpolation::NoMethod);
    }
  }
#endif

  g->setShowProgressBar(true);
  g->readLookupTable("test_files/sample_lookup.table");
  g->setEpsg(26915);
  g->setRasterInMemory(true);
  std::vector<std::vector<double>> r = g->computeDirectionalWindReduction(true);
}
