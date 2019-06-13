/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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

#include "griddata_impl.h"
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

using namespace Adcirc::Geometry;
using namespace Interpolation;

using Point = std::pair<double, double>;

//...A couple constants used within
constexpr double c_oneOver2MinusRoot3 = 1.0 / (2.0 - Constants::root3());
constexpr double c_oneOver2PlusRoot3 = 1.0 / (2.0 + Constants::root3());
constexpr double c_epsilonSquared = std::numeric_limits<double>::epsilon() *
                                    std::numeric_limits<double>::epsilon();
constexpr double c_oneOverWindSigmaSquared =
    1.0 / (GriddataImpl::windSigma() * GriddataImpl::windSigma());
static const double c_rootWindSigmaTwoPi =
    sqrt(2.0 * 4.0 * Constants::pi() * GriddataImpl::windSigma());

constexpr std::array<std::array<short, 7>, 3> c_windDirectionLookup = {
    {{{4, 3, 2, 1, 12, 11, 10}},
     {{4, 0, 0, 0, 0, 0, 10}},
     {{4, 5, 6, 7, 8, 9, 10}}}};

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

bool GriddataImpl::getKeyValue(unsigned short key, double &value) {
  auto t = this->m_lookup.find(key);
  if (t == this->m_lookup.end()) return false;
  value = t->second;
  return true;
}

Griddata::~Griddata() = default;

GriddataImpl::GriddataImpl()
    : m_mesh(nullptr),
      m_raster(nullptr),
      m_rasterFile(std::string()),
      m_interpolationFlags(std::vector<int>()),
      m_filterSize(std::vector<double>()),
      m_defaultValue(-9999.0),
      m_epsg(4326),
      m_datumShift(0.0),
      m_showProgressBar(false),
      m_rasterMultiplier(1.0),
      m_calculateDwindPtr(nullptr),
      m_calculatePointPtr(nullptr),
      m_thresholdValue(0.0),
      m_thresholdMethod(Interpolation::Threshold::NoThreshold),
      m_rasterInMemory(false) {}

GriddataImpl::GriddataImpl(Mesh *mesh, std::string rasterFile)
    : m_mesh(mesh),
      m_rasterFile(rasterFile),
      m_defaultValue(-9999.0),
      m_epsg(4326),
      m_datumShift(0.0),
      m_showProgressBar(false),
      m_rasterMultiplier(1.0),
      m_calculateDwindPtr(nullptr),
      m_calculatePointPtr(nullptr),
      m_thresholdValue(0.0),
      m_thresholdMethod(Interpolation::Threshold::NoThreshold) {
  this->m_raster.reset(new Rasterdata(this->m_rasterFile));
  this->m_interpolationFlags.resize(this->m_mesh->numNodes());
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), Average);
  this->m_filterSize.resize(this->m_mesh->numNodes());
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), 1.0);
}

std::string GriddataImpl::rasterFile() const { return this->m_rasterFile; }

void GriddataImpl::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

double GriddataImpl::rasterMultiplier() const {
  return this->m_rasterMultiplier;
}

void GriddataImpl::setRasterMultiplier(double rasterMultiplier) {
  this->m_rasterMultiplier = rasterMultiplier;
}

bool GriddataImpl::showProgressBar() const { return this->m_showProgressBar; }

void GriddataImpl::setShowProgressBar(bool showProgressBar) {
  this->m_showProgressBar = showProgressBar;
}

int GriddataImpl::epsg() const { return this->m_epsg; }

void GriddataImpl::setEpsg(int epsg) { this->m_epsg = epsg; }

void GriddataImpl::readLookupTable(const std::string &lookupTableFile) {
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

std::vector<int> GriddataImpl::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void GriddataImpl::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
  this->m_interpolationFlags = interpolationFlags;
}

void GriddataImpl::setInterpolationFlags(int interpolationFlag) {
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), interpolationFlag);
}

void GriddataImpl::setInterpolationFlag(size_t index, int flag) {
  if (index < this->m_interpolationFlags.size()) {
    this->m_interpolationFlags[index] =
        static_cast<Interpolation::Method>(flag);
  }
  return;
}

int GriddataImpl::interpolationFlag(size_t index) {
  return index < this->m_interpolationFlags.size()
             ? this->m_interpolationFlags[index]
             : Interpolation::Method::NoMethod;
}

std::vector<double> GriddataImpl::filterSizes() const {
  return this->m_filterSize;
}

void GriddataImpl::setFilterSizes(const std::vector<double> &filterSize) {
  this->m_filterSize = filterSize;
}

void GriddataImpl::setFilterSizes(double &filterSize) {
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), filterSize);
}

double GriddataImpl::filterSize(size_t index) {
  return index < this->m_filterSize.size() ? this->m_filterSize[index] : 0.0;
}

void GriddataImpl::setFilterSize(size_t index, double filterSize) {
  if (index < this->m_filterSize.size()) {
    this->m_filterSize[index] = filterSize;
  }
  return;
}

double GriddataImpl::defaultValue() const { return this->m_defaultValue; }

void GriddataImpl::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

bool GriddataImpl::rasterInMemory() const { return this->m_rasterInMemory; }

void GriddataImpl::setRasterInMemory(bool rasterInMemory) {
  this->m_rasterInMemory = rasterInMemory;
}

bool GriddataImpl::pixelDataInRadius(Point &p, double radius,
                                     std::vector<double> &x,
                                     std::vector<double> &y,
                                     std::vector<double> &z,
                                     std::vector<bool> &valid) {
  Pixel ul, lr;
  this->m_raster.get()->searchBoxAroundPoint(p.first, p.second, radius, ul, lr);
  bool r = false;

  if (ul.isValid() && lr.isValid()) {
    this->m_raster.get()->pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), x, y, z);
    valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != this->m_raster.get()->nodata()) {
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

bool GriddataImpl::pixelDataInRadius(Point &p, double radius,
                                     std::vector<double> &x,
                                     std::vector<double> &y,
                                     std::vector<int> &z,
                                     std::vector<bool> &valid) {
  Pixel ul, lr;
  this->m_raster.get()->searchBoxAroundPoint(p.first, p.second, radius, ul, lr);
  bool r = false;

  if (ul.isValid() && lr.isValid()) {
    this->m_raster.get()->pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), x, y, z);
    valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != this->m_raster.get()->nodataint()) {
        if (Constants::distance(p, x[i], y[i]) <= radius) {
          valid[i] = true;
          r = true;
        }
      }
    }
  }
  return r;
}

bool GriddataImpl::calculateBilskieRadius(double meshSize,
                                          double rasterCellSize,
                                          double &radius) {
  //...Note: Mesh size is passed in as a radius, so multiplied by 2 here
  radius = (0.25 * 2.0 * meshSize);
  return radius / rasterCellSize >= 1.0;
}

double GriddataImpl::calculatePoint(Point &p, double searchRadius,
                                    double gsMultiplier,
                                    Interpolation::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverage(p, searchRadius * gsMultiplier);
      break;
    case Nearest:
      return this->calculateNearest(p, searchRadius * gsMultiplier);
      break;
    case Highest:
      return this->calculateHighest(p, searchRadius * gsMultiplier);
      break;
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviation(
          p, searchRadius * gsMultiplier, 2);
      break;
    case BilskieEtAll:
      return this->calculateBilskieAveraging(p, searchRadius, gsMultiplier);
      break;
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeighted(
          p, searchRadius * gsMultiplier);
      break;
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPoints(p, gsMultiplier);
      break;
    case AverageNearestNPoints:
      return this->calculateAverageNearestN(p, gsMultiplier);
      break;
    default:
      return this->defaultValue();
      break;
  }
}

double GriddataImpl::calculatePointFromLookup(Point &p, double searchRadius,
                                              double gsMultiplier,
                                              Interpolation::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverageFromLookup(p, searchRadius * gsMultiplier);
      break;
    case Nearest:
      return this->calculateNearestFromLookup(p, searchRadius * gsMultiplier);
      break;
    case Highest:
      return this->calculateHighestFromLookup(p, searchRadius * gsMultiplier);
      break;
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviationFromLookup(
          p, searchRadius * gsMultiplier, 2);
      break;
    case BilskieEtAll:
      return this->calculateBilskieAveragingFromLookup(p, searchRadius,
                                                       gsMultiplier);
      break;
    case InverseDistanceWeighted:
      return this->calculateInverseDistanceWeightedFromLookup(
          p, searchRadius * gsMultiplier);
    case InverseDistanceWeightedNPoints:
      return this->calculateInverseDistanceWeightedNPointsFromLookup(
          p, gsMultiplier);
      break;
    case AverageNearestNPoints:
      return this->calculateAverageNearestNFromLookup(p, gsMultiplier);
      break;
    default:
      return this->defaultValue();
      break;
  }
}

double GriddataImpl::calculateAverage(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    double a = 0.0;
    size_t n = 0;
    for (size_t i = 0; i < x.size(); ++i) {
      if (v[i]) {
        a += z[i];
        n++;
      }
    }
    return n > 0 ? a / n : this->defaultValue();
  } else {
    return this->defaultValue();
  }
}

double GriddataImpl::calculateAverageFromLookup(Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
    return (n > 0 ? a / n : this->defaultValue());
  } else {
    return this->defaultValue();
  }
}

double GriddataImpl::calculateBilskieAveraging(Point &p, double w,
                                               double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, this->m_raster.get()->dx(), r)) {
    return this->calculateAverage(p, r * gsMultiplier);
  } else {
    return this->calculateNearest(p, w * gsMultiplier);
  }
}

double GriddataImpl::calculateBilskieAveragingFromLookup(Point &p, double w,
                                                         double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, this->m_raster.get()->dx(), r)) {
    return this->calculateAverageFromLookup(p, r * gsMultiplier);
  } else {
    return this->calculateNearestFromLookup(p, w * gsMultiplier);
  }
}

double GriddataImpl::calculateInverseDistanceWeighted(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
    return num > 0 ? n / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataImpl::calculateInverseDistanceWeightedNPoints(Point &p,
                                                             double n) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  int maxPoints = static_cast<size_t>(n);

  double w = this->calculateExpansionLevelForPoints(maxPoints);

  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
      if (np >= maxPoints) break;
    }
    return np > 0 ? val / d : this->defaultValue();
  }
  return this->defaultValue();
}

bool sortPointsByIncreasingDistance(Point &a, Point &b) {
  return a.first < b.first;
}

double GriddataImpl::calculateAverageNearestN(Point &p, double n) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;

  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    pts.reserve(z.size());
    for (size_t i = 0; i < z.size(); ++i) {
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

double GriddataImpl::calculateAverageNearestNFromLookup(Point &p, double n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  size_t maxPoints = static_cast<size_t>(n);
  double w = this->calculateExpansionLevelForPoints(maxPoints);
  std::vector<Point> pts;

  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    pts.reserve(z.size());
    for (size_t i = 0; i < z.size(); ++i) {
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

double GriddataImpl::calculateInverseDistanceWeightedFromLookup(Point &p,
                                                                double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
    return num > 0 ? n / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataImpl::calculateExpansionLevelForPoints(size_t n) {
  //...This tries to build out a box around a point at the
  // resolution of the raster. The hope is that by going 2
  // additional levels outside of what would be needed for the
  // requested number of points, we'll always hit the request
  // unless we're in a severe nodata region
  int levels = std::floor(n / 8.0) + 2;
  return this->m_raster.get()->dx() * static_cast<double>(levels);
}

Interpolation::Threshold GriddataImpl::thresholdMethod() const {
  return m_thresholdMethod;
}

void GriddataImpl::setThresholdMethod(
    const Interpolation::Threshold &thresholdMethod) {
  m_thresholdMethod = thresholdMethod;
}

double GriddataImpl::thresholdValue() const { return m_thresholdValue; }

void GriddataImpl::setThresholdValue(double filterValue) {
  m_thresholdValue = filterValue;
}

double GriddataImpl::calculateInverseDistanceWeightedNPointsFromLookup(
    Point &p, double n) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  int maxPoints = static_cast<size_t>(n);

  double w = calculateExpansionLevelForPoints(n);

  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
      if (np >= maxPoints) break;
    }
    return np > 0 ? val / d : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataImpl::calculateOutsideStandardDeviation(Point &p, double w,
                                                       int n) {
  std::vector<double> x, y, z, z2;
  std::vector<bool> v;
  z2.reserve(z.size());
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
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
    for (size_t i = 0; i < z2.size(); i++) {
      if (z[i] >= cutoff) {
        a += z[i];
        np++;
      }
    }
    return np > 0 ? a / np : this->calculateAverage(p, w);
  }
  return this->defaultValue();
}

double GriddataImpl::calculateOutsideStandardDeviationFromLookup(Point &p,
                                                                 double w,
                                                                 int n) {
  std::vector<double> x, y, z2;
  std::vector<int> z;
  std::vector<bool> v;
  z2.reserve(z.size());
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    for (size_t i = 0; i < z.size(); ++i) {
      if (v[i]) {
        double zl;
        if (this->getKeyValue(z[i], zl)) {
          z2.push_back(zl);
        }
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
      if (z[i] >= cutoff) {
        a += z[i];
        np++;
      }
    }
    return np > 0 ? a / np : this->calculateAverageFromLookup(p, w);
  }
  return this->defaultValue();
}

double GriddataImpl::calculateNearest(Point &p, double w) {
  Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w) {
    return this->defaultValue();
  } else {
    double z = this->m_raster.get()->pixelValueDouble(px);
    return z != this->m_raster.get()->nodata() ? z : this->defaultValue();
  }
}

double GriddataImpl::calculateNearestFromLookup(Point &p, double w) {
  Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p, pxloc);
  if (d > w)
    return this->defaultValue();
  else {
    int z = this->m_raster.get()->pixelValueInt(px);
    if (z != this->m_raster.get()->nodataint()) {
      double zl;
      return this->getKeyValue(z, zl) ? zl : this->defaultValue();
    } else {
      return this->defaultValue();
    }
  }
}

double GriddataImpl::calculateHighest(Point &p, double w) {
  std::vector<double> x, y, z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    double zm = std::numeric_limits<double>::min();
    for (size_t i = 0; i < x.size(); ++i) {
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

double GriddataImpl::calculateHighestFromLookup(Point &p, double w) {
  std::vector<double> x, y;
  std::vector<int> z;
  std::vector<bool> v;
  if (this->pixelDataInRadius(p, w, x, y, z, v)) {
    double zm = std::numeric_limits<double>::min();
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
    return zm != std::numeric_limits<double>::min() ? zm : this->defaultValue();
  }
  return this->defaultValue();
}

bool GriddataImpl::computeWindDirectionAndWeight(Point &p, double x, double y,
                                                 double &w, int &dir) {
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

void GriddataImpl::computeWeightedDirectionalWindValues(
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

std::vector<double> GriddataImpl::calculateDirectionalWindFromRaster(Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y, z, wind, weight;
  std::vector<bool> v;
  wind.resize(12);
  weight.resize(12);

  std::fill(weight.begin(), weight.end(), 0.0);
  std::fill(wind.begin(), wind.end(), 0.0);

  this->pixelDataInRadius(p, this->windRadius(), x, y, z, v);

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

std::vector<double> GriddataImpl::calculateDirectionalWindFromLookup(Point &p) {
  double nearWeight = 0.0;
  std::vector<double> x, y, wind, weight;
  std::vector<int> z;
  std::vector<bool> v;
  wind.resize(12);
  weight.resize(12);

  std::fill(weight.begin(), weight.end(), 0.0);
  std::fill(wind.begin(), wind.end(), 0.0);

  this->pixelDataInRadius(p, this->windRadius(), x, y, z, v);

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

void GriddataImpl::assignInterpolationFunctionPointer(bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculatePointPtr = &GriddataImpl::calculatePointFromLookup;
  } else {
    this->m_calculatePointPtr = &GriddataImpl::calculatePoint;
  }
}

double GriddataImpl::datumShift() const { return this->m_datumShift; }

void GriddataImpl::setDatumShift(double datumShift) {
  this->m_datumShift = datumShift;
}

void GriddataImpl::checkMatchingCoorindateSystems() {
  if (this->m_epsg != this->m_mesh->projection()) {
    adcircmodules_throw_exception(
        "You must use the same coordinate systems for mesh and raster.");
  }
}

void GriddataImpl::checkRasterOpen() {
  if (!this->m_raster.get()->isOpen()) {
    bool success = this->m_raster.get()->open();
    if (!success) {
      adcircmodules_throw_exception("Could not open the raster file.");
    }
  }
}

void GriddataImpl::assignDirectionalWindReductionFunctionPointer(
    bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculateDwindPtr =
        &GriddataImpl::calculateDirectionalWindFromLookup;
  } else {
    this->m_calculateDwindPtr =
        &GriddataImpl::calculateDirectionalWindFromRaster;
  }
}

std::vector<double> GriddataImpl::computeValuesFromRaster(bool useLookupTable) {
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->assignInterpolationFunctionPointer(useLookupTable);
  boost::progress_display *progress = nullptr;

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  std::vector<double> gridsize = this->m_mesh->computeMeshSize();
  std::vector<double> result;
  result.resize(this->m_mesh->numNodes());

  if (this->showProgressBar()) {
    progress = new boost::progress_display(this->m_mesh->numNodes());
  }

#pragma omp parallel for schedule(dynamic) shared(progress)
  for (signed long long i = 0;
       i < static_cast<signed long long>(this->m_mesh->numNodes()); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress);
    }

    Point p(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    Method m = static_cast<Method>(this->m_interpolationFlags[i]);
    double v = (this->*m_calculatePointPtr)(p, gridsize[i] * 0.5,
                                            this->m_filterSize[i], m);
    result[i] = v * this->m_rasterMultiplier + this->m_datumShift;
  }

  if (this->showProgressBar()) delete progress;

  return result;
}

void GriddataImpl::thresholdData(std::vector<double> &z, std::vector<bool> &v) {
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

std::vector<std::vector<double>> GriddataImpl::computeDirectionalWindReduction(
    bool useLookupTable) {
  boost::progress_display *progress = nullptr;
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->assignDirectionalWindReductionFunctionPointer(useLookupTable);

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  std::vector<std::vector<double>> result;
  result.resize(this->m_mesh->numNodes());

  if (this->showProgressBar()) {
    progress = new boost::progress_display(this->m_mesh->numNodes());
  }

#pragma omp parallel for schedule(dynamic) shared(progress)
  for (signed long long i = 0;
       i < static_cast<signed long long>(this->m_mesh->numNodes()); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress);
    }

    if (this->m_interpolationFlags[i] != NoThreshold) {
      Point p(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
      result[i] = (this->*m_calculateDwindPtr)(p);
      for (auto &r : result[i]) {
        r += this->m_datumShift;
      }
    } else {
      result[i] = std::vector<double>(12);
      std::fill(result[i].begin(), result[i].end(), this->defaultValue());
    }
  }

  if (this->showProgressBar()) delete progress;

  return result;
}
