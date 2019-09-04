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
  std::unique_ptr<ReadOutput> output(new ReadOutput("test_files/maxele.63.nc"));
  output->open();
  output->read();
  std::cout << "Expected: 0.365628193162685, Got: " << output->data(0)->z(7)
            << std::endl;
  if (output->data(0)->z(7) != 0.365628193162685) {
    return 1;
  }

  //...Extra gymnastics to test other functions
  std::vector<double> a = output->data(0)->values(0);
  std::vector<double> s = output->data(0)->values(1);

  for (size_t i = 0; i < a.size(); i++) {
    a[i] = a[i] + 10;
    s[i] = s[i] + 10;
  }

  //output->data(0)->setAll(a,s);

  output->close();

  return 0;
}
