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
#ifndef GRIDDATA_H
#define GRIDDATA_H

#include <memory>
#include <string>
#include <unordered_map>
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

  void readLookupTable(std::string lookupTableFile);

  std::vector<int> interpolationFlags() const;
  void setInterpolationFlags(const std::vector<int> &interpolationFlags);
  int interpolationFlag(size_t index);
  void setInterpolationFlag(size_t index, int flag);

  std::vector<double> filterSizes() const;
  void setFilterSizes(std::vector<double> filterSize);
  double filterSize(size_t index);
  void setFilterSize(size_t index, double filterSize);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  std::vector<double> computeValuesFromRaster(bool useLookupTable = false);
  std::vector<std::vector<double>> computeDirectionalWindReduction(
      bool useLookupTable = false);

  double windRadius() const;

  double windSigma() const;

  int epsg() const;
  void setEpsg(int epsg);

  bool showProgressBar() const;
  void setShowProgressBar(bool showProgressBar);

  double rasterMultiplier() const;
  void setRasterMultiplier(double rasterMultiplier);

  std::unordered_map<size_t, double> lookup() const;

private:
  bool hasKey(int key);
  void buildWindDirectionLookup();
  double calculatePoint(Point &p, double searchRadius, Griddata::Method method);
  double calculateAverage(Point &p, double w);
  double calculateNearest(Point &p, double w);
  double calculateHighest(Point &p, double w);

  double calculatePointFromLookup(Point &p, double w, Griddata::Method method);
  double calculateAverageFromLookup(Point &p, double w);
  double calculateNearestFromLookup(Point &p, double w);
  double calculateHighestFromLookup(Point &p, double w);

  double (Griddata::*m_calculatePointPtr)(Point &p, double w,
                                        Griddata::Method method);

  bool pixelDataInRadius(Point &p, double w, std::vector<double> &x,
                         std::vector<double> &y, std::vector<double> &z);
  bool pixelDataInRadius(Point &p, double w, std::vector<double> &x,
                         std::vector<double> &y, std::vector<int> &z);

  std::vector<double> directionalWind(Point &p, std::vector<double> &x,
                                      std::vector<double> &y,
                                      std::vector<double> &z);

  std::vector<double> directionalWind(Point &p, std::vector<double> &x,
                                      std::vector<double> &y,
                                      std::vector<int> &z);

  void sizeDirectionalWindMatrix(std::vector<std::vector<double>> &result);

  bool computeWindDirectionAndWeight(Point &p, double x, double y, double &w,
                                     int &dir);

  void computeWeightedDirectionalWindValues(double tw, std::vector<double> &wt,
                                            std::vector<double> &r);

  std::vector<double> computeGridScale();
  int windDirection(int i, int j);

  std::vector<double> m_filterSize;
  double m_defaultValue;
  Adcirc::Geometry::Mesh *m_mesh;
  Rasterdata m_raster;
  std::string m_rasterFile;
  std::vector<int> m_interpolationFlags;
  std::unordered_map<size_t, double> m_lookup;
  std::vector<std::vector<int>> m_windDirections;
  int m_epsg;
  double m_rasterMultiplier;
  bool m_showProgressBar;

  const double m_windRadius = 10.0;
  const double m_windSigma = 6.0;
  const double m_windSigmaSquared = m_windSigma * m_windSigma;
};

#endif  // GRIDDATA_H
