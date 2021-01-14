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
  using namespace Adcirc::Output;

  std::unique_ptr<ReadOutput> output(
      new ReadOutput("test_files/sparse_fort.64"));

  //...Open file
  output->open();

  //...Read snap 1
  output->read();

  //...Read snap 2
  output->read();

  //...Read snap 3
  output->read();

  //...Check output
  std::cout << "Expected: -0.000333917, Got: " << output->data(2)->v(1220)
            << std::endl;

  Adcirc::Output::OutputRecord *rec = output->dataAt(1);

  output->close();

  if (std::abs(output->data(2)->v(1220) - (-0.000333917)) < 0.0000001) return 0;
  return 1;
}
