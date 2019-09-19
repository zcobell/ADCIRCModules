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
#ifndef ADCMOD_RASTERDATA_H
#define ADCMOD_RASTERDATA_H

#include <string>
#include <vector>
#include "adcircmodules_global.h"
#include "pixel.h"

using Point = std::pair<double, double>;

// Forward declaration of GDAL classes
class GDALDataset;
class GDALRasterBand;

namespace Adcirc {

namespace Raster {

/**
 * @class Rasterdata
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief Class that accesses raster data through the GDAL library
 *
 */
class Rasterdata {
 public:
  ADCIRCMODULES_EXPORT Rasterdata();

  ADCIRCMODULES_EXPORT Rasterdata(const std::string &filename);

  ~Rasterdata();

  enum RasterTypes {
    Bool,
    Unsigned,
    Float,
    Double,
    Integer,
    ComplexInt,
    ComplexDouble,
    Unknown
  };

  ADCIRCMODULES_EXPORT bool open();
  ADCIRCMODULES_EXPORT bool close();
  ADCIRCMODULES_EXPORT void read();

  ADCIRCMODULES_EXPORT size_t nx() const;

  ADCIRCMODULES_EXPORT size_t ny() const;

  ADCIRCMODULES_EXPORT double dx() const;

  ADCIRCMODULES_EXPORT double dy() const;

  ADCIRCMODULES_EXPORT double xmin() const;

  ADCIRCMODULES_EXPORT double xmax() const;

  ADCIRCMODULES_EXPORT double ymin() const;

  ADCIRCMODULES_EXPORT double ymax() const;

  template <typename T>
  ADCIRCMODULES_EXPORT T nodata() const;

  ADCIRCMODULES_EXPORT Point pixelToCoordinate(size_t i, size_t j);
  ADCIRCMODULES_EXPORT Point pixelToCoordinate(Adcirc::Raster::Pixel &p);

  ADCIRCMODULES_EXPORT Adcirc::Raster::Pixel coordinateToPixel(double x,
                                                               double y);
  ADCIRCMODULES_EXPORT Adcirc::Raster::Pixel coordinateToPixel(Point &p);

  ADCIRCMODULES_EXPORT std::string projectionString() const;

  ADCIRCMODULES_EXPORT int searchBoxAroundPoint(
      double x, double y, double halfSide, Adcirc::Raster::Pixel &upperLeft,
      Adcirc::Raster::Pixel &lowerRight);

  ADCIRCMODULES_EXPORT std::string filename() const;
  ADCIRCMODULES_EXPORT void setFilename(const std::string &filename);

  template <typename T>
  ADCIRCMODULES_EXPORT T pixelValue(Adcirc::Raster::Pixel &p);

  template <typename T>
  ADCIRCMODULES_EXPORT T pixelValue(size_t i, size_t j);

  template <typename T>
  ADCIRCMODULES_EXPORT int pixelValues(size_t ibegin, size_t jbegin,
                                       size_t iend, size_t jend,
                                       std::vector<double> &x,
                                       std::vector<double> &y,
                                       std::vector<T> &z);

  ADCIRCMODULES_EXPORT int rasterType() const;

  ADCIRCMODULES_EXPORT int epsg() const;
  ADCIRCMODULES_EXPORT void setEpsg(int epsg);

  ADCIRCMODULES_EXPORT bool isOpen() const;

 private:
  void init();
  bool getRasterMetadata();
  Adcirc::Raster::Rasterdata::RasterTypes selectRasterType(int d);

  template <typename T>
  int pixelValuesFromDisk(size_t ibegin, size_t jbegin, size_t iend,
                          size_t jend, std::vector<double> &x,
                          std::vector<double> &y, std::vector<T> &z);

  template <typename T>
  int pixelValuesFromMemory(size_t ibegin, size_t jbegin, size_t iend,
                            size_t jend, std::vector<double> &x,
                            std::vector<double> &y, std::vector<T> &z);

  void readIntegerRasterToMemory();
  void readDoubleRasterToMemory();

  GDALDataset *m_file;
  GDALRasterBand *m_band;

  std::vector<std::vector<double>> m_doubleOnDisk;
  std::vector<std::vector<int>> m_intOnDisk;

  bool m_isOpen;
  bool m_isRead;
  size_t m_nx, m_ny;
  int m_epsg;
  double m_dx, m_dy;
  double m_xmin, m_xmax, m_ymin, m_ymax;
  double m_nodata;
  int m_nodataint;
  int m_readType;
  RasterTypes m_rasterType;
  std::string m_projectionReference;
  std::string m_filename;
};
}  // namespace Raster
}  // namespace Adcirc

#endif  // ADCMOD_RASTERDATA_H
