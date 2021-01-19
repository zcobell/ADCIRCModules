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