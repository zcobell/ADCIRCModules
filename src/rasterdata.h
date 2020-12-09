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
#ifndef ADCMOD_RASTERDATA_H
#define ADCMOD_RASTERDATA_H

#include <string>
#include <vector>

#include "boost/multi_array.hpp"
#include "cpl_conv.h"
#include "cpl_error.h"
#include "gdal_priv.h"
#include "pixel.h"

using Point = std::pair<double, double>;

namespace Adcirc {

namespace Raster {

/**
 * @class Rasterdata
 * @author Zachary Cobell
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 * @brief Class that accesses raster data through the GDAL library
 *
 */
class Rasterdata {
 public:
  Rasterdata();

  Rasterdata(const std::string &filename);

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

  bool open();
  bool close();
  void read();

  size_t nx() const;

  size_t ny() const;

  double dx() const;

  double dy() const;

  double xmin() const;

  double xmax() const;

  double ymin() const;

  double ymax() const;

  template <typename T>
  T nodata() const;

  Point pixelToCoordinate(size_t i, size_t j);
  Point pixelToCoordinate(Adcirc::Raster::Pixel &p);

  Adcirc::Raster::Pixel coordinateToPixel(double x, double y);
  Adcirc::Raster::Pixel coordinateToPixel(const Point &p);

  std::string projectionString() const;

  int searchBoxAroundPoint(double x, double y, double halfSide,
                           Adcirc::Raster::Pixel &upperLeft,
                           Adcirc::Raster::Pixel &lowerRight);

  std::string filename() const;
  void setFilename(const std::string &filename);

  template <typename T>
  T pixelValue(Adcirc::Raster::Pixel &p);

  template <typename T>
  T pixelValue(size_t i, size_t j);

  template <typename T>
  int pixelValues(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<T> &z);

  int rasterType() const;

  int epsg() const;
  void setEpsg(int epsg);

  bool isOpen() const;

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

  boost::multi_array<double, 2> m_doubleOnDisk;
  boost::multi_array<int, 2> m_intOnDisk;

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
