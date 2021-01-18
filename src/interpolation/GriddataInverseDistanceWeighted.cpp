//
// Created by Zach Cobell on 1/17/21.
//

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