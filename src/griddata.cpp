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
#include <cmath>
#include <fstream>
#include <numeric>
#include <utility>
#include "constants.h"
#include "elementtable.h"
#include "error.h"
#include "progressbar/progressbar.h"
#include "split.h"
#include "stringconversion.h"

using namespace std;
using namespace Adcirc::Geometry;

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

bool Griddata::hasKey(int key) {
  if (this->m_lookup.find(key) != this->m_lookup.end()) return true;
  return false;
}

Griddata::Griddata() {
  this->m_raster = Rasterdata();
  this->m_rasterFile = string();
  this->m_interpolationFlags = std::vector<int>();
  this->m_filterSize = std::vector<double>();
  this->m_defaultValue = -9999.0;
  this->m_epsg = 4326;
  this->m_showProgressBar = false;
  this->m_rasterMultiplier = 1.0;
}

Griddata::Griddata(Mesh *mesh, string rasterFile) {
  this->m_mesh = mesh;
  this->m_rasterFile = rasterFile;
  this->m_raster = Rasterdata(this->m_rasterFile);
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

Griddata::~Griddata() {}

std::string Griddata::rasterFile() const { return this->m_rasterFile; }

double Griddata::windSigma() const { return this->m_windSigma; }

double Griddata::windRadius() const { return this->m_windRadius; }

void Griddata::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

double Griddata::rasterMultiplier() const { return this->m_rasterMultiplier; }

void Griddata::setRasterMultiplier(double rasterMultiplier) {
  m_rasterMultiplier = rasterMultiplier;
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

std::vector<int> Griddata::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void Griddata::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
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

std::vector<double> Griddata::filterSizes() const { return this->m_filterSize; }

void Griddata::setFilterSizes(std::vector<double> filterSize) {
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
  this->m_windDirections.resize(2);
  this->m_windDirections[0].resize(6);
  this->m_windDirections[1].resize(6);
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

std::unordered_map<size_t, double> Griddata::lookup() const { return m_lookup; }

bool Griddata::pixelDataInRadius(Point &p, double radius, vector<double> &x,
                                 vector<double> &y, vector<double> &z) {
  Pixel ul, lr;
  this->m_raster.searchBoxAroundPoint(p.x(), p.y(), radius, ul, lr);

  if (ul.isValid() && lr.isValid()) {
    std::vector<double> xt, yt, zt;
    this->m_raster.pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), xt, yt, zt);

    x.reserve(xt.size());
    y.reserve(yt.size());
    z.reserve(yt.size());

    for (size_t i = 0; i < xt.size(); ++i) {
      if (zt[i] != this->m_raster.nodata()) {
        if (Constants::distance(p.x(), p.y(), xt[i], yt[i]) <= radius) {
          x.push_back(xt[i]);
          y.push_back(yt[i]);
          z.push_back(zt[i]);
        }
      }
    }
    return x.size() > 0;
  }
  return false;
}

bool Griddata::pixelDataInRadius(Point &p, double radius, vector<double> &x,
                                 vector<double> &y, vector<int> &z) {
  Pixel ul, lr;
  this->m_raster.searchBoxAroundPoint(p.x(), p.y(), radius, ul, lr);

  if (ul.isValid() && lr.isValid()) {
    std::vector<double> xt, yt;
    std::vector<int> zt;
    this->m_raster.pixelValues(ul.i(), ul.j(), lr.i(), lr.j(), xt, yt, zt);

    x.reserve(xt.size());
    y.reserve(yt.size());
    z.reserve(yt.size());

    for (size_t i = 0; i < xt.size(); ++i) {
      if (zt[i] != this->m_raster.nodataint()) {
        if (Constants::distance(p.x(), p.y(), xt[i], yt[i]) <= radius) {
          x.push_back(xt[i]);
          y.push_back(yt[i]);
          z.push_back(zt[i]);
        }
      }
    }
    return x.size() > 0;
  }
  return false;
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
    default:
      return this->defaultValue();
      break;
  }
}

double Griddata::calculateAverage(Point &p, double w) {
  vector<double> x, y, z;
  if (this->pixelDataInRadius(p, w, x, y, z)) {
    return (z.size() > 0 ? std::accumulate(z.begin(), z.end(), 0.0) / z.size()
                         : this->defaultValue());
  } else {
    return this->defaultValue();
  }
}

double Griddata::calculateAverageFromLookup(Point &p, double w) {
  vector<double> x, y;
  vector<int> z;
  if (this->pixelDataInRadius(p, w, x, y, z)) {
    size_t n = 0;
    double a = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
      if (this->hasKey(z[i])) {
        a += this->m_lookup[z[i]];
        n++;
      }
    }
    return (n > 0 ? a / n : this->defaultValue());
  } else {
    return this->defaultValue();
  }
}

double Griddata::calculateNearest(Point &p, double w) {
  Pixel px = this->m_raster.coordinateToPixel(p);
  Point pxloc = this->m_raster.pixelToCoordinate(px);
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
  if (d > w)
    return this->defaultValue();
  else {
    double z = this->m_raster.pixelValueDouble(px);
    return z != this->m_raster.nodata() ? z : this->defaultValue();
  }
}

double Griddata::calculateNearestFromLookup(Point &p, double w) {
  Pixel px = this->m_raster.coordinateToPixel(p);
  Point pxloc = this->m_raster.pixelToCoordinate(px);
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
  if (d > w)
    return this->defaultValue();
  else {
    int z = this->m_raster.pixelValueInt(px);
    if (z != this->m_raster.nodataint()) {
      return this->hasKey(z) ? this->m_lookup[z] : this->defaultValue();
    } else {
      return this->defaultValue();
    }
  }
}

double Griddata::calculateHighest(Point &p, double w) {
  vector<double> x, y, z;
  if (this->pixelDataInRadius(p, w, x, y, z)) {
    double zm = std::numeric_limits<double>::min();
    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] > zm) {
        zm = z[i];
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
  if (this->pixelDataInRadius(p, w, x, y, z)) {
    double zm = std::numeric_limits<double>::min();
    for (size_t i = 0; i < x.size(); ++i) {
      if (z[i] != this->m_raster.nodataint()) {
        if (this->hasKey(z[i])) {
          double zv = this->m_lookup[z[i]];
          if (zv > zm) {
            zm = zv;
          }
        }
      }
    }
    return zm != std::numeric_limits<double>::min() ? zm : this->defaultValue();
  }
  return this->defaultValue();
}

std::vector<double> Griddata::computeGridScale() {
  ElementTable e(this->m_mesh);
  e.build();

  std::vector<double> gridsize;
  gridsize.resize(this->m_mesh->numNodes());

  for (size_t i = 0; i < this->m_mesh->numNodes(); i++) {
    vector<Element *> l = e.elementList(this->m_mesh->node(i));
    double a = 0.0;
    for (size_t j = 0; j < l.size(); ++j) {
      a += l[j]->elementSize(false);
    }
    gridsize[i] = a / l.size();
    if (gridsize[i] <= 0.0) {
      Adcirc::Error::throwError("Error computing mesh size table.");
    }
  }
  return gridsize;
}

bool Griddata::computeWindDirectionAndWeight(Point &p, double x, double y,
                                             double &w, int &dir) {
  double dx = p.x() - x;
  double dy = p.y() - y;
  double d = dx * dx + dy * dy;
  if (sqrt(d) > std::numeric_limits<double>::epsilon()) {
    w = 1.0 / (exp(0.5 * d / (this->m_windSigmaSquared)) *
               pow(((Constants::twoPi() * this->m_windSigma)), 2.0));

    double tanxy;
    if (abs(dx) < std::numeric_limits<double>::epsilon()) {
      tanxy = std::numeric_limits<double>::max();
    } else {
      tanxy = abs(dy / dx);
    }

    int k = min<int>(
        1, static_cast<int>(std::floor(tanxy / (2 - Constants::root3()))));
    k = k + min<int>(1, static_cast<int>(std::floor(tanxy)));
    k = k + min<int>(1, static_cast<int>(
                            std::floor(tanxy / (2 + Constants::root3()))));

    if (k > 3) {
      Adcirc::Error::throwError("Overflow in k");
    }

    int a = static_cast<int>(std::round(sgn(dx)));
    int b = static_cast<int>(std::round(k * sgn(dy)));
    dir = this->windDirection(a, b) - 1;
    return true;
  }
  return false;
}

void Griddata::computeWeightedDirectionalWindValues(double tw,
                                                    vector<double> &wt,
                                                    vector<double> &r) {
  for (int i = 0; i < 12; ++i) {
    double w = tw + wt[i];
    if (w > 1e-12) {
      r[i] = r[i] / w;
    }
  }
  return;
}

vector<double> Griddata::directionalWind(Point &p, vector<double> &x,
                                         vector<double> &y, vector<double> &z) {
  double tw = 0.0;
  double tv = 0.0;
  vector<double> r, wt;
  r.resize(12);
  wt.resize(12);

  for (size_t i = 0; i < x.size(); ++i) {
    if (z[i] != this->m_raster.nodata()) {
      double w;
      int dir;
      if (this->computeWindDirectionAndWeight(p, x[i], y[i], w, dir)) {
        tw += w;
        wt[dir] += w;
        r[dir] += w * z[i];
        tv += w * z[i];
      }
    }
  }

  this->computeWeightedDirectionalWindValues(tw, wt, r);

  return r;
}

vector<double> Griddata::directionalWind(Point &p, vector<double> &x,
                                         vector<double> &y, vector<int> &z) {
  double tw = 0.0;
  double tv = 0.0;
  vector<double> r, wt;
  r.resize(12);
  wt.resize(12);

  for (size_t i = 0; i < x.size(); ++i) {
    if (z[i] != this->m_raster.nodata()) {
      if (this->hasKey(z[i])) {
        double zl = this->m_lookup[z[i]];
        double w;
        int dir;
        if (this->computeWindDirectionAndWeight(p, x[i], y[i], w, dir)) {
          tw += w;
          wt[dir] += w;
          r[dir] += w * zl;
          tv += w * zl;
        }
      } else {
        Adcirc::Error::throwError("Key not found.");
      }
    }
  }

  this->computeWeightedDirectionalWindValues(tw, wt, r);

  return r;
}

std::vector<double> Griddata::computeValuesFromRaster(bool useLookupTable) {
  if (!this->m_raster.isOpen()) {
    bool success = this->m_raster.open();
    if (!success) {
      Adcirc::Error::throwError("Could not open the raster file.");
    }
  }

  if (this->m_epsg != this->m_mesh->projection()) {
    Adcirc::Error::throwError(
        "You must use the same coordinate systems for mesh and raster.");
  }

  std::vector<double> gridsize = this->computeGridScale();
  std::vector<double> result;
  result.resize(this->m_mesh->numNodes());

  if (useLookupTable) {
    this->m_calculatePointPtr = &Griddata::calculatePointFromLookup;
  } else {
    this->m_calculatePointPtr = &Griddata::calculatePoint;
  }

  progressbar *progress;
  if (this->m_showProgressBar)
    progress = progressbar_new("Raster Interpolation", 1000);

  for (size_t i = 0; i < this->m_mesh->numNodes(); ++i) {
    if (this->m_showProgressBar) {
      if (i % (this->m_mesh->numNodes() / 1000) == 0) progressbar_inc(progress);
    }

    Point p = Point(this->m_mesh->node(i)->x(), this->m_mesh->node(i)->y());
    double v;
    double s = (double)this->m_filterSize[i] * gridsize[i] / 2.0;
    Method m = static_cast<Method>(this->m_interpolationFlags[i]);
    v = (this->*m_calculatePointPtr)(p, s, m);
    result[i] = v * this->m_rasterMultiplier;
  }

  if (this->m_showProgressBar) progressbar_finish(progress);

  return result;
}

void Griddata::sizeDirectionalWindMatrix(
    std::vector<std::vector<double> > &result) {
  result.resize(12);
  for (auto &sr : result) {
    sr.resize(this->m_mesh->numNodes());
  }
}

std::vector<std::vector<double> > Griddata::computeDirectionalWindReduction(
    bool useLookupTable) {
  std::vector<std::vector<double> > result;
  this->sizeDirectionalWindMatrix(result);

  return result;
}
