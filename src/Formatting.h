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
  static std::string adcircFileHeader(size_t numSnaps, size_t numNodes,
                                      const double dt, long long dIter,
                                      size_t dimension);

  static std::string adcircFullFormatRecordHeader(double time,
                                                  long long iteration);

  static std::string adcircSparseFormatRecordHeader(double time,
                                                    long long iterations,
                                                    size_t numNonDefault,
                                                    double defaultValue);
  static std::string adcircScalarLineFormat(size_t id, double value);

  static std::string adcircVectorLineFormat(size_t id, double value1,
                                            double value2);

  static std::string adcirc3dLineFormat(size_t id, double value1, double value2,
                                        double value3);
};

}  // namespace Output

}  // namespace Adcirc

#endif  // ADCIRCASCIIFORMAT_H
