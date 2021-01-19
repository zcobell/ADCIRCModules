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
#include "GriddataAverageNearestNPoints.h"

using namespace Adcirc::Private;

GriddataAverageNearestNPoints::GriddataAverageNearestNPoints(
    const Adcirc::Raster::Rasterdata *raster,
    const GriddataAttribute *attribute, const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataAverageNearestNPoints::computeFromRaster() const {
  const auto maxPoints = static_cast<size_t>(attribute()->filterSize());
  const double w = this->calculateExpansionLevelForPoints(maxPoints);
  const auto values = this->pixelDataInSpecifiedRadius<double>(w);

  if (values.code() == 0) {
    std::vector<std::tuple<double, double>> pts;
    pts.reserve(values.pixels().size());

    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        pts.emplace_back(
            Constants::distance(attribute()->point(), v.location()), v.value());
      }
    }

    if (pts.empty()) return GriddataMethod::methodErrorValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      GriddataMethod::sortPointsByIncreasingDistance);

    double val = 0.0;
    auto np_fin = std::min(np, pts.size());

    for (size_t i = 0; i < np_fin; i++) {
      val += std::get<1>(pts[i]);
    }
    return val / static_cast<double>(np_fin);
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataAverageNearestNPoints::computeFromLookup() const {
  const auto maxPoints = static_cast<size_t>(attribute()->filterSize());
  const double w = this->calculateExpansionLevelForPoints(maxPoints);
  const auto values = this->pixelDataInSpecifiedRadius<int>(w);

  if (values.code() == 0) {
    std::vector<std::tuple<double, double>> pts;
    pts.reserve(values.pixels().size());

    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = config()->getKeyValue(v.value());
        if (zl != config()->defaultValue()) {
          pts.emplace_back(
              Constants::distance(attribute()->point(), v.location()), zl);
        }
      }
    }
    if (pts.empty()) return GriddataMethod::methodErrorValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      sortPointsByIncreasingDistance);

    double val = 0.0;
    auto np_fin = std::min(np, pts.size());
    for (size_t i = 0; i < np_fin; ++i) {
      val += std::get<1>(pts[i]);
    }
    return val / static_cast<double>(np_fin);
  }
  return GriddataMethod::methodErrorValue();
}