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

#include "GriddataStandardDeviation.h"
using namespace Adcirc::Private;

GriddataStandardDeviation::GriddataStandardDeviation(
    const Adcirc::Raster::Rasterdata *raster,
    const GriddataAttribute *attribute, const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataStandardDeviation::computeFromRaster() const {
  const auto values = this->pixelDataInRadius<double>();
  if (values.code() == 0) {
    std::vector<double> z2;
    z2.reserve(values.pixels().size());
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        z2.push_back(v.value());
      }
    }
    return GriddataStandardDeviation::doAverageOutsideStandardDeviation(z2,
                                                                        m_n);
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataStandardDeviation::computeFromLookup() const {
  const auto values = this->pixelDataInRadius<int>();
  if (values.code() == 0) {
    std::vector<double> z2;
    z2.reserve(values.pixels().size());

    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = config()->getKeyValue(v.value());
        if (zl != config()->defaultValue()) {
          z2.push_back(zl);
        }
      }
    }
    return GriddataStandardDeviation::doAverageOutsideStandardDeviation(z2,
                                                                        m_n);
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataStandardDeviation::doAverageOutsideStandardDeviation(
    const std::vector<double> &v, int n) {
  double mean = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
  double stddev = std::sqrt(
      std::inner_product(v.begin(), v.end(), v.begin(), 0.0) / v.size() -
      (mean * mean));
  double cutoff = mean + n * stddev;
  double a = 0.0;
  size_t np = 0;
  for (double i : v) {
    if (i >= cutoff) {
      a += i;
      np++;
    }
  }
  return np > 0 ? a / static_cast<double>(np)
                : GriddataMethod::methodErrorValue();
}