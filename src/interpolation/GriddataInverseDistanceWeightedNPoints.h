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

#ifndef ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_
#define ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {
class GriddataInverseDistanceWeightedNPoints : public GriddataMethod {
 public:
  GriddataInverseDistanceWeightedNPoints(
      const Adcirc::Raster::Rasterdata *raster,
      const GriddataAttribute *attribute, const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;

 private:
  static double computeInverseDistanceValue(
      std::vector<std::tuple<double, double>> &pts, size_t maxPoints);
};
}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_
