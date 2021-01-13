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
#ifndef ADCMOD_GRIDDATA_H
#define ADCMOD_GRIDDATA_H

#include <mesh.h>

#include <memory>
#include <string>
#include <vector>

#include "interpolationmethods.h"

namespace Adcirc {

namespace Private {
// Forward declaration of pimpl class
class GriddataPrivate;
}  // namespace Private

namespace Interpolation {

/**
 * @class Griddata
 * @author Zachary Cobell
 * @brief Class that handles interpolation on unstructured meshes
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 *
 * The Griddata class is used to interpolate data to unstructured meshes
 * containing either quadrilaterals or triangles. There are a number of schemes
 * that can be used to interpolate the data to the mesh, including: (1) Grid
 * cell averaging (2) Nearest point (3) Maximum point (4) Average of data
 * outside two standard deviations (5) Average within optimal radius (6) Inverse
 * distance weighting (7) Upwind directional roughness (12 directions)
 *
 * The interpolated values come from any raster format that can be accessed
 * using the GDAL library. The raster data can either be values (floating point
 * numbers) or classifications that require a lookup table. The lookup table
 * is stored as a hashtable internally. For optimium performance, Google's
 * Swiss Tables are used where available (Cygwin is not available and will
 * use the stl unordered_map structure instead).
 *
 * This class uses the PIMPL idiom and the implementation of the
 * class is contained in the MeshImpl class.
 *
 */
class Griddata {
 public:
  ADCIRCMODULES_EXPORT Griddata(const std::vector<double> &x,
                                const std::vector<double> &y,
                                std::vector<double> resolution,
                                const std::string &rasterFile, int epsgQuery,
                                int epsgRaster);

  ADCIRCMODULES_EXPORT Griddata(Adcirc::Geometry::Mesh *mesh,
                                const std::string &rasterFile, int epsgRaster);
  ADCIRCMODULES_EXPORT ~Griddata();

  std::string ADCIRCMODULES_EXPORT rasterFile() const;
  void ADCIRCMODULES_EXPORT setRasterFile(const std::string &rasterFile);

  void ADCIRCMODULES_EXPORT readLookupTable(const std::string &lookupTableFile);

  std::vector<Adcirc::Interpolation::Method> interpolationFlags() const;
  void ADCIRCMODULES_EXPORT setInterpolationFlags(
      const std::vector<Adcirc::Interpolation::Method> &interpolationFlags);
  void ADCIRCMODULES_EXPORT
  setInterpolationFlags(Adcirc::Interpolation::Method interpolationFlag);
  Adcirc::Interpolation::Method ADCIRCMODULES_EXPORT
  interpolationFlag(size_t index);
  void ADCIRCMODULES_EXPORT
  setInterpolationFlag(size_t index, Adcirc::Interpolation::Method flag);

  std::vector<Adcirc::Interpolation::Method> ADCIRCMODULES_EXPORT
  backupInterpolationFlags() const;
  void ADCIRCMODULES_EXPORT setBackupInterpolationFlags(
      const std::vector<Adcirc::Interpolation::Method> &interpolationFlags);
  void ADCIRCMODULES_EXPORT
  setBackupInterpolationFlags(Adcirc::Interpolation::Method interpolationFlag);
  Adcirc::Interpolation::Method ADCIRCMODULES_EXPORT
  backupInterpolationFlag(size_t index);
  void ADCIRCMODULES_EXPORT
  setBackupInterpolationFlag(size_t index, Adcirc::Interpolation::Method flag);

  std::vector<double> ADCIRCMODULES_EXPORT filterSizes() const;
  void ADCIRCMODULES_EXPORT
  setFilterSizes(const std::vector<double> &filterSize);
  void ADCIRCMODULES_EXPORT setFilterSizes(double filterSize);
  double ADCIRCMODULES_EXPORT filterSize(size_t index);
  void ADCIRCMODULES_EXPORT setFilterSize(size_t index, double filterSize);

  double ADCIRCMODULES_EXPORT defaultValue() const;
  void ADCIRCMODULES_EXPORT setDefaultValue(double defaultValue);

  std::vector<double> ADCIRCMODULES_EXPORT
  computeValuesFromRaster(bool useLookupTable = false);

  std::vector<std::vector<double>> ADCIRCMODULES_EXPORT
  computeDirectionalWindReduction(bool useLookupTable = false);

  int ADCIRCMODULES_EXPORT epsg() const;
  void ADCIRCMODULES_EXPORT setEpsg(int epsg);

  bool ADCIRCMODULES_EXPORT showProgressBar() const;
  void ADCIRCMODULES_EXPORT setShowProgressBar(bool showProgressBar);

  double ADCIRCMODULES_EXPORT rasterMultiplier() const;
  void ADCIRCMODULES_EXPORT setRasterMultiplier(double rasterMultiplier);

  bool ADCIRCMODULES_EXPORT rasterInMemory() const;
  void ADCIRCMODULES_EXPORT setRasterInMemory(bool rasterInMemory);

  double ADCIRCMODULES_EXPORT datumShift() const;
  void ADCIRCMODULES_EXPORT setDatumShift(double datumShift);

  Interpolation::Threshold ADCIRCMODULES_EXPORT thresholdMethod() const;
  void ADCIRCMODULES_EXPORT
  setThresholdMethod(const Interpolation::Threshold &method);

  double ADCIRCMODULES_EXPORT thresholdValue() const;
  void ADCIRCMODULES_EXPORT setThresholdValue(const double &value);

  double ADCIRCMODULES_EXPORT windRadius() const;

  std::vector<double> extents() const;

 private:
  std::unique_ptr<Adcirc::Private::GriddataPrivate> m_impl;
};
}  // namespace Interpolation
}  // namespace Adcirc
#endif  // ADCMOD_GRIDDATA_H
