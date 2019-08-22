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

  std::vector<double> control = {
    -9999.0, 
    -0.117294,    
    -0.142737,   
    2.22507e-308, 
    -0.370975,    
    -0.477835,    
    -0.475127,    
    -0.397994,    
    -0.607414    
  };

  std::unique_ptr<Mesh> m(new Mesh("test_files/ms-riv.grd"));
  m->read();
  m->defineProjection(4326, true);
  m->reproject(26915);

  Multithreading::disable();

  std::unique_ptr<Griddata> g(
      new Griddata(m.get(), "test_files/bathy_sampleraster.tif"));
  std::unique_ptr<Griddata> gm(
      new Griddata(m.get(), "test_files/bathy_sampleraster.tif"));
  g->setShowProgressBar(true);
  gm->setShowProgressBar(true);
  g->setEpsg(26915);
  gm->setEpsg(26915);

  for (size_t i = 0; i < m->numNodes(); ++i) {
    g->setInterpolationFlag(i, i % 9);
    gm->setInterpolationFlag(i, i % 9);
    if (g->interpolationFlag(i) == 7 || g->interpolationFlag(i) == 8) {
      g->setFilterSize(i, 16.0);
      gm->setFilterSize(i, 16.0);
    }
  }

  g->setRasterInMemory(false);
  gm->setRasterInMemory(true);

  std::cout << "Interpolating from disk..." << std::endl;
  std::vector<double> r = g->computeValuesFromRaster();

  std::cout << "Interpolating from memory..." << std::endl;
  std::vector<double> rm = gm->computeValuesFromRaster();

  for(size_t i=0;i<9;++i){
      std::cout << i << " " << r[i] << " " << rm[i] << " " << control[i] << std::endl;
      if( std::abs(r[i]-control[i])>0.000001 || std::abs(rm[i]-control[i])>0.000001 ){
          return 1;
      }
  }

  return 0;
}
