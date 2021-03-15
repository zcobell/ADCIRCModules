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

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include "Constants.h"
#include "GriddataAttribute.h"
#include "GriddataConfig.h"
#include "InterpolationMethods.h"
#include "Mesh.h"
#include "PixelValueVector.h"
#include "Point.h"
#include "RasterData.h"

namespace Adcirc {
namespace Private {

class GriddataPrivate {
 public:
  GriddataPrivate(const std::vector<double> &x, const std::vector<double> &y,
                  const std::vector<double> &resolution,
                  const std::string &rasterFile, int epsgQuery, int epsgRaster);

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
  void setShowProgressBar(bool showProgressBar);

  double rasterMultiplier() const;
  void setRasterMultiplier(double rasterMultiplier);

  bool rasterInMemory() const;
  void setRasterInMemory(bool rasterInMemory);

  double datumShift() const;
  void setDatumShift(double datumShift);

  double thresholdValue() const;
  void setThresholdValue(double thresholdValue);

  Interpolation::Threshold thresholdMethod() const;
  void setThresholdMethod(const Interpolation::Threshold &thresholdMethod);

  std::vector<double> extents() const;

  static double windRadius() ;

 private:
  double calculatePoint(size_t index, const Interpolation::Method &method);

  void checkRasterOpen();

  static std::vector<Point> meshToQueryPoints(Adcirc::Geometry::Mesh *m,
                                              int epsgRaster);

  static std::vector<Point> convertQueryPointCoordinates(
      const std::vector<Point> &input, int epsgInput, int epsgOutput);

  std::unique_ptr<Adcirc::Raster::Rasterdata> m_raster;
  std::vector<Adcirc::Private::GriddataAttribute> m_attributes;
  Adcirc::Private::GriddataConfig m_config;

  std::string m_rasterFile;

  int m_epsg;

  bool m_showProgressBar;
  bool m_rasterInMemory;
};

}  // namespace Private
}  // namespace Adcirc

#endif  // ADCMOD_GRIDDATAPRIVATE_H
