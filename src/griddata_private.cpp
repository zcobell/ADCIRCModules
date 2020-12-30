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

#include "griddata_private.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <numeric>
#include <utility>

#include "constants.h"
#include "default_values.h"
#include "elementtable.h"
#include "fileio.h"
#include "griddata.h"
#include "logging.h"
#include "progressbar.h"
#include "projection.h"
#include "stringconversion.h"

using namespace Adcirc;
using namespace Adcirc::Geometry;
using namespace Adcirc::Interpolation;
using namespace Adcirc::Private;

using Point = std::pair<double, double>;

//...A couple constants used within
constexpr double c_oneOver2MinusRoot3 = 1.0 / (2.0 - Constants::root3());
constexpr double c_oneOver2PlusRoot3 = 1.0 / (2.0 + Constants::root3());
constexpr double c_epsilonSquared = std::numeric_limits<double>::epsilon() *
    std::numeric_limits<double>::epsilon();
constexpr double c_oneOverWindSigmaSquared =
    1.0 / (GriddataPrivate::windSigma() * GriddataPrivate::windSigma());
const double c_rootWindSigmaTwoPi =
    sqrt(2.0 * 4.0 * Constants::pi() * GriddataPrivate::windSigma());

constexpr std::array<std::array<short, 7>, 3> c_windDirectionLookup = {
    {{{4, 3, 2, 1, 12, 11, 10}},
     {{4, 0, 0, 0, 0, 0, 10}},
     {{4, 5, 6, 7, 8, 9, 10}}}};

template<typename T>
int sgn(T val) { return (T(0) < val) - (val < T(0)); }

bool GriddataPrivate::getKeyValue(unsigned key, double &value) {
  assert(key < m_lookup.size());
  // if (key > m_lookup.size()) return false;
  value = m_lookup[key];
  return value != m_defaultValue;
}

Griddata::~Griddata() = default;

GriddataPrivate::GriddataPrivate(Mesh *mesh, const std::string &rasterFile,
                                 int epsgRaster)
    : m_thresholdMethod(Interpolation::Threshold::NoThreshold),
      m_raster(std::make_unique<Adcirc::Raster::Rasterdata>(rasterFile)),
      m_rasterFile(rasterFile), m_inputEpsg(mesh->projection()),
      m_epsg(epsgRaster), m_defaultValue(-9999.0), m_rasterMultiplier(1.0),
      m_datumShift(0.0), m_thresholdValue(0.0), m_showProgressBar(false),
      m_rasterInMemory(false), m_calculatePointPtr(nullptr), m_calculateDwindPtr(nullptr) {
  this->m_interpolationFlags.resize(mesh->numNodes());
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), Average);

  this->m_backupFlags.resize(mesh->numNodes());
  std::fill(this->m_backupFlags.begin(), m_backupFlags.end(), NoMethod);

  this->m_filterSize.resize(mesh->numNodes());
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), 1.0);

  this->m_queryLocations =
      Adcirc::Private::GriddataPrivate::meshToQueryPoints(mesh, epsgRaster);
  this->m_queryResolution = mesh->computeMeshSize(epsgRaster);
}

GriddataPrivate::GriddataPrivate(const std::vector<double> &x,
                                 const std::vector<double> &y,
                                 std::vector<double> resolution,
                                 const std::string &rasterFile, int epsgQuery,
                                 int epsgRaster)
    : m_thresholdMethod(Interpolation::Threshold::NoThreshold),
      m_raster(std::make_unique<Adcirc::Raster::Rasterdata>(rasterFile)),
      m_queryResolution(std::move(resolution)), m_rasterFile(rasterFile),
      m_inputEpsg(epsgQuery), m_epsg(epsgRaster), m_defaultValue(-9999.0),
      m_rasterMultiplier(1.0), m_datumShift(0.0), m_thresholdValue(0.0),
      m_showProgressBar(false), m_rasterInMemory(false), m_calculatePointPtr(nullptr), m_calculateDwindPtr(nullptr) {
  assert(x.size() == y.size());
  assert(!x.empty());
  this->m_interpolationFlags.resize(x.size());

  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), Average);

  this->m_backupFlags.resize(x.size());
  std::fill(this->m_backupFlags.begin(), m_backupFlags.end(), NoMethod);

  this->m_filterSize.resize(x.size());
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), 1.0);

  m_queryLocations.reserve(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    m_queryLocations.emplace_back(x[i], y[i]);
  }
  if (epsgQuery != epsgRaster) {
    m_queryLocations = GriddataPrivate::convertQueryPointCoordinates(
        m_queryLocations, epsgQuery, epsgRaster);
  }
}

std::vector<Point> GriddataPrivate::meshToQueryPoints(Adcirc::Geometry::Mesh *m,
                                                      int epsgRaster) {
  std::vector<Point> qp;
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

std::vector<std::pair<double, double>>
GriddataPrivate::convertQueryPointCoordinates(const std::vector<Point> &input,
                                              int epsgInput, int epsgOutput) {
  std::vector<std::pair<double, double>> output;
  bool isLatLon;
  Projection::transform(epsgInput, epsgOutput, input, output, isLatLon);
  return output;
}

std::string GriddataPrivate::rasterFile() const { return this->m_rasterFile; }

void GriddataPrivate::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

double GriddataPrivate::rasterMultiplier() const {
  return this->m_rasterMultiplier;
}

void GriddataPrivate::setRasterMultiplier(double rasterMultiplier) {
  this->m_rasterMultiplier = rasterMultiplier;
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

  m_lookup.resize(c_max + 1, m_defaultValue);
  for (auto c : temp_lookup) {
    m_lookup[c.first] = c.second;
  }
}

std::vector<Interpolation::Method> GriddataPrivate::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void GriddataPrivate::setInterpolationFlags(
    const std::vector<Interpolation::Method> &interpolationFlags) {
  this->m_interpolationFlags = interpolationFlags;
}

void GriddataPrivate::setInterpolationFlags(Method interpolationFlag) {
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), interpolationFlag);
}

void GriddataPrivate::setInterpolationFlag(size_t index,
                                           Interpolation::Method flag) {
  if (index < this->m_interpolationFlags.size()) {
    this->m_interpolationFlags[index] = flag;
  }
}

Interpolation::Method GriddataPrivate::interpolationFlag(size_t index) {
  return index < this->m_interpolationFlags.size()
         ? this->m_interpolationFlags[index]
         : Interpolation::Method::NoMethod;
}

void GriddataPrivate::setBackupInterpolationFlags(
    const std::vector<Interpolation::Method> &interpolationFlags) {
  assert(interpolationFlags.size() == m_backupFlags.size());
  this->m_backupFlags = interpolationFlags;
}

void GriddataPrivate::setBackupInterpolationFlags(
    Interpolation::Method interpolationFlag) {
  std::fill(this->m_backupFlags.begin(), this->m_backupFlags.end(),
            interpolationFlag);
}

void GriddataPrivate::setBackupInterpolationFlag(size_t index,
                                                 Interpolation::Method flag) {
  if (index < this->m_backupFlags.size()) {
    this->m_backupFlags[index] = flag;
  }
}

Interpolation::Method GriddataPrivate::backupInterpolationFlag(size_t index) {
  return index < this->m_backupFlags.size() ? this->m_backupFlags[index]
                                            : Interpolation::Method::NoMethod;
}

std::vector<Interpolation::Method>
GriddataPrivate::backupInterpolationFlags() const {
  return this->m_backupFlags;
}

std::vector<double> GriddataPrivate::filterSizes() const {
  return this->m_filterSize;
}

void GriddataPrivate::setFilterSizes(const std::vector<double> &filterSize) {
  this->m_filterSize = filterSize;
}

void GriddataPrivate::setFilterSizes(double &filterSize) {
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), filterSize);
}

double GriddataPrivate::filterSize(size_t index) {
  return index < this->m_filterSize.size() ? this->m_filterSize[index] : 0.0;
}

void GriddataPrivate::setFilterSize(size_t index, double filterSize) {
  if (index < this->m_filterSize.size()) {
    this->m_filterSize[index] = filterSize;
  }
}

double GriddataPrivate::defaultValue() const { return this->m_defaultValue; }

void GriddataPrivate::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

bool GriddataPrivate::rasterInMemory() const { return this->m_rasterInMemory; }

void GriddataPrivate::setRasterInMemory(bool rasterInMemory) {
  this->m_rasterInMemory = rasterInMemory;
}

template<typename T>
bool GriddataPrivate::pixelDataInRadius(Raster::Rasterdata *raster,
                                        const Point &p, double radius,
                                        std::vector<double> &x,
                                        std::vector<double> &y,
                                        std::vector<T> &z,
                                        std::vector<bool> &valid) {
  Adcirc::Raster::Pixel ul, lr;
  raster->searchBoxAroundPoint(p.first, p.second, radius, ul, lr);
  bool r = false;

  if (ul.isValid() && lr.isValid()) {
    raster->pixelValues<T>(ul.i(), ul.j(), lr.i(), lr.j(), x, y, z);
    valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != raster->nodata<T>()) {
        if (Constants::distance(p, x[i], y[i]) <= radius) {
          valid[i] = true;
          r = true;
        }
      }
    }

    if (this->thresholdMethod() != Interpolation::Threshold::NoThreshold) {
      this->thresholdData(z, valid);
    }

    return r;
  }
  return r;
}

bool GriddataPrivate::calculateBilskieRadius(double meshSize,
                                             double rasterCellSize,
                                             double &radius) {
  //...Note: Mesh size is passed in as a radius, so multiplied by 2 here
  radius = (0.25 * 2.0 * meshSize);
  return radius / rasterCellSize >= 1.0;
}

double GriddataPrivate::calculatePoint(Raster::Rasterdata *raster,
                                       const Point &p, double searchRadius,
                                       double gsMultiplier,
                                       Interpolation::Method method) {
  switch (method) {
    case Average:return this->calculateAverage(raster, p, searchRadius * gsMultiplier);
    case Nearest:return this->calculateNearest(raster, p, searchRadius * gsMultiplier);
    case Highest:return this->calculateHighest(raster, p, searchRadius * gsMultiplier);
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviation(
          raster, p, searchRadius * gsMultiplier, 2);
    case BilskieEtAll:
      return this->calculateBilskieAveraging(raster, p, searchRadius,
                                             gsMultiplier);
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeighted(raster, p,
                                                    searchRadius * gsMultiplier);
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPoints(raster, p,
                                                           gsMultiplier);
    case AverageNearestNPoints:return this->calculateAverageNearestN(raster, p, gsMultiplier);
    default:return this->defaultValue();
  }
}

double GriddataPrivate::calculatePointFromLookup(Raster::Rasterdata *raster,
                                                 const Point &p,
                                                 double searchRadius,
                                                 double gsMultiplier,
                                                 Interpolation::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverageFromLookup(raster, p,
                                              searchRadius * gsMultiplier);
    case Nearest:
      return this->calculateNearestFromLookup(raster, p,
                                              searchRadius * gsMultiplier);
    case Highest:
      return this->calculateHighestFromLookup(raster, p,
                                              searchRadius * gsMultiplier);
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviationFromLookup(
          raster, p, searchRadius * gsMultiplier, 2);
    case BilskieEtAll:
      return this->calculateBilskieAveragingFromLookup(raster, p, searchRadius,
                                                       gsMultiplier);
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeightedFromLookup(
          raster, p, searchRadius * gsMultiplier);
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPointsFromLookup(
          raster, p, gsMultiplier);
    case AverageNearestNPoints:return this->calculateAverageNearestNFromLookup(raster, p, gsMultiplier);
    default:return this->defaultValue();
  }
}

double GriddataPrivate::calculateAverage(Raster::Rasterdata *raster,
                                         const Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double a = 0.0;
    size_t n = 0;
    for (size_t i = 0; i < x.size(); ++i) {
      if (v[i]) {
        a += z[i];
        n++;
      }
    }
    return n > 0 ? a / static_cast<double>(n) : this->defaultValue();
  } else {
    return this->methodErrorValue();
  }
}

double GriddataPrivate::calculateAverageFromLookup(Raster::Rasterdata *raster,
                                                   const Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    size_t n = 0;
    double a = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          a += zl;
          n++;
        }
      }
    }
    return (n > 0 ? a / static_cast<double>(n) : this->methodErrorValue());
  } else {
    return this->defaultValue();
  }
}

double GriddataPrivate::calculateBilskieAveraging(Raster::Rasterdata *raster,
                                                  const Point &p, double w,
                                                  double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, raster->dx(), r)) {
    return this->calculateAverage(raster, p, r * gsMultiplier);
  } else {
    return this->calculateNearest(raster, p, w * gsMultiplier);
  }
}

double GriddataPrivate::calculateBilskieAveragingFromLookup(
    Raster::Rasterdata *raster, const Point &p, double w, double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, raster->dx(), r)) {
    return this->calculateAverageFromLookup(raster, p, r * gsMultiplier);
  } else {
    return this->calculateNearestFromLookup(raster, p, w * gsMultiplier);
  }
}

double
GriddataPrivate::calculateInverseDistanceWeighted(Raster::Rasterdata *raster,
                                                  const Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double dis = Constants::distance(p, x[i], y[i]);
        n += z[i] / dis;
        d += 1.0 / dis;
        num++;
      }
    }
    return num > 0 ? n / d : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateInverseDistanceWeightedNPoints(
    Raster::Rasterdata *raster, const Point &p, double n) {
  assert(n > 0.0);
  std::vector<double> x, y, z;
  std::vector<bool> v;
  auto maxPoints = static_cast<size_t>(n);

  double w = this->calculateExpansionLevelForPoints(maxPoints);

  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double val = 0.0;
    double d = 0.0;
    size_t np = 0;
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double dis = Constants::distance(p, x[i], y[i]);
        val += z[i] / dis;
        d += 1.0 / dis;
        np++;
      }
      if (np >= maxPoints)
        break;
    }
    return np > 0 ? val / d : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

bool sortPointsByIncreasingDistance(const Point &a, const Point &b) {
  return a.first < b.first;
}

double GriddataPrivate::calculateAverageNearestN(Raster::Rasterdata *raster,
                                                 const Point &p, double n) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  auto maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;

  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    pts.reserve(z.size());
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        pts.emplace_back(Constants::distance(p, x[i], y[i]), z[i]);
      }
    }

    if (pts.empty())
      return this->methodErrorValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      sortPointsByIncreasingDistance);

    double val = 0.0;
    for (size_t i = 0; i < np; i++) {
      val += pts[i].second;
    }
    return val / static_cast<double>(np);
  }
  return this->methodErrorValue();
}

double
GriddataPrivate::calculateAverageNearestNFromLookup(Raster::Rasterdata *raster,
                                                    const Point &p, double n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  auto maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;

  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    pts.reserve(z.size());
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          pts.emplace_back(Constants::distance(p, x[i], y[i]), zl);
        }
      }
    }

    if (pts.empty())
      return this->methodErrorValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      sortPointsByIncreasingDistance);

    double val = 0.0;
    for (size_t i = 0; i < np; i++) {
      val += pts[i].second;
    }
    return val / static_cast<double>(np);
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateInverseDistanceWeightedFromLookup(
    Raster::Rasterdata *raster, const Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          double dis = Constants::distance(p, x[i], y[i]);
          n += zl / dis;
          d += 1.0 / dis;
          num++;
        }
      }
    }
    return num > 0 ? n / d : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateExpansionLevelForPoints(size_t n) {
  //...This tries to build out a box around a point at the
  // resolution of the raster. The hope is that by going 2
  // additional levels outside of what would be needed for the
  // requested number of points, we'll always hit the request
  // unless we're in a severe nodata region
  auto levels = static_cast<int>(std::floor(n / 8.0) + 2);
  return this->m_raster->dx() * static_cast<double>(levels);
}

Adcirc::Interpolation::Threshold GriddataPrivate::thresholdMethod() const {
  return this->m_thresholdMethod;
}

void GriddataPrivate::setThresholdMethod(
    const Interpolation::Threshold &thresholdMethod) {
  m_thresholdMethod = thresholdMethod;
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

double GriddataPrivate::thresholdValue() const { return m_thresholdValue; }

void GriddataPrivate::setThresholdValue(double filterValue) {
  m_thresholdValue = filterValue;
}

double GriddataPrivate::calculateInverseDistanceWeightedNPointsFromLookup(
    Raster::Rasterdata *raster, const Point &p, double n) {
  assert(n > 0.0);
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  auto maxPoints = static_cast<size_t>(n);

  double w = calculateExpansionLevelForPoints(n);

  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double val = 0.0;
    double d = 0.0;
    size_t np = 0;
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          double dis = Constants::distance(p, x[i], y[i]);
          val += zl / dis;
          d += 1.0 / dis;
          np++;
        }
      }
      if (np >= maxPoints)
        break;
    }
    return np > 0 ? val / d : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateOutsideStandardDeviation(
    Raster::Rasterdata *raster, const Point &p, double w, int n) {
  std::vector<double> x, y, z, z2;
  std::vector<bool> v;
  z2.reserve(z.size());
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        z2.push_back(z[i]);
      }
    }
    double mean = std::accumulate(z2.begin(), z2.end(), 0.0) / z2.size();
    double stddev = sqrt(
        std::inner_product(z2.begin(), z2.end(), z2.begin(), 0.0) / z2.size() -
            (mean * mean));
    double cutoff = mean + n * stddev;
    double a = 0;
    size_t np = 0;
    for (double i : z2) {
      if (i >= cutoff) {
        a += i;
        np++;
      }
    }
    return np > 0 ? a / static_cast<double>(np) : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateOutsideStandardDeviationFromLookup(
    Raster::Rasterdata *raster, const Point &p, double w, int n) {
  std::vector<double> x, y, z2;
  std::vector<int> z;
  std::vector<bool> v;
  z2.reserve(z.size());
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          z2.push_back(zl);
        }
      }
    }
    double mean = std::accumulate(z2.begin(), z2.end(), 0.0) / z2.size();
    double stddev = std::sqrt(
        std::inner_product(z2.begin(), z2.end(), z2.begin(), 0.0) / z2.size() -
            (mean * mean));
    double cutoff = mean + n * stddev;
    double a = 0.0;
    size_t np = 0;
    for (double i : z2) {
      if (i >= cutoff) {
        a += i;
        np++;
      }
    }
    return np > 0 ? a / static_cast<double>(np) : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

double GriddataPrivate::calculateNearest(Raster::Rasterdata *raster,
                                         const Point &p, double w) {
  Adcirc::Raster::Pixel px = raster->coordinateToPixel(p);
  Point pxloc = raster->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w) {
    return this->methodErrorValue();
  } else {
    auto z = raster->pixelValue<double>(px);
    return z != raster->nodata<double>() ? z : this->methodErrorValue();
  }
}

double GriddataPrivate::calculateNearestFromLookup(Raster::Rasterdata *raster,
                                                   const Point &p, double w) {
  Adcirc::Raster::Pixel px = raster->coordinateToPixel(p);
  Point pxloc = raster->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w)
    return this->methodErrorValue();
  else {
    int z = raster->pixelValue<int>(px);
    if (z != raster->nodata<int>()) {
      double zl;
      return this->getKeyValue(z, zl) ? zl : this->methodErrorValue();
    } else {
      return this->methodErrorValue();
    }
  }
}

double GriddataPrivate::calculateHighest(Raster::Rasterdata *raster,
                                         const Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double zm = std::numeric_limits<double>::max();
    for (size_t i = 0; i < x.size(); ++i) {
      if (v[i]) {
        if (z[i] > zm) {
          zm = z[i];
        }
      }
    }
    return zm;
  } else {
    return this->methodErrorValue();
  }
}

double GriddataPrivate::calculateHighestFromLookup(Raster::Rasterdata *raster,
                                                   const Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(raster, p, w, x, y, z, v)) {
    double zm = -std::numeric_limits<double>::max();
    for (size_t i = 0; i < x.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          if (zl > zm) {
            zm = zl;
          }
        }
      }
    }
    return zm != -std::numeric_limits<double>::max() ? zm
                                                     : this->methodErrorValue();
  }
  return this->methodErrorValue();
}

bool GriddataPrivate::computeWindDirectionAndWeight(const Point &p, double x,
                                                    double y, double &w,
                                                    int &dir) {
  double dx = (x - p.first) * 0.001;
  double dy = (y - p.second) * 0.001;
  double d = dx * dx + dy * dy;

  w = 1.0 /
      (std::exp(0.5 * d * c_oneOverWindSigmaSquared) + c_rootWindSigmaTwoPi);
  if (d > c_epsilonSquared) {
    double tanxy = 10000000.0;
    if (std::abs(dx) > std::numeric_limits<double>::epsilon()) {
      tanxy = std::abs(dy / dx);
    }

    uint64_t k = std::min(1.0, tanxy * c_oneOver2MinusRoot3) +
        std::min(1.0, tanxy) +
        std::min(1.0, tanxy * c_oneOver2PlusRoot3);

    auto a = static_cast<short>(sgn(dx));
    auto b = static_cast<short>(k * sgn(dy));
    dir = c_windDirectionLookup[a + 1][b + 3] - 1;
    return true;
  }
  return false;
}

void GriddataPrivate::computeWeightedDirectionalWindValues(
    std::vector<double> &weight, std::vector<double> &wind, double nearWeight) {
  for (size_t i = 0; i < 12; ++i) {
    double w = weight[i] + nearWeight;
    if (w > 1e-12) {
      wind[i] = wind[i] / w;
    } else {
      wind[i] = 0.0;
    }
  }
}

std::vector<double>
GriddataPrivate::calculateDirectionalWindFromRaster(Raster::Rasterdata *raster,
                                                    const Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y, z, wind, weight;
  std::vector<bool> v;
  wind.resize(12);
  weight.resize(12);

  std::fill(weight.begin(), weight.end(), 0.0);
  std::fill(wind.begin(), wind.end(), 0.0);

  this->pixelDataInRadius(
      raster, p, Adcirc::Private::GriddataPrivate::windRadius(), x, y, z, v);

  for (size_t i = 0; i < x.size(); ++i) {
    if (v[i]) {
      double w = 0.0;
      int dir = 0;

      if (this->computeWindDirectionAndWeight(p, x[i], y[i], w, dir)) {
        weight[dir] += w;
        wind[dir] += w * z[i];
      } else {
        nearWeight += w;
      }
    }
  }

  this->computeWeightedDirectionalWindValues(weight, wind, nearWeight);
  return wind;
}

std::vector<double>
GriddataPrivate::calculateDirectionalWindFromLookup(Raster::Rasterdata *raster,
                                                    const Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y, wind, weight;
  std::vector<int> z;
  std::vector<bool> v;
  wind.resize(12);
  weight.resize(12);

  std::fill(weight.begin(), weight.end(), 0.0);
  std::fill(wind.begin(), wind.end(), 0.0);

  this->pixelDataInRadius(
      raster, p, Adcirc::Private::GriddataPrivate::windRadius(), x, y, z, v);

  for (size_t i = 0; i < x.size(); ++i) {
    if (v[i]) {
      double zl;
      if (this->getKeyValue(z[i], zl)) {
        double w = 0.0;
        int dir = 0;

        if (this->computeWindDirectionAndWeight(p, x[i], y[i], w, dir)) {
          weight[dir] += w;
          wind[dir] += w * zl;
        } else {
          nearWeight += w;
        }
      }
    }
  }

  this->computeWeightedDirectionalWindValues(weight, wind, nearWeight);
  return wind;
}

void GriddataPrivate::assignInterpolationFunctionPointer(bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculatePointPtr = &GriddataPrivate::calculatePointFromLookup;
  } else {
    this->m_calculatePointPtr = &GriddataPrivate::calculatePoint;
  }
}

double GriddataPrivate::datumShift() const { return this->m_datumShift; }

void GriddataPrivate::setDatumShift(double datumShift) {
  this->m_datumShift = datumShift;
}

void GriddataPrivate::checkRasterOpen() {
  if (!this->m_raster->isOpen()) {
    bool success = this->m_raster->open();
    if (!success) {
      adcircmodules_throw_exception("Could not open the raster file.");
    }
  }
}

void GriddataPrivate::assignDirectionalWindReductionFunctionPointer(
    bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculateDwindPtr =
        &GriddataPrivate::calculateDirectionalWindFromLookup;
  } else {
    this->m_calculateDwindPtr =
        &GriddataPrivate::calculateDirectionalWindFromRaster;
  }
}

std::vector<double>
GriddataPrivate::computeValuesFromRaster(bool useLookupTable) {
  this->checkRasterOpen();
  this->assignInterpolationFunctionPointer(useLookupTable);
  ProgressBar progress(m_queryLocations.size());

  // Per above, we're going to sidestep the case where we just
  // put the whole raster into memory
  if (this->m_rasterInMemory) {
    this->m_raster->read();
  }

  std::vector<double> result;
  result.resize(this->m_queryLocations.size());

  if (this->showProgressBar())
    progress.begin();

#pragma omp parallel default(none) shared(progress, result)
  {
    std::unique_ptr<Adcirc::Raster::Rasterdata> local_raster;
    Adcirc::Raster::Rasterdata *local_raster_ptr = nullptr;
    if (m_rasterInMemory) {
      local_raster_ptr = m_raster.get();
    } else {
      local_raster = std::make_unique<Adcirc::Raster::Rasterdata>(m_rasterFile);
      local_raster->open();
      local_raster_ptr = local_raster.get();
    }
#pragma omp for schedule(dynamic)
    for (signed long long i = 0;
         i < static_cast<signed long long>(m_queryLocations.size()); ++i) {
      if (this->m_showProgressBar)
        progress.tick();
      double v = (this->*m_calculatePointPtr)(
          local_raster_ptr, m_queryLocations[i], m_queryResolution[i] * 0.5,
          this->m_filterSize[i], m_interpolationFlags[i]);
      if (v == GriddataPrivate::methodErrorValue()) {
        v = (this->*m_calculatePointPtr)(local_raster_ptr, m_queryLocations[i],
                                         m_queryResolution[i] * 0.5,
                                         this->m_filterSize[i], m_backupFlags[i]);
        if (v == methodErrorValue())
          v = this->defaultValue();
      }
      result[i] = v * this->m_rasterMultiplier + this->m_datumShift;
    }
  }

  if (this->m_showProgressBar)
    progress.end();
  return result;
}

template<typename T>
void GriddataPrivate::thresholdData(std::vector<T> &z, std::vector<bool> &v) {
  if (std::is_same<T, int>::value)
    adcircmodules_throw_exception(
        "Cannot use thresholding and integer rasters");

  if (this->thresholdMethod() == Interpolation::Threshold::NoThreshold)
    return;

  if (this->thresholdMethod() == Interpolation::Threshold::ThresholdAbove) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (!v[i])
        continue;
      double zz = z[i] * this->rasterMultiplier() + this->datumShift();
      if (zz < this->thresholdValue()) {
        z[i] = this->defaultValue();
        v[i] = false;
      }
    }
  } else if (this->thresholdMethod() ==
      Interpolation::Threshold::ThresholdBelow) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (!v[i])
        continue;
      double zz = z[i] * this->rasterMultiplier() + this->datumShift();
      if (zz > this->thresholdValue()) {
        z[i] = this->defaultValue();
        v[i] = false;
      }
    }
  }
}

std::vector<std::vector<double>>
GriddataPrivate::computeDirectionalWindReduction(bool useLookupTable) {
  this->checkRasterOpen();
  this->assignDirectionalWindReductionFunctionPointer(useLookupTable);

  if (this->m_rasterInMemory) {
    this->m_raster->read();
  }

  std::vector<std::vector<double>> result;
  result.resize(m_queryLocations.size());

  ProgressBar progress(m_queryLocations.size());
  if (this->showProgressBar())
    progress.begin();

#pragma omp parallel default(none) shared(progress, result)
  {
    std::unique_ptr<Adcirc::Raster::Rasterdata> local_raster;
    Adcirc::Raster::Rasterdata *local_raster_ptr = nullptr;
    if (m_rasterInMemory) {
      local_raster_ptr = m_raster.get();
    } else {
      local_raster = std::make_unique<Adcirc::Raster::Rasterdata>(m_rasterFile);
      local_raster->open();
      local_raster_ptr = local_raster.get();
    }
#pragma omp for schedule(dynamic)
    for (unsigned long long i = 0;
         i < static_cast<unsigned long long>(m_queryLocations.size()); ++i) {
      if (this->m_showProgressBar)
        progress.tick();
      if (this->m_interpolationFlags[i] != NoMethod) {
        result[i] =
            (this->*m_calculateDwindPtr)(local_raster_ptr, m_queryLocations[i]);
      } else {
        result[i] = std::vector<double>(12, this->defaultValue());
      }
    }
  }

  if (this->m_showProgressBar)
    progress.end();

  return result;
}
