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
#include "GriddataAverage.h"

using namespace Adcirc::Private;

GriddataAverage::GriddataAverage(const Adcirc::Raster::Rasterdata *raster,
                                 const GriddataAttribute *attribute,
                                 const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataAverage::computeFromRaster() const {
  const auto values = this->pixelDataInRadius<double>();

  if (values.code() == 0) {
    double a = 0.0;
    size_t n = 0;
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        a += v.value();
        n++;
      }
    }
    return n > 0 ? a / static_cast<double>(n) : this->config()->defaultValue();
  } else {
    return GriddataAverage::methodErrorValue();
  }
}

double GriddataAverage::computeFromLookup() const {
  const auto values = this->pixelDataInRadius<int>();
  if (values.code() == 0) {
    size_t n = 0;
    double a = 0.0;
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = this->config()->getKeyValue(v.value());
        if (zl != this->config()->defaultValue()) {
          a += zl;
          n++;
        }
      }
    }
    return (n > 0 ? a / static_cast<double>(n)
                  : GriddataMethod::methodErrorValue());
  } else {
    return this->config()->defaultValue();
  }
}