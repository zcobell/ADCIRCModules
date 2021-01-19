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

#include "GriddataInverseDistanceWeightedNPoints.h"

using namespace Adcirc::Private;

GriddataInverseDistanceWeightedNPoints::GriddataInverseDistanceWeightedNPoints(
    const Adcirc::Raster::Rasterdata *raster,
    const GriddataAttribute *attribute, const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataInverseDistanceWeightedNPoints::computeFromRaster() const {
  const auto maxPoints = static_cast<size_t>(attribute()->filterSize());
  const double w = this->calculateExpansionLevelForPoints(maxPoints);
  const auto values = this->pixelDataInSpecifiedRadius<double>(w);

  if (values.code() == 0) {
    std::vector<std::tuple<double, double>> pts;
    pts.reserve(maxPoints);
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        pts.emplace_back(
            Constants::distance(attribute()->point(), v.location()), v.value());
      }
      if (pts.size() == maxPoints) break;
    }

    return pts.empty() ? GriddataMethod::methodErrorValue()
                       : GriddataInverseDistanceWeightedNPoints::
                             computeInverseDistanceValue(pts, maxPoints);
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataInverseDistanceWeightedNPoints::computeFromLookup() const {
  const auto maxPoints = static_cast<size_t>(attribute()->filterSize());
  const double w = this->calculateExpansionLevelForPoints(maxPoints);
  const auto values = this->pixelDataInSpecifiedRadius<int>(w);

  if (values.code() == 0) {
    std::vector<std::tuple<double, double>> pts;
    pts.reserve(maxPoints);
    for (const auto &v : values.pixels()) {
      if (v.valid()) {
        double zl = config()->getKeyValue(v.value());
        if (zl != config()->defaultValue()) {
          pts.emplace_back(
              Constants::distance(attribute()->point(), v.location()), zl);
        }
      }
      if (pts.size() == maxPoints) break;
    }

    return pts.empty() ? GriddataMethod::methodErrorValue()
                       : GriddataInverseDistanceWeightedNPoints::
                             computeInverseDistanceValue(pts, maxPoints);
  }
  return GriddataMethod::methodErrorValue();
}

double GriddataInverseDistanceWeightedNPoints::computeInverseDistanceValue(
    std::vector<std::tuple<double, double>> &pts, size_t maxPoints) {
  size_t np = std::min(pts.size(), maxPoints);
  std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                    GriddataMethod::sortPointsByIncreasingDistance);
  double val = 0.0;
  double d = 0.0;
  for (const auto &p : pts) {
    val += std::get<1>(p) / std::get<0>(p);
    d += 1.0 / std::get<0>(p);
  }
  return val / d;
}