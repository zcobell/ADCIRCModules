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
#ifndef RASTERDATA_H
#define RASTERDATA_H

#include <string>
#include <vector>
#include "pixel.h"
#include "point.h"

class GDALDataset;
class GDALRasterBand;

/**
 * @class Constants
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
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

  double nodata() const;

  Point pixelToCoordinate(size_t i, size_t j);
  Point pixelToCoordinate(Pixel &p);

  Pixel coordinateToPixel(double x, double y);
  Pixel coordinateToPixel(Point &p);

  std::string projectionString() const;

  int searchBoxAroundPoint(double x, double y, double halfSide,
                           Pixel &upperLeft, Pixel &lowerRight);

  std::string filename() const;
  void setFilename(const std::string &filename);

  double pixelValueDouble(Pixel &p);
  double pixelValueDouble(size_t i, size_t j);
  int pixelValueInt(Pixel p);
  int pixelValueInt(size_t i, size_t j);

  int pixelValues(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<int> &z);

  int pixelValues(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<double> &z);

  int rasterType() const;

  int epsg() const;
  void setEpsg(int epsg);

  bool isOpen() const;

  int nodataint() const;

 private:
  void init();
  bool getRasterMetadata();
  RasterTypes selectRasterType(int d);
  int pixelValuesFromDisk(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<int> &z);

  int pixelValuesFromDisk(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<double> &z);

  int pixelValuesFromMemory(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<int> &z);

  int pixelValuesFromMemory(size_t ibegin, size_t jbegin, size_t iend, size_t jend,
                  std::vector<double> &x, std::vector<double> &y,
                  std::vector<double> &z);

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
  RasterTypes m_rasterType;
  std::string m_projectionReference;
  std::string m_filename;
};

#endif  // RASTERDATA_H
