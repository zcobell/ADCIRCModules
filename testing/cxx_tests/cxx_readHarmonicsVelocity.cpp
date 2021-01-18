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

#include "AdcircModules.h"

int main() {
  using namespace Adcirc::Geometry;
  using namespace Adcirc::Harmonics;

  std::unique_ptr<HarmonicsOutput> harm(
      new HarmonicsOutput("test_files/fort.54"));
  harm->read();

  double m2_u_mag = harm->u_amplitude("M2")->value(0);
  double m2_u_pha = harm->u_phase("M2")->value(0);
  double m2_v_mag = harm->v_amplitude("M2")->value(0);
  double m2_v_pha = harm->v_phase("M2")->value(0);

  double k1_u_mag = harm->u_amplitude("K1")->value(0);
  double k1_u_pha = harm->u_phase("K1")->value(0);
  double k1_v_mag = harm->v_amplitude("K1")->value(0);
  double k1_v_pha = harm->v_phase("K1")->value(0);

  if (m2_u_mag != 7.30159113E+000 || m2_u_pha != 100.5995 ||
      m2_v_mag != 3.43586134E+000 || m2_v_pha != 100.5995 ||
      k1_u_mag != 4.95712421E-001 || k1_u_pha != 310.5501 ||
      k1_v_mag != 2.33264108E-001 || k1_v_pha != 310.5501) {
    return 1;
  }

  std::cout << "Validated initial read.\n";

  harm->write("test_files/testwrite.54");

  std::unique_ptr<HarmonicsOutput> harm2(
      new HarmonicsOutput("test_files/testwrite.54"));
  harm2->read();

  std::cout << "File written.\n";

  m2_u_mag = harm2->u_amplitude("M2")->value(0);
  m2_u_pha = harm2->u_phase("M2")->value(0);
  m2_v_mag = harm2->v_amplitude("M2")->value(0);
  m2_v_pha = harm2->v_phase("M2")->value(0);

  k1_u_mag = harm2->u_amplitude("K1")->value(0);
  k1_u_pha = harm2->u_phase("K1")->value(0);
  k1_v_mag = harm2->v_amplitude("K1")->value(0);
  k1_v_pha = harm2->v_phase("K1")->value(0);

  if (m2_u_mag != 7.30159113E+000 || m2_u_pha != 100.5995 ||
      m2_v_mag != 3.43586134E+000 || m2_v_pha != 100.5995 ||
      k1_u_mag != 4.95712421E-001 || k1_u_pha != 310.5501 ||
      k1_v_mag != 2.33264108E-001 || k1_v_pha != 310.5501) {
    return 1;
  }

  std::cout << "Validated read/write cycle.\n";

  return 0;
}
