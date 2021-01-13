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
  using namespace Adcirc::Interpolation;

  std::vector<double> control = {0.108665,  0.0929215, 0.091493,  0.125231,
                                 0.0909828, 0.0702279, 0.0441836, 0.025,
                                 0.025,     0.025,     0.0299349, 0.108283};

  std::unique_ptr<Mesh> m(new Mesh("test_files/ms-riv.grd"));
  m->read();
  m->defineProjection(4326, true);
  m->reproject(26915);

  std::unique_ptr<Griddata> g(
      new Griddata(m.get(), "test_files/lulc_samplelulcraster.tif", 26915));
  // Multithreading::disable();

#if 1
  for (size_t i = 0; i < m->numNodes(); ++i) {
    if (i < 100) {
      g->setInterpolationFlag(i, Adcirc::Interpolation::Average);
    } else {
      g->setInterpolationFlag(i, Adcirc::Interpolation::NoMethod);
    }
  }
#endif

  g->setShowProgressBar(true);
  g->readLookupTable("test_files/sample_lookup.table");
  g->setRasterInMemory(true);
  std::vector<std::vector<double>> r = g->computeDirectionalWindReduction(true);

  bool fail = false;
  for (size_t i = 0; i < 12; ++i) {
    std::cout << r[0][i] << " " << control[i] << " ";
    if (std::abs(r[0][i] - control[i]) > 0.00001) {
      fail = true;
      std::cout << "FAIL" << std::endl;
    } else {
      std::cout << "PASS" << std::endl;
    }
  }

  return fail;
}
