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
  using namespace Adcirc::Output;

  std::unique_ptr<Mesh> mesh(new Mesh("test_files/ms-riv.grd"));
  mesh->read();

  std::unique_ptr<ReadOutput> output(new ReadOutput("test_files/fort.63"));

  //...Open file
  output->open();

  //...Read snap 1
  output->read();

  //...Read snap 2
  output->read();

  //...Read snap 3
  output->read();

  output->close();

  double checkValue = output->data(1)->z(10);

  std::unique_ptr<WriteOutput> writer(
      new WriteOutput("test_files/fort.write.63.nc", output.get(), mesh.get()));
  writer->open();
  writer->write(output->data(0));
  writer->write(output->data(1));
  writer->write(output->data(2));
  writer->close();

  output.reset(new ReadOutput("test_files/fort.write.63.nc"));
  writer.reset(nullptr);

  output->open();
  output->read();
  output->read();

  double checkValue2 = output->data(1)->z(10);

  output->close();

  if (std::abs(checkValue - checkValue2) > 0.000001) return 1;

  return 0;
}
