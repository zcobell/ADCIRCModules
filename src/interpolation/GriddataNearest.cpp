//
// Created by Zach Cobell on 1/17/21.
//

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