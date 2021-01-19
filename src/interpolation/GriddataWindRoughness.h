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

#ifndef ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_
#define ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_

#include <array>

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {

class GriddataWindRoughness : public GriddataMethod {
 public:
  GriddataWindRoughness(const Adcirc::Raster::Rasterdata *raster,
                        const GriddataAttribute *attribute,
                        const GriddataConfig *config);

  std::vector<double> computeMultipleFromRaster() const override;

  std::vector<double> computeMultipleFromLookup() const override;

  static constexpr double distanceFactor() { return 1.0 / 1000.0; }

  static constexpr double windRadius() { return 10000.0; }

  static constexpr double windSigma() { return 6.0; }

 private:
  static void computeWeightedDirectionalWindValues(
      const std::array<double, 12> &weight, std::vector<double> &wind,
      double nearWeight);

  static std::tuple<bool, double, char> computeWindDirectionAndWeight(
      const Point &p1, const Point &p2);

  static double gaussian(double distance);

  template <typename T>
  static int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }
};

}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_
