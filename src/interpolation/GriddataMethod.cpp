//
// Created by Zach Cobell on 1/17/21.
//

#include "GriddataMethod.h"

using namespace Adcirc::Private;

GriddataMethod::GriddataMethod(const Adcirc::Raster::Rasterdata *raster,
                               const GriddataAttribute *attribute,
                               const GriddataConfig *config)
    : m_raster(raster), m_attribute(attribute), m_config(config) {}

const Adcirc::Private::GriddataAttribute *GriddataMethod::attribute() const {
  return m_attribute;
}

const Adcirc::Raster::Rasterdata *GriddataMethod::raster() const {
  return m_raster;
}

const GriddataConfig *GriddataMethod::config() const { return m_config; }

double GriddataMethod::compute() const {
  if (this->config()->useLookup()) {
    return this->computeFromLookup();
  } else {
    return this->computeFromRaster();
  }
}

std::vector<double> GriddataMethod::computeMultiple() const {
  if (this->config()->useLookup()) {
    return this->computeMultipleFromLookup();
  } else {
    return this->computeMultipleFromRaster();
  }
}