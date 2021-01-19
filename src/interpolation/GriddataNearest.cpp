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

#include "GriddataNearest.h"
using namespace Adcirc::Private;

GriddataNearest::GriddataNearest(const Adcirc::Raster::Rasterdata *raster,
                                 const GriddataAttribute *attribute,
                                 const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataNearest::computeFromRaster() const {
  Adcirc::Raster::Pixel px =
      this->raster()->coordinateToPixel(attribute()->point());

  Point pixel_location = this->raster()->pixelToCoordinate(px);
  double d = Constants::distance(attribute()->point(), pixel_location);

  if (d > attribute()->queryResolution()) {
    return GriddataMethod::methodErrorValue();
  } else {
    auto z = this->raster()->pixelValue<double>(px);
    return z != this->raster()->nodata<double>()
               ? z
               : GriddataMethod::methodErrorValue();
  }
}

double GriddataNearest::computeFromLookup() const {
  Adcirc::Raster::Pixel px =
      this->raster()->coordinateToPixel(attribute()->point());
  Point pixel_location = this->raster()->pixelToCoordinate(px);
  double d = Constants::distance(attribute()->point(), pixel_location);
  if (d > attribute()->queryResolution())
    return GriddataMethod::methodErrorValue();
  else {
    int z = this->raster()->pixelValue<int>(px);
    if (z != this->raster()->nodata<int>()) {
      double zl = config()->getKeyValue(z);
      if (zl != config()->defaultValue()) {
        return zl;
      } else {
        return GriddataMethod::methodErrorValue();
      }
    } else {
      return GriddataMethod::methodErrorValue();
    }
  }
}