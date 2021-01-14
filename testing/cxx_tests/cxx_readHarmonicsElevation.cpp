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
  using namespace Adcirc::Harmonics;

  std::unique_ptr<HarmonicsOutput> harm(
      new HarmonicsOutput("test_files/fort.53"));
  harm->read();

  double m2_amp_value = harm->amplitude("M2")->value(0);
  double m2_pha_value = harm->phase("M2")->value(0);
  double k1_amp_value = harm->amplitude("K1")->value(0);
  double k1_pha_value = harm->phase("K1")->value(0);

  if (m2_amp_value != 8.22651718E-001 || m2_pha_value != 247.8750 ||
      k1_amp_value != 9.36588108E-002 || k1_pha_value != 138.2870) {
    return 1;
  }

  std::cout << "Validated read\n";
  std::cout.flush();

  harm->write("test_files/testwrite.53");

  std::unique_ptr<HarmonicsOutput> harm2(
      new HarmonicsOutput("test_files/testwrite.53"));
  harm2->read();

  std::cout << "Read file written by this code.\n";
  std::cout.flush();

  m2_amp_value = harm2->amplitude("M2")->value(0);
  m2_pha_value = harm2->phase("M2")->value(0);
  k1_amp_value = harm2->amplitude("K1")->value(0);
  k1_pha_value = harm2->phase("K1")->value(0);

  if (m2_amp_value != 8.22651718E-001 || m2_pha_value != 247.8750 ||
      k1_amp_value != 9.36588108E-002 || k1_pha_value != 138.2870) {
    return 1;
  }

  std::cout << "Validated full read/write cycle.\n";
  std::cout.flush();

  return 0;
}
