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

#include "griddata.h"
#include <algorithm>
#include <fstream>
#include <numeric>
#include <utility>
#include "boost/progress.hpp"
#include "constants.h"
#include "elementtable.h"
#include "error.h"
#include "split.h"
#include "stringconversion.h"

using namespace std;
using namespace Adcirc::Geometry;

//...A couple constants used within
static const double oOwss = 1.0 / Griddata::windSigmaSquared();
static const double oO2mr3 = 1.0 / (2.0 - Constants::root3());
static const double oO2pr3 = 1.0 / (2.0 + Constants::root3());
static const double ws2pi = Griddata::windSigma2pi();
static const double epsilonSquared =
    pow(std::numeric_limits<double>::epsilon(), 2.0);

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

bool Griddata::getKeyValue(size_t key, double &value) {
  auto t = this->m_lookup.find(key);
  if (t == this->m_lookup.end()) return false;
  value = t->second;
  return true;
}

Griddata::Griddata() {
  this->m_raster.reset(nullptr);
  this->m_rasterFile = string();
  this->m_interpolationFlags = vector<int>();
  this->m_filterSize = vector<double>();
  this->m_defaultValue = -9999.0;
  this->m_epsg = 4326;
  this->m_showProgressBar = false;
  this->m_rasterMultiplier = 1.0;
}

Griddata::Griddata(Mesh *mesh, string rasterFile) {
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
}

Griddata::~Griddata() { this->m_raster.reset(); }

std::string Griddata::rasterFile() const { return this->m_rasterFile; }

void Griddata::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

double Griddata::rasterMultiplier() const { return this->m_rasterMultiplier; }

void Griddata::setRasterMultiplier(double rasterMultiplier) {
  this->m_rasterMultiplier = rasterMultiplier;
}

bool Griddata::showProgressBar() const { return this->m_showProgressBar; }

void Griddata::setShowProgressBar(bool showProgressBar) {
  this->m_showProgressBar = showProgressBar;
}

int Griddata::epsg() const { return this->m_epsg; }

void Griddata::setEpsg(int epsg) { this->m_epsg = epsg; }

void Griddata::readLookupTable(string lookupTableFile) {
  this->m_lookup.clear();
  std::fstream fid(lookupTableFile);

  string l;
  while (std::getline(fid, l)) {
    l = StringConversion::sanitizeString(l);
    if (l.length() == 0) {
      break;
    }
    vector<string> ls = split::stringSplitToVector(l);
    if (ls.size() < 2) {
      fid.close();
      Adcirc::Error::throwError("Could not read the lookup table.");
    }
    bool ok;
    size_t cls = StringConversion::stringToSizet(ls[0], ok);
    double v = StringConversion::stringToDouble(ls[1], ok);
    this->m_lookup[cls] = v;
  }
  fid.close();
  return;
}

vector<int> Griddata::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void Griddata::setInterpolationFlags(const vector<int> &interpolationFlags) {
  this->m_interpolationFlags = interpolationFlags;
}

void Griddata::setInterpolationFlag(size_t index, int flag) {
  if (index < this->m_interpolationFlags.size()) {
    this->m_interpolationFlags[index] = static_cast<Method>(flag);
  }
  return;
}

int Griddata::interpolationFlag(size_t index) {
  return index < this->m_interpolationFlags.size()
             ? this->m_interpolationFlags[index]
             : Method::NoMethod;
}

vector<double> Griddata::filterSizes() const { return this->m_filterSize; }

void Griddata::setFilterSizes(vector<double> filterSize) {
  this->m_filterSize = filterSize;
}

double Griddata::filterSize(size_t index) {
  return index < this->m_filterSize.size() ? this->m_filterSize[index] : 0.0;
}

void Griddata::setFilterSize(size_t index, double filterSize) {
  if (index < this->m_filterSize.size()) {
    this->m_filterSize[index] = filterSize;
  }
  return;
}

double Griddata::defaultValue() const { return this->m_defaultValue; }

void Griddata::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

void Griddata::buildWindDirectionLookup() {
  this->m_windDirections.resize(3);
  this->m_windDirections[0].resize(7);
  this->m_windDirections[1].resize(7);
  this->m_windDirections[2].resize(7);
  this->m_windDirections[1 + 1][0 + 3] = 7;
  this->m_windDirections[1 + 1][1 + 3] = 8;
  this->m_windDirections[1 + 1][2 + 3] = 9;
  this->m_windDirections[1 + 1][3 + 3] = 10;
  this->m_windDirections[0 + 1][3 + 3] = 10;
  this->m_windDirections[-1 + 1][3 + 3] = 10;
  this->m_windDirections[-1 + 1][2 + 3] = 11;
  this->m_windDirections[-1 + 1][1 + 3] = 12;
  this->m_windDirections[-1 + 1][0 + 3] = 1;
  this->m_windDirections[-1 + 1][-1 + 3] = 2;
  this->m_windDirections[-1 + 1][-2 + 3] = 3;
  this->m_windDirections[-1 + 1][-3 + 3] = 4;
  this->m_windDirections[0 + 1][-3 + 3] = 4;
  this->m_windDirections[1 + 1][-3 + 3] = 4;
  this->m_windDirections[1 + 1][-2 + 3] = 5;
  this->m_windDirections[1 + 1][-1 + 3] = 6;
  this->m_windDirections[0 + 1][2 + 3] = 0;
  this->m_windDirections[0 + 1][1 + 3] = 0;
  this->m_windDirections[0 + 1][0 + 3] = 0;
  this->m_windDirections[0 + 1][-1 + 3] = 0;
  this->m_windDirections[0 + 1][-2 + 3] = 0;
  return;
}

int Griddata::windDirection(int i, int j) {
  return this->m_windDirections[i + 1][j + 3];
}

bool Griddata::rasterInMemory() const { return this->m_rasterInMemory; }

void Griddata::setRasterInMemory(bool rasterInMemory) {
  this->m_rasterInMemory = rasterInMemory;
}

std::unordered_map<size_t, double> Griddata::lookup() const {
  return this->m_lookup;
}

bool Griddata::pixelDataInRadius(Point &p, double radius, vector<double> &x,
                                 vector<double> &y, vector<double> &z,
                                 vector<bool> &valid) {
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

bool Griddata::pixelDataInRadius(Point &p, double radius, vector<double> &x,
                                 vector<double> &y, vector<int> &z,
                                 vector<bool> &valid) {
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

double Griddata::calculatePoint(Point &p, double searchRadius,
                                Griddata::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverage(p, searchRadius);
      break;
    case Nearest:
      return this->calculateNearest(p, searchRadius);
      break;
    case Highest:
      return this->calculateHighest(p, searchRadius);
      break;
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviation(p, searchRadius, 2);
      break;
    default:
      return this->defaultValue();
      break;
  }
}

double Griddata::calculatePointFromLookup(Point &p, double searchRadius,
                                          Griddata::Method method) {
  switch (method) {
    case Average:
      return this->calculateAverageFromLookup(p, searchRadius);
      break;
    case Nearest:
      return this->calculateNearestFromLookup(p, searchRadius);
      break;
    case Highest:
      return this->calculateHighestFromLookup(p, searchRadius);
      break;
    case PlusTwoSigma:
      return this->calculateOutsideStandardDeviationFromLookup(p, searchRadius,
                                                               2);
      break;
    default:
      return this->defaultValue();
      break;
  }
}

double Griddata::calculateAverage(Point &p, double w) {
  vector<double> x, y, z;
  vector<bool> v;
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

double Griddata::calculateAverageFromLookup(Point &p, double w) {
  vector<double> x, y;
  vector<int> z;
  vector<bool> v;
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

double Griddata::calculateOutsideStandardDeviation(Point &p, double w, int n) {
  vector<double> x, y, z, z2;
  vector<bool> v;
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

double Griddata::calculateOutsideStandardDeviationFromLookup(Point &p, double w,
                                                             int n) {
  vector<double> x, y, z2;
  vector<int> z;
  vector<bool> v;
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

double Griddata::calculateNearest(Point &p, double w) {
  Pixel px = this->m_raster.get()->coordinateToPixel(p);
  Point pxloc = this->m_raster.get()->pixelToCoordinate(px);
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
  if (d > w)
    return this->defaultValue();
  else {
    double z = this->m_raster.get()->pixelValueDouble(px);
    return z != this->m_raster.get()->nodata() ? z : this->defaultValue();
  }
}

double Griddata::calculateNearestFromLookup(Point &p, double w) {
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

double Griddata::calculateHighest(Point &p, double w) {
  vector<double> x, y, z;
  vector<bool> v;
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

double Griddata::calculateHighestFromLookup(Point &p, double w) {
  vector<double> x, y;
  vector<int> z;
  vector<bool> v;
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

vector<double> Griddata::computeGridScale() {
  ElementTable e(this->m_mesh);
  e.build();

  vector<double> gridsize;
  gridsize.resize(this->m_mesh->numNodes());

  for (size_t i = 0; i < this->m_mesh->numNodes(); i++) {
    vector<Element *> l = e.elementList(this->m_mesh->node(i));
    double a = 0.0;
    for (size_t j = 0; j < l.size(); ++j) {
      a += l[j]->elementSize(false);
    }
    if (l.size() > 0)
      gridsize[i] = a / l.size();
    else
      gridsize[i] = 0.0;

    if (gridsize[i] < 0.0) {
      Adcirc::Error::throwError("Error computing mesh size table.");
    }
  }
  return gridsize;
}

bool Griddata::computeWindDirectionAndWeight(Point &p, double x, double y,
                                             double &w, int &dir) {
  double dx = (x - p.x()) * 0.001;
  double dy = (y - p.y()) * 0.001;
  double d = dx * dx + dy * dy;

  w = 1.0 / (std::exp(0.5 * d * oOwss) + ws2pi);
  if (d > epsilonSquared) {
    double tanxy = 10000000.0;
    if (std::abs(dx) > std::numeric_limits<double>::epsilon()) {
      tanxy = std::abs(dy / dx);
    }

    int64_t k = min<int64_t>(1.0, tanxy * oO2mr3) +
                min<int64_t>(1.0, tanxy) +
                min<int64_t>(1.0, tanxy * oO2pr3);

    int a = static_cast<int>(sgn(dx));
    int b = static_cast<int>(k * sgn(dy));
    dir = this->windDirection(a, b) - 1;
    return true;
  }
  return false;
}

void Griddata::computeWeightedDirectionalWindValues(vector<double> &weight,
                                                    vector<double> &wind,
                                                    double nearWeight) {
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

vector<double> Griddata::calculateDirectionalWindFromRaster(Point &p) {
  double nearWeight = 0.0;
  vector<double> x, y, z, wind, weight;
  vector<bool> v;
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

vector<double> Griddata::calculateDirectionalWindFromLookup(Point &p) {
  double nearWeight = 0.0;
  vector<double> x, y, wind, weight;
  vector<int> z;
  vector<bool> v;
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

void Griddata::assignInterpolationFunctionPointer(bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculatePointPtr = &Griddata::calculatePointFromLookup;
  } else {
    this->m_calculatePointPtr = &Griddata::calculatePoint;
  }
}

void Griddata::checkMatchingCoorindateSystems() {
  if (this->m_epsg != this->m_mesh->projection()) {
    Adcirc::Error::throwError(
        "You must use the same coordinate systems for mesh and raster.");
  }
}

void Griddata::checkRasterOpen() {
  if (!this->m_raster.get()->isOpen()) {
    bool success = this->m_raster.get()->open();
    if (!success) {
      Adcirc::Error::throwError("Could not open the raster file.");
    }
  }
}

void Griddata::assignDirectionalWindReductionFunctionPointer(
    bool useLookupTable) {
  if (useLookupTable) {
    this->m_calculateDwindPtr = &Griddata::calculateDirectionalWindFromLookup;
  } else {
    this->m_calculateDwindPtr = &Griddata::calculateDirectionalWindFromRaster;
  }
}

vector<double> Griddata::computeValuesFromRaster(bool useLookupTable) {
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->assignInterpolationFunctionPointer(useLookupTable);

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  vector<double> gridsize = this->computeGridScale();
  vector<double> result;
  result.resize(this->m_mesh->numNodes());

  boost::progress_display progress(this->m_mesh->numNodes());

#pragma omp parallel for schedule(dynamic) shared(progress)
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++progress;
    }

    Point p = Point(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    Method m = static_cast<Method>(this->m_interpolationFlags[i]);
    double s = (double)this->m_filterSize[i] * gridsize[i] * 0.5;
    double v = (this->*m_calculatePointPtr)(p, s, m);
    result[i] = v * this->m_rasterMultiplier;
  }

  return result;
}

vector<vector<double> > Griddata::computeDirectionalWindReduction(
    bool useLookupTable) {
  this->checkRasterOpen();
  this->checkMatchingCoorindateSystems();
  this->buildWindDirectionLookup();
  this->assignDirectionalWindReductionFunctionPointer(useLookupTable);

  if (this->m_rasterInMemory) {
    this->m_raster.get()->read();
  }

  vector<vector<double> > result;
  result.resize(this->m_mesh->numNodes());

  boost::progress_display progress(this->m_mesh->numNodes());

#pragma omp parallel for schedule(dynamic) shared(progress)
  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
#pragma omp critical
      ++progress;
    }

    Point p = Point(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    result[i] = (this->*m_calculateDwindPtr)(p);
  }

  return result;
}
