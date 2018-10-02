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

#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include "constants.h"
#include "mesh.h"
#include "point.h"
#include "rasterdata.h"

class Griddata {
 public:
  enum Method {
    NoMethod = 0,
    Average = 1,
    Nearest = 2,
    Highest = 3,
    PlusTwoSigma = 4,
    BilskieEtAll = 5
  };

  Griddata();
  Griddata(Adcirc::Geometry::Mesh *mesh, std::string rasterFile);
  ~Griddata();

  std::string rasterFile() const;
  void setRasterFile(const std::string &rasterFile);

  void readLookupTable(const std::string &lookupTableFile);

  std::vector<int> interpolationFlags() const;
  void setInterpolationFlags(const std::vector<int> &interpolationFlags);
  int interpolationFlag(size_t index);
  void setInterpolationFlag(size_t index, int flag);

  std::vector<double> filterSizes() const;
  void setFilterSizes(const std::vector<double> &filterSize);
  double filterSize(size_t index);
  void setFilterSize(size_t index, double filterSize);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  std::vector<double> computeValuesFromRaster(bool useLookupTable = false);
  std::vector<std::vector<double>> computeDirectionalWindReduction(
      bool useLookupTable = false);

  int epsg() const;
  void setEpsg(int epsg);

  bool showProgressBar() const;
  void setShowProgressBar(bool showProgressBar);

  double rasterMultiplier() const;
  void setRasterMultiplier(double rasterMultiplier);

  std::unordered_map<size_t, double> lookup() const;

  bool rasterInMemory() const;
  void setRasterInMemory(bool rasterInMemory);

  static constexpr double windRadius() { return 10000.0; }
  static constexpr double windSigma() { return 6.0; }
  static constexpr double windSigmaSquared() {
    return Griddata::windSigma() * Griddata::windSigma();
  }
  static double windSigma2pi() {
    return sqrt(2.0 * Constants::pi() * Griddata::windSigma());
  }

 private:
  bool getKeyValue(size_t key, double &value);
  void buildWindDirectionLookup();
  double calculatePoint(Point &p, double searchRadius, double gsMultiplier,
                        Griddata::Method method);
  double calculateAverage(Point &p, double w);
  double calculateNearest(Point &p, double w);
  double calculateHighest(Point &p, double w);
  double calculateOutsideStandardDeviation(Point &p, double w, int n);
  double calculateBilskieAveraging(Point &p, double w, double gsMultiplier);

  double calculatePointFromLookup(Point &p, double w, double gsMultiplier,
                                  Griddata::Method method);
  double calculateAverageFromLookup(Point &p, double w);
  double calculateNearestFromLookup(Point &p, double w);
  double calculateHighestFromLookup(Point &p, double w);
  double calculateOutsideStandardDeviationFromLookup(Point &p, double w, int n);
  double calculateBilskieAveragingFromLookup(Point &p, double w,
                                             double gsMultiplier);

  double (Griddata::*m_calculatePointPtr)(Point &p, double w,
                                          double gsMultiplier,
                                          Griddata::Method method);

  std::vector<double> (Griddata::*m_calculateDwindPtr)(Point &p);

  double mean(std::vector<double> &v);
  double standardDeviation(std::vector<double> &v);
  bool calculateBilskieRadius(double meshSize, double rasterCellSize,
                              double &radius);

  bool pixelDataInRadius(Point &p, double radius, std::vector<double> &x,
                         std::vector<double> &y, std::vector<double> &z,
                         std::vector<bool> &valid);
  bool pixelDataInRadius(Point &p, double radius, std::vector<double> &x,
                         std::vector<double> &y, std::vector<int> &z,
                         std::vector<bool> &valid);

  std::vector<double> calculateDirectionalWindFromRaster(Point &p);
  std::vector<double> calculateDirectionalWindFromLookup(Point &p);

  bool computeWindDirectionAndWeight(Point &p, double x, double y, double &w,
                                     int &dir);

  void computeWeightedDirectionalWindValues(std::vector<double> &weight,
                                            std::vector<double> &wind,
                                            double nearWeight);

  std::vector<double> computeGridScale();
  int windDirection(int i, int j);

  void checkMatchingCoorindateSystems();
  void checkRasterOpen();
  void assignDirectionalWindReductionFunctionPointer(bool useLookupTable);
  void assignInterpolationFunctionPointer(bool useLookupTable);

  std::vector<double> m_filterSize;
  double m_defaultValue;
  Adcirc::Geometry::Mesh *m_mesh;
  std::unique_ptr<Rasterdata> m_raster;
  std::string m_rasterFile;
  std::vector<int> m_interpolationFlags;
  std::unordered_map<size_t, double> m_lookup;
  std::vector<std::vector<int>> m_windDirections;
  int m_epsg;
  double m_rasterMultiplier;
  bool m_showProgressBar;
  bool m_rasterInMemory;
};

#endif  // GRIDDATA_H
