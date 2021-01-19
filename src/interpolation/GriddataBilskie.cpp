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
#include "GriddataBilskie.h"

#include "GriddataAverage.h"
#include "GriddataNearest.h"

using namespace Adcirc::Private;

GriddataBilskie::GriddataBilskie(const Adcirc::Raster::Rasterdata *raster,
                                 const GriddataAttribute *attribute,
                                 const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataBilskie::computeFromRaster() const {
  if (this->useBilskieRadius()) {
    const auto a =
        GriddataAttribute(attribute()->point(), 0.5, attribute()->resolution(),
                          Interpolation::Average, Interpolation::Average);
    GriddataAverage gd(raster(), &a, config());
    return gd.compute();
  } else {
    GriddataNearest gd(raster(), attribute(), config());
    return gd.compute();
  }
}

double GriddataBilskie::computeFromLookup() const {
  return this->computeFromRaster();
}

bool GriddataBilskie::useBilskieRadius() const {
  const double radius = (0.25 * 2.0 * attribute()->resolution());
  return radius / raster()->dx() >= 1.0;
}