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
#include "adcirc.h"

int main() {
  using namespace Adcirc::Geometry;
  using namespace Adcirc::Output;
  std::unique_ptr<OutputFile> output(new OutputFile("test_files/fort.63.nc"));
  output->open();
  output->read();
  output->read();
  output->read();
  std::cout << "Expected: 1.84674, Got: " << output->data(2)->z(925)
            << std::endl;
  if (std::fabs(output->data(2)->z(925) - 1.84674) > 0.00001) return 1;
  return 0;
}
