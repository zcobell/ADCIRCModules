#include "griddata.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include "constants.h"
#include "elementtable.h"
#include "error.h"

template <typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

using namespace std;
using namespace Adcirc::Geometry;

Griddata::Griddata() {
  this->m_raster = Rasterdata();
  this->m_rasterFile = string();
  this->m_interpolationFlags = std::vector<int>();
  this->m_filterSize = std::vector<double>();
  this->m_defaultValue = -9999.0;
  this->buildWindDirectionLookup();
}

Griddata::Griddata(Mesh &mesh, string rasterFile) {
  this->m_mesh = mesh;
  this->m_rasterFile = rasterFile;
  this->m_raster = Rasterdata(this->m_rasterFile);
  this->m_interpolationFlags.resize(this->m_mesh.numNodes());
  std::fill(this->m_interpolationFlags.begin(),
            this->m_interpolationFlags.end(), Average);
  this->m_filterSize.resize(this->m_mesh.numNodes());
  std::fill(this->m_filterSize.begin(), this->m_filterSize.end(), 1.0);
  this->computeGridScale();
  this->m_defaultValue = -9999;
  this->buildWindDirectionLookup();
}

Griddata::~Griddata() {}

std::string Griddata::rasterFile() const { return this->m_rasterFile; }

double Griddata::windSigma() const { return this->m_windSigma; }

double Griddata::windRadius() const { return this->m_windRadius; }

void Griddata::setRasterFile(const std::string &rasterFile) {
  this->m_rasterFile = rasterFile;
}

std::vector<int> Griddata::interpolationFlags() const {
  return this->m_interpolationFlags;
}

void Griddata::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
  this->m_interpolationFlags = interpolationFlags;
}

std::vector<double> Griddata::filterSize() const { return this->m_filterSize; }

void Griddata::setFilterSize(std::vector<double> filterSize) {
  this->m_filterSize = filterSize;
}

double Griddata::defaultValue() const { return this->m_defaultValue; }

void Griddata::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

void Griddata::buildWindDirectionLookup() {
  this->m_windDirections[make_pair<int, int>(1, 0)] = 7;
  this->m_windDirections[make_pair<int, int>(1, 1)] = 8;
  this->m_windDirections[make_pair<int, int>(1, 2)] = 9;
  this->m_windDirections[make_pair<int, int>(1, 3)] = 10;
  this->m_windDirections[make_pair<int, int>(0, 3)] = 10;
  this->m_windDirections[make_pair<int, int>(-1, 3)] = 10;
  this->m_windDirections[make_pair<int, int>(-1, 2)] = 11;
  this->m_windDirections[make_pair<int, int>(-1, 1)] = 12;
  this->m_windDirections[make_pair<int, int>(-1, 0)] = 1;
  this->m_windDirections[make_pair<int, int>(-1, -1)] = 2;
  this->m_windDirections[make_pair<int, int>(-1, -2)] = 3;
  this->m_windDirections[make_pair<int, int>(-1, -3)] = 4;
  this->m_windDirections[make_pair<int, int>(0, -3)] = 4;
  this->m_windDirections[make_pair<int, int>(1, -3)] = 4;
  this->m_windDirections[make_pair<int, int>(1, -2)] = 5;
  this->m_windDirections[make_pair<int, int>(1, -1)] = 6;
  this->m_windDirections[make_pair<int, int>(0, 2)] = 0;
  this->m_windDirections[make_pair<int, int>(0, 1)] = 0;
  this->m_windDirections[make_pair<int, int>(0, 0)] = 0;
  this->m_windDirections[make_pair<int, int>(0, -1)] = 0;
  this->m_windDirections[make_pair<int, int>(0, -2)] = 0;
  return;
}

double Griddata::calculatePoint(Point &p, double searchRadius,
                                Griddata::Method method) {
  switch (method) {
    case Average:
      return this->calculateAvearage(p, searchRadius);
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

double Griddata::calculateAvearage(Point &p, double w) {
  size_t ibegin, iend, jbegin, jend;
  vector<double> x, y, z;
  double a = 0.0;
  size_t n = 0;

  this->m_raster.searchBoxAroundPoint(p.x(), p.y(), w, ibegin, jbegin, iend,
                                      jend);
  this->m_raster.pixelValues(ibegin, jbegin, iend, jend, x, y, z);
  for (size_t i = 0; i < x.size(); ++i) {
    double d = Constants::distance(p.x(), p.y(), x[i], y[i]);
    if (d < w) {
      a += z[i];
      n++;
    }
  }
  return a / n;
}

double Griddata::calculateNearest(Point &p, double w) {
  Pixel px = this->m_raster.coordinateToPixel(p);
  Point pxloc = this->m_raster.pixelToCoordinate(px);
  double d = Constants::distance(p.x(), p.y(), pxloc.x(), pxloc.y());
  if (d > w)
    return this->defaultValue();
  else
    return this->m_raster.pixelValueDouble(px);
}

double Griddata::calculateHighest(Point &p, double w) {
  size_t ibegin, iend, jbegin, jend;
  vector<double> x, y, z;
  double zm = std::numeric_limits<double>::min();

  this->m_raster.searchBoxAroundPoint(p.x(), p.y(), w, ibegin, jbegin, iend,
                                      jend);
  this->m_raster.pixelValues(ibegin, jbegin, iend, jend, x, y, z);
  for (size_t i = 0; i < x.size(); ++i) {
    if (z[i] > zm) {
      double d = Constants::distance(p.x(), p.y(), x[i], y[i]);
      if (d < w) {
        zm = z[i];
      }
    }
  }
  if (zm == std::numeric_limits<double>::min())
    return this->defaultValue();
  else
    return zm;
}

void Griddata::computeGridScale() {
  ElementTable e(&this->m_mesh);
  e.build();

  this->m_gridsize.resize(this->m_mesh.numNodes());

  for (size_t i = 0; i < this->m_mesh.numNodes(); ++i) {
    vector<Element *> l = e.elementList(i);
    double a = 0.0;
    size_t n = 0;
    for (size_t j = 0; j < l.size(); ++j) {
      a += l[j]->elementSize(false);
      n++;
    }
    this->m_gridsize[i] = a / n;
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
    double dx = p.x() - x[i];
    double dy = p.y() - y[i];
    double d = dx * dx + dy * dy;
    if (sqrt(d) > std::numeric_limits<double>::epsilon()) {
      double w =
          1.0 / (exp(0.5 * d / (this->m_windSigmaSquared)) *
                 pow(((2.0 * Constants::pi() * this->m_windSigma)), 2.0));
      tw += w;
      tv += w * z[i];

      double tanxy;
      if (abs(dx) < std::numeric_limits<double>::epsilon()) {
        tanxy = std::numeric_limits<double>::max();
      } else {
        tanxy = abs(dy / dx);
      }

      int k = min<int>(1, tanxy / (2 - Constants::root3()));
      k = k + min<int>(1, tanxy);
      k = k + min<int>(1, tanxy / (2 * Constants::root3()));

      if (k > 3) {
        Adcirc::Error::throwError("Overflow in k");
      }

      int a = static_cast<int>(std::round(sgn(dx)));
      int b = static_cast<int>(std::round(k * sgn(y)));
      std::pair<int, int> pr = {a, b};
      int dir = this->m_windDirections[pr] - 1;
      r[dir] += w * z[i];
      wt[dir] += w;
    }
  }

  for (int i = 0; i < 12; i++) {
    double w = tw + wt[i];
    if (w > 1e-12) {
      r[i] = r[i] / w;
    }
  }

  return r;
}

void Griddata::computeValuesFromRaster(std::vector<double> &result) {
  if (!this->m_raster.isOpen()) {
    bool success = this->m_raster.open();
    if (!success) {
      Adcirc::Error::throwError("Could not open the raster file.");
    }
  }

  if (this->m_raster.epsg() == 4326 || this->m_raster.epsg() == 4269) {
    Adcirc::Error::warning("Recommend using planar based coordinate system.");
  }

  if (this->m_mesh.projection() != this->m_raster.epsg())
    this->m_mesh.reproject(this->m_raster.epsg());

  if (this->m_gridsize.size() != this->m_mesh.numNodes())
    this->computeGridScale();

  for (size_t i = 0; i < this->m_mesh.numNodes(); ++i) {
    Point p = Point(this->m_mesh.node(i)->x(), this->m_mesh.node(i)->y());
    result[i] = this->calculatePoint(
        p, this->m_filterSize[i] * this->m_gridsize[i],
        static_cast<Method>(this->m_interpolationFlags[i]));
  }
}

void Griddata::computeValuesFromLookup(std::vector<double> &result) { return; }
