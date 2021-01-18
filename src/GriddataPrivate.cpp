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

#include "GriddataPrivate.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <numeric>
#include <utility>

#include "Constants.h"
#include "DefaultValues.h"
#include "ElementTable.h"
#include "FileIO.h"
#include "Griddata.h"
#include "GriddataMethod.h"
#include "GriddataMethodHeaders.h"
#include "Logging.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "StringConversion.h"

using namespace Adcirc;
using namespace Adcirc::Geometry;
using namespace Adcirc::Interpolation;
using namespace Adcirc::Private;

Griddata::~Griddata() = default;

GriddataPrivate::GriddataPrivate(Mesh *mesh, const std::string &rasterFile,
                                 int epsgRaster)
    : m_config(GriddataConfig(false, NoThreshold, 0.0, 0.0, 1.0, -9999.0,
                              std::vector<double>())),
      m_raster(std::make_unique<Adcirc::Raster::Rasterdata>(rasterFile)),
      m_rasterFile(rasterFile),
      m_epsg(epsgRaster),
      m_showProgressBar(false),
      m_rasterInMemory(false) {
  auto locations =
      Adcirc::Private::GriddataPrivate::meshToQueryPoints(mesh, epsgRaster);
  auto resolution = mesh->computeMeshSize(epsgRaster);
  assert(locations.size() == mesh->numNodes());
  assert(resolution.size() == mesh->numNodes());

  m_attributes.reserve(mesh->numNodes());
  for (size_t i = 0; i < mesh->numNodes(); ++i) {
    m_attributes.emplace_back(locations[i], 1.0, resolution[i], Average,
                              NoMethod);
  }
}

GriddataPrivate::GriddataPrivate(const std::vector<double> &x,
                                 const std::vector<double> &y,
                                 const std::vector<double> &resolution,
                                 const std::string &rasterFile, int epsgQuery,
                                 int epsgRaster)
    : m_config(GriddataConfig(false, NoThreshold, 0.0, 0.0, 1.0, -9999.0,
                              std::vector<double>())),
      m_raster(std::make_unique<Adcirc::Raster::Rasterdata>(rasterFile)),
      m_rasterFile(rasterFile),
      m_epsg(epsgRaster),
      m_showProgressBar(false),
      m_rasterInMemory(false) {
  assert(!x.empty());
  assert(x.size() == y.size());

  std::vector<Point> queryLocations;
  queryLocations.reserve(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    queryLocations.emplace_back(x[i], y[i]);
  }
  if (epsgQuery != epsgRaster) {
    queryLocations = GriddataPrivate::convertQueryPointCoordinates(
        queryLocations, epsgQuery, epsgRaster);
  }

  m_attributes.reserve(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    m_attributes.emplace_back(Point(queryLocations[i]), 1.0, resolution[i],
                              Average, NoMethod);
  }
}

std::vector<Adcirc::Point> GriddataPrivate::meshToQueryPoints(
    Adcirc::Geometry::Mesh *m, int epsgRaster) {
  std::vector<Adcirc::Point> qp;
  qp.reserve(m->numNodes());
  for (size_t i = 0; i < m->numNodes(); ++i) {
    qp.emplace_back(m->node(i)->x(), m->node(i)->y());
  }

  if (epsgRaster != m->projection()) {
    return GriddataPrivate::convertQueryPointCoordinates(qp, m->projection(),
                                                         epsgRaster);
  } else {
    return qp;
  }
}

std::vector<Point> GriddataPrivate::convertQueryPointCoordinates(
    const std::vector<Point> &input, int epsgInput, int epsgOutput) {
  std::vector<Point> output;
  bool isLatLon;
  Projection::transform(epsgInput, epsgOutput, input, output, isLatLon);
  return output;
}

std::string GriddataPrivate::rasterFile() const { return this->m_rasterFile; }

double GriddataPrivate::windRadius() {
  return GriddataWindRoughness::windRadius();
}

void GriddataPrivate::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

double GriddataPrivate::rasterMultiplier() const {
  return this->m_config.rasterMultiplier();
}

void GriddataPrivate::setRasterMultiplier(double rasterMultiplier) {
  this->m_config.setRasterMultiplier(rasterMultiplier);
}

bool GriddataPrivate::showProgressBar() const {
  return this->m_showProgressBar;
}

void GriddataPrivate::setShowProgressBar(bool showProgressBar) {
  this->m_showProgressBar = showProgressBar;
}

int GriddataPrivate::epsg() const { return this->m_epsg; }

void GriddataPrivate::setEpsg(int epsg) { this->m_epsg = epsg; }

void GriddataPrivate::readLookupTable(const std::string &lookupTableFile) {
  if (!FileIO::Generic::fileExists(lookupTableFile)) {
    adcircmodules_throw_exception("Lookup table file does not exist");
  }

  std::fstream fid(lookupTableFile);

  adcmap<unsigned, double> temp_lookup;
  unsigned c_max = 0;

  std::string l;
  while (std::getline(fid, l)) {
    l = StringConversion::sanitizeString(l);
    if (l.length() == 0) {
      break;
    }

    std::vector<std::string> ls;
    FileIO::Generic::splitString(l, ls);
    if (ls.size() < 2) {
      fid.close();
      adcircmodules_throw_exception("Could not read the lookup table.");
    }
    bool ok;
    unsigned cls = StringConversion::stringToSizet(ls[0], ok);
    double v = StringConversion::stringToDouble(ls[1], ok);
    temp_lookup[cls] = v;
    c_max = std::max(c_max, cls);
  }
  fid.close();

  std::vector<double> lookup;
  lookup.resize(c_max + 1, m_config.defaultValue());
  for (auto c : temp_lookup) {
    lookup[c.first] = c.second;
  }

  m_config.setLookupTable(lookup);
}

std::vector<Interpolation::Method> GriddataPrivate::interpolationFlags() const {
  std::vector<Interpolation::Method> method;
  method.reserve(m_attributes.size());
  for (const auto &a : m_attributes) {
    method.push_back(a.interpolationFlag());
  }
  return method;
}

void GriddataPrivate::setInterpolationFlags(
    const std::vector<Interpolation::Method> &interpolationFlags) {
  assert(interpolationFlags.size() == m_attributes.size());
  size_t count = 0;
  for (auto &a : m_attributes) {
    a.setInterpolationFlag(interpolationFlags[count]);
    count++;
  }
}

void GriddataPrivate::setInterpolationFlags(Method interpolationFlag) {
  for (auto &a : m_attributes) {
    a.setInterpolationFlag(interpolationFlag);
  }
}

void GriddataPrivate::setInterpolationFlag(size_t index,
                                           Interpolation::Method flag) {
  assert(index < m_attributes.size());
  if (index < this->m_attributes.size()) {
    m_attributes[index].setInterpolationFlag(flag);
  }
}

Interpolation::Method GriddataPrivate::interpolationFlag(size_t index) {
  assert(index < m_attributes.size());
  return m_attributes[index].interpolationFlag();
}

void GriddataPrivate::setBackupInterpolationFlags(
    const std::vector<Interpolation::Method> &interpolationFlags) {
  assert(interpolationFlags.size() == m_attributes.size());
  size_t count = 0;
  for (auto &a : m_attributes) {
    a.setBackupFlag(interpolationFlags[count]);
    count++;
  }
}

void GriddataPrivate::setBackupInterpolationFlags(
    Interpolation::Method interpolationFlag) {
  for (auto &a : m_attributes) {
    a.setBackupFlag(interpolationFlag);
  }
}

void GriddataPrivate::setBackupInterpolationFlag(size_t index,
                                                 Interpolation::Method flag) {
  assert(index < m_attributes.size());
  m_attributes[index].setBackupFlag(flag);
}

Interpolation::Method GriddataPrivate::backupInterpolationFlag(size_t index) {
  assert(index < m_attributes.size());
  return m_attributes[index].backupFlag();
}

std::vector<Interpolation::Method> GriddataPrivate::backupInterpolationFlags()
    const {
  std::vector<Interpolation::Method> flags;
  flags.reserve(m_attributes.size());
  for (const auto &a : m_attributes) {
    flags.push_back(a.backupFlag());
  }
  return flags;
}

std::vector<double> GriddataPrivate::filterSizes() const {
  std::vector<double> filter;
  filter.reserve(m_attributes.size());
  for (const auto &a : m_attributes) {
    filter.push_back(a.filterSize());
  }
  return filter;
}

void GriddataPrivate::setFilterSizes(const std::vector<double> &filterSize) {
  size_t count = 0;
  for (auto &a : m_attributes) {
    a.setFilterSize(filterSize[count]);
    count++;
  }
}

void GriddataPrivate::setFilterSizes(double &filterSize) {
  for (auto &a : m_attributes) {
    a.setFilterSize(filterSize);
  }
}

double GriddataPrivate::filterSize(size_t index) {
  assert(index < m_attributes.size());
  return m_attributes[index].filterSize();
}

void GriddataPrivate::setFilterSize(size_t index, double filterSize) {
  assert(index < m_attributes.size());
  m_attributes[index].setFilterSize(filterSize);
}

double GriddataPrivate::defaultValue() const {
  return this->m_config.defaultValue();
}

void GriddataPrivate::setDefaultValue(double defaultValue) {
  this->m_config.setDefaultValue(defaultValue);
}

bool GriddataPrivate::rasterInMemory() const { return this->m_rasterInMemory; }

void GriddataPrivate::setRasterInMemory(bool rasterInMemory) {
  this->m_rasterInMemory = rasterInMemory;
}

double GriddataPrivate::calculatePoint(const size_t index,
                                       const Interpolation::Method &method) {
  std::unique_ptr<GriddataMethod> calc_method;
  switch (method) {
    case Average:
      calc_method = std::make_unique<GriddataAverage>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case Nearest:
      calc_method = std::make_unique<GriddataNearest>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case Highest:
      calc_method = std::make_unique<GriddataHighest>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case PlusTwoSigma:
      calc_method = std::make_unique<GriddataStandardDeviation>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case BilskieEtAll:
      calc_method = std::make_unique<GriddataBilskie>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case InverseDistanceWeighted:
      calc_method = std::make_unique<GriddataInverseDistanceWeighted>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case InverseDistanceWeightedNPoints:
      calc_method = std::make_unique<GriddataInverseDistanceWeightedNPoints>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    case AverageNearestNPoints:
      calc_method = std::make_unique<GriddataAverageNearestNPoints>(
          m_raster.get(), &m_attributes[index], &m_config);
      break;
    default:
      return this->defaultValue();
  }
  return calc_method->compute();
}

Adcirc::Interpolation::Threshold GriddataPrivate::thresholdMethod() const {
  return this->m_config.thresholdMethod();
}

void GriddataPrivate::setThresholdMethod(
    const Interpolation::Threshold &thresholdMethod) {
  m_config.setThresholdMethod(thresholdMethod);
}

std::vector<double> GriddataPrivate::extents() const {
  if (this->m_raster) {
    if (!this->m_raster->isOpen()) {
      this->m_raster->open();
    }
    return std::vector<double>{this->m_raster->xmin(), this->m_raster->ymin(),
                               this->m_raster->xmax(), this->m_raster->ymax()};
  } else {
    return std::vector<double>(4);
  }
}

double GriddataPrivate::thresholdValue() const {
  return m_config.thresholdValue();
}

void GriddataPrivate::setThresholdValue(double filterValue) {
  m_config.setThresholdValue(filterValue);
}

double GriddataPrivate::datumShift() const {
  return this->m_config.datumShift();
}

void GriddataPrivate::setDatumShift(double datumShift) {
  this->m_config.setDatumShift(datumShift);
}

void GriddataPrivate::checkRasterOpen() {
  if (!this->m_raster->isOpen()) {
    bool success = this->m_raster->open();
    if (!success) {
      adcircmodules_throw_exception("Could not open the raster file.");
    }
  }
}

std::vector<double> GriddataPrivate::computeValuesFromRaster(
    bool useLookupTable) {
  this->checkRasterOpen();
  ProgressBar progress(m_attributes.size());

  if (this->m_rasterInMemory) {
    this->m_raster->read();
  }

  this->m_config.setUseLookup(useLookupTable);

  std::vector<double> result(m_attributes.size(), m_config.defaultValue());

  if (this->showProgressBar()) progress.begin();

#pragma omp parallel for schedule(dynamic) default(none) \
    shared(progress, result, useLookupTable)
  for (size_t i = 0; i < m_attributes.size(); ++i) {
    if (this->m_showProgressBar) progress.tick();
    if (m_attributes[i].interpolationFlag() != Interpolation::NoMethod) {
      double v = this->calculatePoint(i, m_attributes[i].interpolationFlag());
      if (v == GriddataMethod::methodErrorValue()) {
        v = this->calculatePoint(i, m_attributes[i].backupFlag());
      }
      result[i] = v;
    }
  }

  if (this->m_showProgressBar) progress.end();

  return result;
}

std::vector<std::vector<double>>
GriddataPrivate::computeDirectionalWindReduction(bool useLookupTable) {
  this->checkRasterOpen();

  if (this->m_rasterInMemory) {
    this->m_raster->read();
  }

  this->m_config.setUseLookup(useLookupTable);

  std::vector<std::vector<double>> result;
  result.resize(m_attributes.size());

  ProgressBar progress(m_attributes.size());
  if (this->showProgressBar()) progress.begin();

#pragma omp parallel for schedule(dynamic) default(none) \
    shared(progress, result, useLookupTable)
  for (size_t i = 0; i < m_attributes.size(); ++i) {
    if (this->m_showProgressBar) progress.tick();
    GriddataWindRoughness wind(m_raster.get(), &m_attributes[i], &m_config);
    result[i] = wind.computeMultiple();
  }

  if (this->m_showProgressBar) progress.end();

  return result;
}
