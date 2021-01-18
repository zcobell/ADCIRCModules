//
// Created by Zach Cobell on 1/17/21.
//

#include "GriddataBilskie.h"

#include "GriddataAverage.h"
#include "GriddataNearest.h"

using namespace Adcirc::Private;

GriddataBilskie::GriddataBilskie(const Adcirc::Raster::Rasterdata *raster,
                                 const GriddataAttribute *attribute,
                                 const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

double GriddataBilskie::computeFromRaster() const {
  if (this->useBilskieRadius()) {
    const auto a =
        GriddataAttribute(attribute()->point(), 0.5, attribute()->resolution(),
                          Interpolation::Average, Interpolation::Average);
    GriddataAverage gd(raster(), &a, config());
    return gd.compute();
  } else {
    GriddataNearest gd(raster(), attribute(), config());
    return gd.compute();
  }
}

double GriddataBilskie::computeFromLookup() const {
  return this->computeFromRaster();
}

bool GriddataBilskie::useBilskieRadius() const {
  const double radius = (0.25 * 2.0 * attribute()->resolution());
  return radius / raster()->dx() >= 1.0;
}