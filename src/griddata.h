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

#include <mesh.h>
#include <memory>
#include <string>
#include <vector>
#include "interpolationmethods.h"

class GriddataImpl;

namespace Interpolation {

/**
 * @class Griddata
 * @author Zachary Cobell
 * @brief Class that handles interpolation on unstructured meshes
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
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
  Griddata();
  Griddata(Adcirc::Geometry::Mesh *mesh, std::string rasterFile);
  ~Griddata();

  std::string rasterFile() const;
  void setRasterFile(const std::string &rasterFile);

  void readLookupTable(const std::string &lookupTableFile);

  std::vector<int> interpolationFlags() const;
  void setInterpolationFlags(const std::vector<int> &interpolationFlags);
  void setInterpolationFlags(int interpolationFlag);
  int interpolationFlag(size_t index);
  void setInterpolationFlag(size_t index, int flag);

  std::vector<double> filterSizes() const;
  void setFilterSizes(const std::vector<double> &filterSize);
  void setFilterSizes(double filterSize);
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

 private:
  GriddataImpl *m_impl;
};
}  // namespace Interpolation
#endif  // GRIDDATA_H
