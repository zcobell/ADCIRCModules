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
#ifndef ADCMOD_GRIDDATAPRIVATE_H
#define ADCMOD_GRIDDATAPRIVATE_H

#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include "adcmap.h"
#include "constants.h"
#include "interpolationmethods.h"
#include "mesh.h"
#include "rasterdata.h"

namespace Adcirc {
namespace Private {

using Point = std::pair<double, double>;

class GriddataPrivate {
 public:
  GriddataPrivate();

  GriddataPrivate(const std::vector<double> &x, const std::vector<double> &y,
                  std::vector<double> resolution, const std::string &rasterFile,
                  int epsgQuery, int epsgRaster);

  GriddataPrivate(Adcirc::Geometry::Mesh *mesh, const std::string &rasterFile,
                  int epsgRaster);

  std::string rasterFile() const;
  void setRasterFile(const std::string &rasterFile);

  void readLookupTable(const std::string &lookupTableFile);

  std::vector<Interpolation::Method> interpolationFlags() const;
  void setInterpolationFlags(
      const std::vector<Interpolation::Method> &interpolationFlags);
  void setInterpolationFlags(Interpolation::Method interpolationFlag);
  Interpolation::Method interpolationFlag(size_t index);
  void setInterpolationFlag(size_t index, Interpolation::Method flag);

  std::vector<Interpolation::Method> backupInterpolationFlags() const;
  void setBackupInterpolationFlags(
      const std::vector<Interpolation::Method> &interpolationFlags);
  void setBackupInterpolationFlags(Interpolation::Method interpolationFlag);
  Interpolation::Method backupInterpolationFlag(size_t index);
  void setBackupInterpolationFlag(size_t index, Interpolation::Method flag);

  std::vector<double> filterSizes() const;
  void setFilterSizes(const std::vector<double> &filterSize);
  void setFilterSizes(double &filterSize);
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
  void setShowProgressBar(bool showProgressBar, int displayMode = 0);

  double rasterMultiplier() const;
  void setRasterMultiplier(double rasterMultiplier);

  bool rasterInMemory() const;
  void setRasterInMemory(bool rasterInMemory);

  static constexpr double windRadius() { return 10000.0; }
  static constexpr double windSigma() { return 6.0; }

  double datumShift() const;
  void setDatumShift(double datumShift);

  double thresholdValue() const;
  void setThresholdValue(double thresholdValue);

  Interpolation::Threshold thresholdMethod() const;
  void setThresholdMethod(const Interpolation::Threshold &thresholdMethod);

  std::vector<double> extents() const;

 private:
  bool getKeyValue(unsigned key, double &value);

  constexpr double methodErrorValue() {
    return -std::numeric_limits<double>::max();
  }

  double calculatePoint(const Point &p, double searchRadius,
                        double gsMultiplier, Interpolation::Method method);
  double calculateAverage(const Point &p, double w);
  double calculateNearest(const Point &p, double w);
  double calculateHighest(const Point &p, double w);
  double calculateOutsideStandardDeviation(const Point &p, double w, int n);
  double calculateBilskieAveraging(const Point &p, double w,
                                   double gsMultiplier);
  double calculateInverseDistanceWeighted(const Point &p, double w);
  double calculateInverseDistanceWeightedNPoints(const Point &p, double n);
  double calculateAverageNearestN(const Point &p, double n);

  double calculatePointFromLookup(const Point &p, double w, double gsMultiplier,
                                  Interpolation::Method method);
  double calculateAverageFromLookup(const Point &p, double w);
  double calculateNearestFromLookup(const Point &p, double w);
  double calculateHighestFromLookup(const Point &p, double w);
  double calculateOutsideStandardDeviationFromLookup(const Point &p, double w,
                                                     int n);
  double calculateBilskieAveragingFromLookup(const Point &p, double w,
                                             double gsMultiplier);
  double calculateInverseDistanceWeightedFromLookup(const Point &p, double w);
  double calculateInverseDistanceWeightedNPointsFromLookup(const Point &p,
                                                           double n);
  double calculateAverageNearestNFromLookup(const Point &p, double n);

  double (GriddataPrivate::*m_calculatePointPtr)(const Point &p, double w,
                                                 double gsMultiplier,
                                                 Interpolation::Method method);

  std::vector<double> (GriddataPrivate::*m_calculateDwindPtr)(const Point &p);

  bool calculateBilskieRadius(double meshSize, double rasterCellSize,
                              double &radius);

  template <typename T>
  bool pixelDataInRadius(const Point &p, double radius, std::vector<double> &x,
                         std::vector<double> &y, std::vector<T> &z,
                         std::vector<bool> &valid);

  template <typename T>
  void thresholdData(std::vector<T> &z, std::vector<bool> &v);

  std::vector<double> calculateDirectionalWindFromRaster(const Point &p);
  std::vector<double> calculateDirectionalWindFromLookup(const Point &p);

  bool computeWindDirectionAndWeight(const Point &p, double x, double y,
                                     double &w, int &dir);

  void computeWeightedDirectionalWindValues(std::vector<double> &weight,
                                            std::vector<double> &wind,
                                            double nearWeight);

  short windDirection(short i, short j);

  void checkRasterOpen();
  void assignDirectionalWindReductionFunctionPointer(bool useLookupTable);
  void assignInterpolationFunctionPointer(bool useLookupTable);
  double calculateExpansionLevelForPoints(size_t n);

  static std::vector<Point> meshToQueryPoints(Adcirc::Geometry::Mesh *m,
                                              int epsgRaster);

  static std::vector<std::pair<double, double>> convertQueryPointCoordinates(
      const std::vector<Point> &input, int epsgInput, int epsgOutput);

  std::vector<double> m_lookup;
  Interpolation::Threshold m_thresholdMethod;

  std::unique_ptr<Adcirc::Raster::Rasterdata> m_raster;

  std::vector<Point> m_queryLocations;
  std::vector<double> m_filterSize;
  std::vector<double> m_queryResolution;
  std::vector<Interpolation::Method> m_interpolationFlags;
  std::vector<Interpolation::Method> m_backupFlags;

  std::string m_rasterFile;

  int m_inputEpsg;
  int m_epsg;
  int m_progressDisplayMode;

  double m_defaultValue;
  double m_rasterMultiplier;
  double m_datumShift;
  double m_thresholdValue;

  bool m_showProgressBar;
  bool m_rasterInMemory;
};

}  // namespace Private
}  // namespace Adcirc

#endif  // ADCMOD_GRIDDATAPRIVATE_H
