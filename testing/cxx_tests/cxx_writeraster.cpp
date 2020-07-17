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
  using namespace Adcirc::Geometry;
  using namespace Adcirc::Output;
 
  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  //std::unique_ptr<ReadOutput> output(new ReadOutput("test_files/fort.63"));

  mesh->read();
  mesh->toRaster("adcirc_depth.img",mesh->z(),mesh->extent(),0.001,-9999.0);

  //output->open();
  //output->read();
  //mesh->toRaster("adcirc_wse.img",output->data(0)->values(),mesh->extent(),0.001,-99999.0);

  return 0;

}
