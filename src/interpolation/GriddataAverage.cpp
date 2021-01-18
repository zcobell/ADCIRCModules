//
// Created by Zach Cobell on 1/17/21.
//

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