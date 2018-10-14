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

#include "griddataimpl.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <numeric>
#include <utility>
#include "boost/progress.hpp"
#include "constants.h"
#include "elementtable.h"
#include "error.h"
#include "split.h"
#include "stringconversion.h"

using namespace Adcirc::Geometry;
using namespace Interpolation;

//...A couple constants used within
static const double c_oneOverWindSigmaSquared =
    1.0 / pow(GriddataImpl::windSigma(), 2.0);
static const double c_oneOver2MinusRoot3 = 1.0 / (2.0 - sqrt(3));
static const double c_oneOver2PlusRoot3 = 1.0 / (2.0 + sqrt(3));
static const double c_rootWindSigmaTwoPi =
    sqrt(2.0 * 4.0 * atan2(1.0, 1.0) * GriddataImpl::windSigma());
static const double c_epsilonSquared =
    pow(std::numeric_limits<double>::epsilon(), 2.0);

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

GriddataImpl::GriddataImpl() {
  this->m_raster.reset(nullptr);
  this->m_rasterFile = std::string();
  this->m_interpolationFlags = std::vector<int>();
  this->m_filterSize = std::vector<double>();
  this->m_defaultValue = -9999.0;
  this->m_epsg = 4326;
  this->m_datumShift = 0.0;
  this->m_showProgressBar = false;
  this->m_rasterMultiplier = 1.0;
  this->m_calculateDwindPtr = nullptr;
  this->m_calculatePointPtr = nullptr;
}
GriddataImpl::GriddataImpl(Mesh *mesh, std::string rasterFile) {
  this->m_mesh = mesh;
  this->m_rasterFile = rasterFile;
  this->m_raster.reset(new Rasterdata(this->m_rasterFile));
  this->m_interpolationFlags.resize(this->m_mesh->numNodes());
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), Average);
  this->m_filterSize.resize(this->m_mesh->numNodes());
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), 1.0);
  this->m_defaultValue = -9999;
  this->m_epsg = 4326;
  this->m_showProgressBar = false;
  this->m_rasterMultiplier = 1.0;
  this->m_datumShift = 0.0;
  this->m_calculateDwindPtr = nullptr;
  this->m_calculatePointPtr = nullptr;
}

GriddataImpl::~GriddataImpl() {}

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
    std::vector<std::string> ls = split::stringSplitToVector(l);
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
  this->m_raster.get()->searchBoxAroundPoint(p.x(), p.y(), radius, ul, lr);
  bool r = false;

  if (ul.isValid() && lr.isValid()) {
    this->m_raster.get()->pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), x, y, z);
    valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != this->m_raster.get()->nodata()) {
        if (Constants::distance(p.x(), p.y(), x[i], y[i]) <= radius) {
          valid[i] = true;
          r = true;
        }
      }
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
  this->m_raster.get()->searchBoxAroundPoint(p.x(), p.y(), radius, ul, lr);
  bool r = false;

  if (ul.isValid() && lr.isValid()) {
    this->m_raster.get()->pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), x, y, z);
    valid.resize(x.size());
    std::fill(valid.begin(), valid.end(), false);

    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != this->m_raster.get()->nodataint()) {
        if (Constants::distance(p.x(), p.y(), x[i], y[i]) <= radius) {
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
  double n = (0.25 * meshSize) / rasterCellSize;
  radius = pow(2 * n + 1.0, 2.0) * rasterCellSize;
  return n >= 1.0;
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
    return this->calculateNearest(p, r * gsMultiplier);
  }
}

double GriddataImpl::calculateBilskieAveragingFromLookup(Point &p, double w,
                                                         double gsMultiplier) {
  double r;
  if (this->calculateBilskieRadius(w, this->m_raster.get()->dx(), r)) {
    return this->calculateAverageFromLookup(p, r * gsMultiplier);
  } else {
    return this->calculateNearestFromLookup(p, r * gsMultiplier);
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
        double dis = Constants::distance(p.x(), p.y(), x[i], y[i]);
        n += z[i] / dis;
        d += 1.0 / dis;
        n++;
      }
    }
    return num > 0 ? n / d : this->defaultValue();
  } else {
    return this->defaultValue();
  }
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
          double dis = Constants::distance(p.x(), p.y(), x[i], y[i]);
          n += zl / dis;
          d += 1.0 / dis;
          n++;
        }
      }
    }
    return num > 0 ? n / d : this->defaultValue();
  } else {
    return this->defaultValue();
  }
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
    size_t n = 0;
    for (size_t i = 0; i < z2.size(); i++) {
      if (z[i] >= cutoff) {
        a += z[i];
        n++;
      }
    }
    return n > 0 ? a / n : this->defaultValue();
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
    size_t n = 0;
    for (size_t i = 0; i < z2.size(); i++) {
      if (z[i] >= cutoff) {
        a += z[i];
        n++;
      }
    }
    return n > 0 ? a / n : this->defaultValue();
  }
  return this->defaultValue();
}

double GriddataImpl::calculateNearest(Point &p, double w) {
  Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
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
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
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
  double dx = (x - p.x()) * 0.001;
  double dy = (y - p.y()) * 0.001;
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
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress);
    }

    Point p = Point(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    Method m = static_cast<Method>(this->m_interpolationFlags[i]);
    double v = (this->*m_calculatePointPtr)(p, gridsize[i] * 0.5,
                                            this->m_filterSize[i], m);
    result[i] = v * this->m_rasterMultiplier + this->m_datumShift;
  }

  if (this->showProgressBar()) delete progress;

  return result;
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
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++(*progress);
    }

    if (this->m_interpolationFlags[i] != NoMethod) {
      Point p = Point(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
      result[i] = (this->*m_calculateDwindPtr)(p);
      for (auto &r : result[i]) {
        r += this->m_datumShift;
      }
    }
  }

  if (this->showProgressBar()) delete progress;

  return result;
}
