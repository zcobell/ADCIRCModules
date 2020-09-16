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
#include <fstream>
#include <numeric>
#include <utility>

#include "boost/progress.hpp"
#include "constants.h"
#include "default_values.h"
#include "elementtable.h"
#include "fileio.h"
#include "griddata.h"
#include "logging.h"
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
static const double c_rootWindSigmaTwoPi =
    sqrt(2.0 * 4.0 * Constants::pi() * GriddataPrivate::windSigma());

constexpr std::array<std::array<short, 7>, 3> c_windDirectionLookup = {
    {{{4, 3, 2, 1, 12, 11, 10}},
     {{4, 0, 0, 0, 0, 0, 10}},
     {{4, 5, 6, 7, 8, 9, 10}}}};

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

bool GriddataPrivate::getKeyValue(unsigned short key, double &value) {
  auto t = this->m_lookup.find(key);
  if (t == this->m_lookup.end()) return false;
  value = t->second;
  return true;
}

Griddata::~Griddata() = default;

//GriddataPrivate::GriddataPrivate()
//    : 
//      m_calculatePointPtr(nullptr),
//      m_calculateDwindPtr(nullptr),
//      m_mesh(nullptr),
//      m_filterSize(0),
//      m_defaultValue(-9999.0),
//      m_raster(nullptr),
//      m_rasterFile(std::string()),
//      m_interpolationFlags(0),
//      m_epsg(4326),
//      m_thresholdMethod(Interpolation::Threshold::NoThreshold),
//      m_rasterMultiplier(1.0),
//      m_datumShift(0.0),
//      m_thresholdValue(0.0),
//      m_showProgressBar(false),
//      m_rasterInMemory(false) {}

GriddataPrivate::GriddataPrivate(Mesh *mesh, const std::string &rasterFile)
    : m_calculatePointPtr(nullptr),
      m_calculateDwindPtr(nullptr),
      m_mesh(mesh),
      m_filterSize(m_mesh->numNodes(),1.0),
      m_defaultValue(-9999.0),
      m_raster(new Adcirc::Raster::Rasterdata(rasterFile)),
      m_rasterFile(rasterFile),
      m_interpolationFlags(m_mesh->numNodes(),Average),
      m_epsg(4326),
      m_thresholdMethod(Interpolation::Threshold::NoThreshold),
      m_rasterMultiplier(1.0),
      m_datumShift(0.0),
      m_thresholdValue(0.0),
      m_showProgressBar(false),
      m_rasterInMemory(false){}

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
    size_t cls = StringConversion::stringToSizet(ls[0], ok);
    double v = StringConversion::stringToDouble(ls[1], ok);
    this->m_lookup[cls] = v;
  }
  fid.close();
  return;
}

std::vector<int> GriddataPrivate::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void GriddataPrivate::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
  this->m_interpolationFlags = interpolationFlags;
}

void GriddataPrivate::setInterpolationFlags(int interpolationFlag) {
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), interpolationFlag);
}

void GriddataPrivate::setInterpolationFlag(size_t index, int flag) {
  if (index < this->m_interpolationFlags.size()) {
    this->m_interpolationFlags[index] =
        static_cast<Interpolation::Method>(flag);
  }
  return;
}

int GriddataPrivate::interpolationFlag(size_t index) {
  return index < this->m_interpolationFlags.size()
             ? this->m_interpolationFlags[index]
             : Interpolation::Method::NoMethod;
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
  return;
}

double GriddataPrivate::defaultValue() const { return this->m_defaultValue; }

void GriddataPrivate::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

bool GriddataPrivate::rasterInMemory() const { return this->m_rasterInMemory; }

void GriddataPrivate::setRasterInMemory(bool rasterInMemory) {
  this->m_rasterInMemory = rasterInMemory;
}

template <typename T>
bool GriddataPrivate::pixelDataInRadius(Point &p, double radius,
                                        size_t &n,
                                        std::vector<double> &x,
                                        std::vector<double> &y,
                                        std::vector<T> &z,
                                        std::vector<bool> &valid) {
  Adcirc::Raster::Pixel ul, lr;
  this->m_raster.get()->searchBoxAroundPoint(p.first, p.second, radius, ul, lr);
  bool r = false;
  n = 0;

  if (ul.isValid() && lr.isValid()) {
    this->m_raster.get()->pixelValues<T>(ul.i(), ul.j(), lr.i(), lr.j(), n, x,
                                         y, z);

    if (valid.size() != x.size()) valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < n; ++i) {
      if (z[i] != this->m_raster.get()->nodata<T>()) {
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

double GriddataPrivate::calculatePoint(Point &p, double searchRadius,
                                       double gsMultiplier,
                                       Interpolation::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverage(p, searchRadius * gsMultiplier);
    case Nearest:
      return this->calculateNearest(p, searchRadius * gsMultiplier);
    case Highest:
      return this->calculateHighest(p, searchRadius * gsMultiplier);
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviation(
          p, searchRadius * gsMultiplier, 2);
    case BilskieEtAll:
      return this->calculateBilskieAveraging(p, searchRadius, gsMultiplier);
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeighted(
          p, searchRadius * gsMultiplier);
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPoints(p, gsMultiplier);
    case AverageNearestNPoints:
      return this->calculateAverageNearestN(p, gsMultiplier);
    default:
      return this->defaultValue();
  }
}

double GriddataPrivate::calculatePointFromLookup(Point &p, double searchRadius,
                                                 double gsMultiplier,
                                                 Interpolation::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverageFromLookup(p, searchRadius * gsMultiplier);
    case Nearest:
      return this->calculateNearestFromLookup(p, searchRadius * gsMultiplier);
    case Highest:
      return this->calculateHighestFromLookup(p, searchRadius * gsMultiplier);
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviationFromLookup(
          p, searchRadius * gsMultiplier, 2);
    case BilskieEtAll:
      return this->calculateBilskieAveragingFromLookup(p, searchRadius,
                                                       gsMultiplier);
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeightedFromLookup(
          p, searchRadius * gsMultiplier);
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPointsFromLookup(
          p, gsMultiplier);
    case AverageNearestNPoints:
      return this->calculateAverageNearestNFromLookup(p, gsMultiplier);
    default:
      return this->defaultValue();
  }
}

double GriddataPrivate::calculateAverage(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t np = 0;
  if (this->pixelDataInRadius(p, w, np, x, y, z, v)) {
    double a = 0.0;
    size_t n = 0;
    for (size_t i = 0; i < np; ++i) {
      if (v[i]) {
        a += z[i];
        n++;
      }
    }
    return n > 0 ? a / static_cast<double>(n) : this->defaultValue();
  } else {
    return this->defaultValue();
  }
}

double GriddataPrivate::calculateAverageFromLookup(Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t np = 0;
  if (this->pixelDataInRadius(p, w, np, x, y, z, v)) {
    size_t n = 0;
    double a = 0.0;
    for (size_t i = 0; i < np; ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          a += zl;
          n++;
        }
      }
    }
    return (n > 0 ? a / static_cast<double>(n) : this->defaultValue());
  } else {
    return this->defaultValue();
  }
}

double GriddataPrivate::calculateBilskieAveraging(Point &p, double w,
                                                  double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, this->m_raster.get()->dx(), r)) {
    return this->calculateAverage(p, r * gsMultiplier);
  } else {
    return this->calculateNearest(p, w * gsMultiplier);
  }
}

double GriddataPrivate::calculateBilskieAveragingFromLookup(
    Point &p, double w, double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, this->m_raster.get()->dx(), r)) {
    return this->calculateAverageFromLookup(p, r * gsMultiplier);
  } else {
    return this->calculateNearestFromLookup(p, w * gsMultiplier);
  }
}

double GriddataPrivate::calculateInverseDistanceWeighted(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t np = 0;
  if (this->pixelDataInRadius(p, w, np, x, y, z, v)) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;
    for (size_t i = 0; i < np; ++i) {
      if (v[i]) {
        double dis = Constants::distance(p, x[i], y[i]);
        n += z[i] / dis;
        d += 1.0 / dis;
        num++;
      }
    }
    return num > 0 ? n / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateInverseDistanceWeightedNPoints(Point &p,
                                                                double n) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);

  double w = this->calculateExpansionLevelForPoints(maxPoints);

  size_t onp = 0;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    double val = 0.0;
    double d = 0.0;
    size_t np = 0;
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        double dis = Constants::distance(p, x[i], y[i]);
        val += z[i] / dis;
        d += 1.0 / dis;
        np++;
      }
      if (np >= maxPoints) break;
    }
    return np > 0 ? val / d : this->defaultValue();
  }
  return this->defaultValue();
}

bool sortPointsByIncreasingDistance(Point &a, Point &b) {
  return a.first < b.first;
}

double GriddataPrivate::calculateAverageNearestN(Point &p, double n) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;
  size_t onp = 0;

  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    pts.reserve(z.size());
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        pts.push_back(Point(Constants::distance(p, x[i], y[i]), z[i]));
      }
    }

    if (pts.size() == 0) return this->defaultValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      sortPointsByIncreasingDistance);

    double val = 0.0;
    for (size_t i = 0; i < np; i++) {
      val += pts[i].second;
    }
    return val / static_cast<double>(np);
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateAverageNearestNFromLookup(Point &p, double n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;
  size_t onp = 0;

  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    pts.reserve(onp);
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          pts.push_back(Point(Constants::distance(p, x[i], y[i]), zl));
        }
      }
    }

    if (pts.size() == 0) return this->defaultValue();

    size_t np = std::min(pts.size(), maxPoints);
    std::partial_sort(pts.begin(), pts.begin() + np, pts.end(),
                      sortPointsByIncreasingDistance);

    double val = 0.0;
    for (size_t i = 0; i < np; i++) {
      val += pts[i].second;
    }
    return val / static_cast<double>(np);
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateInverseDistanceWeightedFromLookup(Point &p,
                                                                   double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t onp = 0;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    double n = 0.0;
    double d = 0.0;
    size_t num = 0;
    for (size_t i = 0; i < onp; ++i) {
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
    return num > 0 ? n / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateExpansionLevelForPoints(size_t n) {
  //...This tries to build out a box around a point at the
  // resolution of the raster. The hope is that by going 2
  // additional levels outside of what would be needed for the
  // requested number of points, we'll always hit the request
  // unless we're in a severe nodata region
  int levels = std::floor(n / 8.0) + 2;
  return this->m_raster.get()->dx() * static_cast<double>(levels);
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
    Point &p, double n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);

  double w = calculateExpansionLevelForPoints(n);
  size_t onp = 0;

  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    double val = 0.0;
    double d = 0.0;
    size_t np = 0;
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          double dis = Constants::distance(p, x[i], y[i]);
          val += zl / dis;
          d += 1.0 / dis;
          np++;
        }
      }
      if (np >= maxPoints) break;
    }
    return np > 0 ? val / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateOutsideStandardDeviation(Point &p, double w,
                                                          int n) {
  std::vector<double> x, y, z, z2;
  std::vector<bool> v;
  size_t onp = 0;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    z2.reserve(z.size());
    for (size_t i = 0; i < onp; ++i) {
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
    for (size_t i = 0; i < z2.size(); i++) {
      if (z2[i] >= cutoff) {
        a += z2[i];
        np++;
      }
    }
    return np > 0 ? a / static_cast<double>(np) : this->calculateAverage(p, w);
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateOutsideStandardDeviationFromLookup(Point &p,
                                                                    double w,
                                                                    int n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t onp;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    std::vector<double> z2;
    z2.reserve(z.size());
    for (size_t i = 0; i < onp; ++i) {
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
    for (size_t i = 0; i < z2.size(); i++) {
      if (z2[i] >= cutoff) {
        a += z2[i];
        np++;
      }
    }
    return np > 0 ? a / static_cast<double>(np)
                  : this->calculateAverageFromLookup(p, w);
  }
  return this->defaultValue();
}

double GriddataPrivate::calculateNearest(Point &p, double w) {
  Adcirc::Raster::Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w) {
    return this->defaultValue();
  } else {
    double z = this->m_raster.get()->pixelValue<double>(px);
    return z != this->m_raster.get()->nodata<double>() ? z
                                                       : this->defaultValue();
  }
}

double GriddataPrivate::calculateNearestFromLookup(Point &p, double w) {
  Adcirc::Raster::Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w)
    return this->defaultValue();
  else {
    int z = this->m_raster.get()->pixelValue<int>(px);
    if (z != this->m_raster.get()->nodata<int>()) {
      double zl;
      return this->getKeyValue(z, zl) ? zl : this->defaultValue();
    } else {
      return this->defaultValue();
    }
  }
}

double GriddataPrivate::calculateHighest(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t onp = 0;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    double zm = std::numeric_limits<double>::min();
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        if (z[i] > zm) {
          zm = z[i];
        }
      }
    }
    return zm;
  } else {
    return this->defaultValue();
  }
}

double GriddataPrivate::calculateHighestFromLookup(Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t onp = 0;
  if (this->pixelDataInRadius(p, w, onp, x, y, z, v)) {
    double zm = std::numeric_limits<double>::min();
    for (size_t i = 0; i < onp; ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          if (zl > zm) {
            zm = zl;
          }
        }
      }
    }
    return zm != std::numeric_limits<double>::min() ? zm : this->defaultValue();
  }
  return this->defaultValue();
}

bool GriddataPrivate::computeWindDirectionAndWeight(Point &p, double x,
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

    short a = static_cast<short>(sgn(dx));
    short b = static_cast<short>(k * sgn(dy));
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
  return;
}

std::vector<double> GriddataPrivate::calculateDirectionalWindFromRaster(
    Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y, z;
  std::vector<bool> v;

  std::vector<double> weight(12,0.0);
  std::vector<double> wind(12,0.0);
  size_t onp = 0;

  this->pixelDataInRadius(p, this->windRadius(), onp, x, y, z, v);

  for (size_t i = 0; i < onp; ++i) {
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

std::vector<double> GriddataPrivate::calculateDirectionalWindFromLookup(
    Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;

  std::vector<double> wind(12,0.0);
  std::vector<double> weight(12,0.0);
  size_t n = 0;

  this->pixelDataInRadius(p, this->windRadius(), n, x, y, z, v);
  if(std::all_of(z.begin(),z.end(),[&](const double &r){return r==z.front();})){
    if(v.front()){    
        double zl = 0.0;
        if(this->getKeyValue(z.front(),zl)){
            std::fill(wind.begin(),wind.end(),zl);
        }
    }
    return wind;
  }

  for (size_t i = 0; i < n; ++i) {
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

void GriddataPrivate::checkMatchingCoorindateSystems() {
  if (this->m_epsg != this->m_mesh->projection()) {
    adcircmodules_throw_exception(
        "You must use the same coordinate systems for mesh and raster.");
  }
}

void GriddataPrivate::checkRasterOpen() {
  if (!this->m_raster.get()->isOpen()) {
    bool success = this->m_raster.get()->open();
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

std::vector<double> GriddataPrivate::computeValuesFromRaster(
    bool useLookupTable) {
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->assignInterpolationFunctionPointer(useLookupTable);
  std::unique_ptr<boost::progress_display> progress(nullptr);

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  std::vector<double> gridsize = this->m_mesh->computeMeshSize();
  std::vector<double> result;
  result.resize(this->m_mesh->numNodes());

  if (this->showProgressBar()) {
    progress = std::unique_ptr<boost::progress_display>(
        new boost::progress_display(this->m_mesh->numNodes()));
  }

#pragma omp parallel for schedule(dynamic) shared(progress)
  for (signed long long i = 0;
       i < static_cast<signed long long>(this->m_mesh->numNodes()); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress.get());
    }

    Point p(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    Method m = static_cast<Method>(this->m_interpolationFlags[i]);
    double v = (this->*m_calculatePointPtr)(p, gridsize[i] * 0.5,
                                            this->m_filterSize[i], m);
    result[i] = v * this->m_rasterMultiplier + this->m_datumShift;
  }

  return result;
}

template <typename T>
void GriddataPrivate::thresholdData(std::vector<T> &z, std::vector<bool> &v) {
  if (std::is_same<T, int>::value)
    adcircmodules_throw_exception(
        "Cannot use thresholding and integer rasters");

  if (this->thresholdMethod() == Interpolation::Threshold::NoThreshold) return;

  if (this->thresholdMethod() == Interpolation::Threshold::ThresholdAbove) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (!v[i]) continue;
      double zz = z[i] * this->rasterMultiplier() + this->datumShift();
      if (zz < this->thresholdValue()) {
        z[i] = this->defaultValue();
        v[i] = false;
      }
    }
  } else if (this->thresholdMethod() ==
             Interpolation::Threshold::ThresholdBelow) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (!v[i]) continue;
      double zz = z[i] * this->rasterMultiplier() + this->datumShift();
      if (zz > this->thresholdValue()) {
        z[i] = this->defaultValue();
        v[i] = false;
      }
    }
  }
  return;
}

std::vector<std::vector<double>>
GriddataPrivate::computeDirectionalWindReduction(bool useLookupTable) {
  std::unique_ptr<boost::progress_display> progress(nullptr);
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->assignDirectionalWindReductionFunctionPointer(useLookupTable);

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  if (this->showProgressBar()) {
    progress = std::unique_ptr<boost::progress_display>(
        new boost::progress_display(this->m_mesh->numNodes()));
  }

  std::vector<std::vector<double>> result(m_mesh->numNodes());
  for(auto &r : result){
      r = std::vector<double>(12,0.0);
  }

#pragma omp parallel for schedule(dynamic) shared(progress, result)
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress);
    }

    if (this->m_interpolationFlags[i] != NoMethod) {
      Point p(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
      result[i] = (this->*m_calculateDwindPtr)(p);
      if(m_datumShift!=0.0){
        for (auto &r : result[i]) {
          r += this->m_datumShift;
        }
      }
    }
  }

  return result;
}
