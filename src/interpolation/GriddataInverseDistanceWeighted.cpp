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
#include "GriddataInverseDistanceWeighted.h"

using namespace Adcirc::Private;

GriddataInverseDistanceWeighted::GriddataInverseDistanceWeighted(
    const Adcirc::Raster::Rasterdata *raster,
    const GriddataAttribute *attribute, const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataInverseDistanceWeighted::computeFromRaster() const {
  const auto values = this->pixelDataInRadius<double>();
  if (values.code() == 0) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double dis = Constants::distance(attribute()->point(), v.location());
        n += v.value() / dis;
        d += 1 / dis;
        num++;
      }
    }
    return num > 0 ? n / d : GriddataMethod::methodErrorValue();
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataInverseDistanceWeighted::computeFromLookup() const {
  const auto values = this->pixelDataInRadius<int>();
  if (values.code() == 0) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;

    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = config()->getKeyValue(v.value());
        if (zl != config()->defaultValue()) {
          double dis = Constants::distance(attribute()->point(), v.location());
          n += zl / dis;
          d += 1.0 / dis;
        }
      }
    }
    return num > 0 ? n / d : GriddataMethod::methodErrorValue();
  }
  return GriddataMethod::methodErrorValue();
}