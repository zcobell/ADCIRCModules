#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include "mesh.h"
#include "point.h"
#include "rasterdata.h"

class Griddata {
 public:
  enum Method { NoMethod = 0, Average = 1, Nearest = 2, Highest = 3 };

  Griddata();
  Griddata(Adcirc::Geometry::Mesh *mesh, std::string rasterFile);
  ~Griddata();

  std::string rasterFile() const;
  void setRasterFile(const std::string &rasterFile);

  std::vector<int> interpolationFlags() const;
  void setInterpolationFlags(const std::vector<int> &interpolationFlags);

  std::vector<double> filterSize() const;
  void setFilterSize(std::vector<double> filterSize);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  std::vector<double> computeValuesFromRaster();
  void computeValuesFromLookup(std::vector<double> &result);

  double windRadius() const;

  double windSigma() const;

  int epsg() const;
  void setEpsg(int epsg);

  bool showProgressBar() const;
  void setShowProgressBar(bool showProgressBar);

  double rasterMultiplier() const;
  void setRasterMultiplier(double rasterMultiplier);

private:
  void buildWindDirectionLookup();
  double calculatePoint(Point &p, double searchRadius, Griddata::Method method);
  double calculateAvearage(Point &p, double w);
  double calculateNearest(Point &p, double w);
  double calculateHighest(Point &p, double w);
  std::vector<double> directionalWind(Point &p, std::vector<double> &x,
                                      std::vector<double> &y,
                                      std::vector<double> &z);

  void computeGridScale();

  std::vector<double> m_filterSize;
  double m_defaultValue;
  Adcirc::Geometry::Mesh *m_mesh;
  Rasterdata m_raster;
  std::string m_rasterFile;
  std::vector<int> m_interpolationFlags;
  std::vector<double> m_gridsize;
  std::map<size_t, double> m_lookup;
  std::map<std::pair<int, int>, int> m_windDirections;
  int m_epsg;
  double m_rasterMultiplier;
  bool m_showProgressBar;

  const double m_windRadius = 10.0;
  const double m_windSigma = 6.0;
  const double m_windSigmaSquared = m_windSigma * m_windSigma;
};

#endif  // GRIDDATA_H
