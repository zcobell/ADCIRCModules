/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
//------------------------------------------------------------------------*/
#ifndef ADCMOD_FPCOMPARE_H
#define ADCMOD_FPCOMPARE_H

#include <vector>

#include "AdcircModules_Global.h"
#include "boost/math/special_functions/relative_difference.hpp"

namespace Adcirc {

namespace FpCompare {

template <typename T>
bool ADCIRCMODULES_EXPORT equalTo(const T &a, const T &b) {
  return !(boost::math::relative_difference(a, b) > 0);
}

template <typename T>
bool ADCIRCMODULES_EXPORT equalTo(const std::vector<T> &a,
                                  const std::vector<T> &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (!equalTo(a[i], b[i])) return false;
  }
  return true;
}

};  // namespace FpCompare
}  // namespace Adcirc

//...Explicit instantiations
template bool ADCIRCMODULES_EXPORT
Adcirc::FpCompare::equalTo<double>(const double &a, const double &b);
template bool ADCIRCMODULES_EXPORT
Adcirc::FpCompare::equalTo<float>(const float &a, const float &b);
template bool ADCIRCMODULES_EXPORT Adcirc::FpCompare::equalTo<double>(
    const std::vector<double> &a, const std::vector<double> &b);
template bool ADCIRCMODULES_EXPORT Adcirc::FpCompare::equalTo<float>(
    const std::vector<float> &a, const std::vector<float> &b);

#endif  // ADCMOD_FPCOMPARE_H
