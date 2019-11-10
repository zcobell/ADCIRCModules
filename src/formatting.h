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
#ifndef ADCMOD_ADCIRCASCIIFORMAT_H
#define ADCMOD_ADCIRCASCIIFORMAT_H

#include <string>

namespace Adcirc {

namespace Output {

class Formatting {
 public:
  static std::string adcircFileHeader(const size_t numSnaps, const size_t numNodes,
                               const double dt, const long long dIter,
                               const size_t dimension);

  static std::string adcircFullFormatRecordHeader(const double time,
                                           const long long iteration);

  static std::string adcircSparseFormatRecordHeader(const double time,
                                             const long long iterations,
                                             const size_t numNonDefault,
                                             const double defaultValue);
  static std::string adcircScalarLineFormat(const size_t id, const double value);

  static std::string adcircVectorLineFormat(const size_t id, const double value1,
                                     const double value2);

  static std::string adcirc3dLineFormat(const size_t id, const double value1,
                                 const double value2, const double value3);

};

}  // namespace Output

}  // namespace Adcirc

#endif  // ADCIRCASCIIFORMAT_H
