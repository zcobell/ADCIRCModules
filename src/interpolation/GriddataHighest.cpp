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

#include "GriddataHighest.h"

using namespace Adcirc::Private;

GriddataHighest::GriddataHighest(const Adcirc::Raster::Rasterdata *raster,
                                 const GriddataAttribute *attribute,
                                 const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataHighest::computeFromRaster() const {
  const auto values = this->pixelDataInRadius<double>();
  if (values.code() == 0) {
    double zm = -std::numeric_limits<double>::max();
    for (const auto &v : values.pixels()) {
      if (v.valid() && v.value() > zm) {
        zm = v.value();
      }
    }
    return zm == -std::numeric_limits<double>::max()
               ? GriddataMethod::methodErrorValue()
               : zm;
  } else {
    return GriddataMethod::methodErrorValue();
  }
}

double GriddataHighest::computeFromLookup() const {
  const auto values = this->pixelDataInRadius<int>();
  if (values.code() == 0) {
    double zm = -std::numeric_limits<double>::max();
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = config()->getKeyValue(v.value());
        if (zl != config()->defaultValue()) {
          if (zl > zm) {
            zm = zl;
          }
        }
      }
    }
    return zm != -std::numeric_limits<double>::max()
               ? zm
               : GriddataMethod::methodErrorValue();
  }
  return GriddataMethod::methodErrorValue();
}