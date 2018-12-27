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
  using namespace Adcirc::ModelParameters;

  std::unique_ptr<NodalAttributes> fort13(
      new Adcirc::ModelParameters::NodalAttributes("test_files/ms-riv.13"));
  fort13->read();

  int idx = fort13->locateAttribute("mannings_n_at_sea_floor");

  double n = fort13->attribute(idx, 10)->value(0);

  std::cout << "Manning n before: " << n << std::endl;
  std::cout << "Manning n expected: 0.020922" << std::endl;

  if (n != 0.020922) return 1;

  fort13->attribute(idx, 10)->setValue(0, 0.022);
  n = fort13->attribute(idx, 10)->value(0);

  std::cout << "Attempted to set value to 0.022 and got " << n << std::endl;

  if (n != 0.022) return 1;

  double dv = fort13->metadata("mannings_n_at_sea_floor")->defaultValue();
  if (dv != 0.012) return 1;

  fort13->metadata("mannings_n_at_sea_floor")->setDefaultValue(0.022);
  dv = fort13->metadata(idx)->defaultValue();

  if (dv != 0.022) return 1;

  std::cout << "Now attempting to re-write fort.13..." << std::endl;
  fort13->write("test_files/ms-riv-rewrite.13");

  return 0;
}
