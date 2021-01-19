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

#ifndef ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTED_H_
#define ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTED_H_

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {

class GriddataInverseDistanceWeighted : public GriddataMethod {
 public:
  GriddataInverseDistanceWeighted(const Adcirc::Raster::Rasterdata *raster,
                                  const GriddataAttribute *attribute,
                                  const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;
};

}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTED_H_
