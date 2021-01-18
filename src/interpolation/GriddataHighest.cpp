//
// Created by Zach Cobell on 1/17/21.
//

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