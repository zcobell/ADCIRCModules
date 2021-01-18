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
#include <vector>

#include "AdcircModules.h"

int main() {
  Adcirc::CDate d(2019, 1, 1, 0, 0, 0);
  Adcirc::CDate d1 = d + 86400;
  std::cout << d1 << std::endl;
  std::vector<int> d1v = d1.get();
  std::vector<int> d1v_expected = {2019, 1, 2, 0, 0, 0, 0};
  if (d1v != d1v_expected) {
    std::cout << "Error: incorrect date addition" << std::endl;
    return 1;
  }

  Adcirc::CDate d2 = d + Adcirc::CDate::years(1) + Adcirc::CDate::months(14) +
                     Adcirc::CDate::weeks(7) + Adcirc::CDate::days(36) +
                     Adcirc::CDate::hours(49) + Adcirc::CDate::minutes(64) +
                     Adcirc::CDate::seconds(65) + 20 + 1.5;
  std::cout << d2 << std::endl;
  std::vector<int> d2v = d2.get();
  std::vector<int> d2v_expected = {2021, 5, 27, 2, 5, 26, 500};
  if (d2v != d2v_expected) {
    std::cout << "Error: incorrect date addition" << std::endl;
  }

  Adcirc::CDate d3 = d - Adcirc::CDate::years(1) - Adcirc::CDate::months(14) -
                     Adcirc::CDate::weeks(7) - Adcirc::CDate::days(36) -
                     Adcirc::CDate::hours(49) - Adcirc::CDate::minutes(64) -
                     Adcirc::CDate::seconds(65) - 20 - 1.5;
  std::cout << d3 << std::endl;
  std::vector<int> d3v = d3.get();
  std::vector<int> d3v_expected{2016, 8, 5, 21, 54, 33, 500};
  if (d3v != d3v_expected) {
    std::cout << "Error: incorrect date subtraction" << std::endl;
  }

  return 0;
}
