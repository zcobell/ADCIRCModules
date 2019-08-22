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
#include "rasterdata.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include "cpl_conv.h"
#include "cpl_error.h"
#include "gdal_priv.h"
#include "logging.h"

//...Template Instantiation
template int Rasterdata::pixelValue<int>(Pixel &p);
template double Rasterdata::pixelValue<double>(Pixel &p);
template int Rasterdata::pixelValues<int>(size_t ibegin, size_t jbegin,
                                          size_t iend, size_t jend,
                                          std::vector<double> &x,
                                          std::vector<double> &y,
                                          std::vector<int> &z);
template int Rasterdata::pixelValues<double>(size_t ibegin, size_t jbegin,
                                             size_t iend, size_t jend,
                                             std::vector<double> &x,
                                             std::vector<double> &y,
                                             std::vector<double> &z);
template int Rasterdata::nodata<int>() const;
template double Rasterdata::nodata<double>() const;

/**
 * @brief Default constructor without a filename
 */
Rasterdata::Rasterdata() : m_filename(std::string()) {
  this->init();
  return;
}

/**
 * @brief Default constructor that takes a filename
 * @param filename name of raster file
 */
Rasterdata::Rasterdata(const std::string &filename) : m_filename(filename) {
  this->init();
  return;
}

Rasterdata::~Rasterdata() { this->close(); }

/**
 * @brief Initialization routine.
 *
 * Sets all internals to default values, many of which are just extreme values
 * that can be easily detected
 */
void Rasterdata::init() {
  this->m_file = nullptr;
  this->m_band = nullptr;
  this->m_isOpen = false;
  this->m_isRead = false;
  this->m_epsg = 4326;
  this->m_nx = -std::numeric_limits<size_t>::max();
  this->m_ny = -std::numeric_limits<size_t>::max();
  this->m_xmin = std::numeric_limits<double>::max();
  this->m_xmax = -std::numeric_limits<double>::max();
  this->m_ymin = std::numeric_limits<double>::max();
  this->m_ymax = -std::numeric_limits<double>::max();
  this->m_dx = 0.0;
  this->m_dy = 0.0;
  this->m_nodata = -std::numeric_limits<double>::max();
  this->m_nodataint = -std::numeric_limits<int>::max();
  this->m_readType = GDT_Unknown;
  return;
}

/**
 * @brief Opens a raster using the GDAL library
 * @return true if the file was successfully opened
 */
bool Rasterdata::open() {
  GDALAllRegister();
  this->m_file = static_cast<GDALDataset *>(
      GDALOpen(this->m_filename.c_str(), GA_ReadOnly));
  if (this->m_file == nullptr) {
    return false;
  } else {
    this->m_isOpen = true;
    return this->getRasterMetadata();
  }
}

/**
 * @brief Reads the raster metadata
 * @return true if metadata was successfully read
 *
 * This will get the nodata value, raster band std::pair<double,double>er, data
 * type, and x,y sizes
 */
bool Rasterdata::getRasterMetadata() {
  int successNoData;
  assert(this->m_file != nullptr);

  this->m_band = this->m_file->GetRasterBand(1);
  this->m_nodata = this->m_band->GetNoDataValue(&successNoData);

  if (!successNoData) {
    this->m_nodata = std::numeric_limits<double>::min();
  }

  this->m_nodataint = static_cast<int>(std::round(this->m_nodata));

  GDALDataType d = this->m_band->GetRasterDataType();
  this->m_rasterType = this->selectRasterType(d);

  double adfGeoTransform[6];
  if (this->m_file->GetGeoTransform(adfGeoTransform) == CE_None) {
    this->m_nx = static_cast<size_t>(this->m_file->GetRasterXSize());
    this->m_ny = static_cast<size_t>(this->m_file->GetRasterYSize());
    this->m_xmin = adfGeoTransform[0];
    this->m_ymax = adfGeoTransform[3];
    this->m_dx = adfGeoTransform[1];
    this->m_dy = -adfGeoTransform[5];
    this->m_xmax =
        static_cast<double>(this->m_nx - 1) * this->m_dx + this->m_xmin;
    this->m_ymin =
        static_cast<double>(this->m_ny - 1) * (-this->m_dy) + this->m_ymax;
  } else {
    return false;
  }

  this->m_projectionReference = std::string(this->m_file->GetProjectionRef());
  return true;
}

/**
 * @brief Returns the raster type information
 * @return RasterType enum
 */
int Rasterdata::rasterType() const { return this->m_rasterType; }

/**
 * @brief Filename of raster being used in this object
 * @return filename
 */
std::string Rasterdata::filename() const { return this->m_filename; }

/**
 * @brief Sets the filename for the raster to use in the object
 * @param filename name of file
 */
void Rasterdata::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

/**
 * @brief Closes the GDAL raster object
 * @return true if object was successfully closed
 */
bool Rasterdata::close() {
  if (this->m_file != nullptr) {
    GDALClose(static_cast<GDALDatasetH>(this->m_file));
    this->m_isOpen = false;
    return true;
  }
  return false;
}

/**
 * @brief Calculates the coordinate of the pixel center
 * @param i i-index
 * @param j j-index
 * @return std::pair<double,double> object with coordinate information for the
 * pixel
 */
Point Rasterdata::pixelToCoordinate(size_t i, size_t j) {
  if (i <= this->m_nx && j <= this->m_ny) {
    return Point(
        static_cast<double>(i) * this->m_dx + this->m_xmin + 0.50 * this->m_dx,
        this->m_ymax - (static_cast<double>(j + 1) * this->m_dy) +
            0.50 * this->m_dy);
  } else {
    return Point();
  }
}

/**
 * @brief Calculates the coordinate of the pixel center
 * @param p Pixel object with i,j indicies
 * @return std::pair<double,double> object with coordinate information for the
 * pixel
 */
Point Rasterdata::pixelToCoordinate(Pixel &p) {
  return this->pixelToCoordinate(p.i(), p.j());
}

/**
 * @brief Converts an x,y coordinate to the pixel i,j
 * @param x x-coordinate
 * @param y y-coordinate
 * @return Pixel object with i,j indicies for given x,y
 */
Pixel Rasterdata::coordinateToPixel(double x, double y) {
  if (x > this->xmax() || x < this->xmin() || y > this->ymax() || y < ymin()) {
    return Pixel();
  } else {
    return Pixel(static_cast<size_t>((x - this->m_xmin) / this->m_dx),
                 static_cast<size_t>((this->m_ymax - y) / this->m_dy));
  }
}

/**
 * @brief Converts an x,y coordinate to the pixel i,j
 * @param p std::pair<double,double> object with x,y-coordinates
 * @return Pixel object with i,j indicies for given x,y
 */
Pixel Rasterdata::coordinateToPixel(Point &p) {
  return this->coordinateToPixel(p.first, p.second);
}

/**
 * @brief y-max value for raster
 * @return y-max value
 */
double Rasterdata::ymax() const { return this->m_ymax; }

/**
 * @brief y-min value for raster
 * @return y-min value
 */
double Rasterdata::ymin() const { return this->m_ymin; }

/**
 * @brief x-max value for raster
 * @return x-max value
 */
double Rasterdata::xmax() const { return this->m_xmax; }

/**
 * @brief x-min value for raster
 * @return x-min value
 */
double Rasterdata::xmin() const { return this->m_xmin; }

/**
 * @brief dy value for raster
 * @return dy value
 */
double Rasterdata::dy() const { return this->m_dy; }

/**
 * @brief dx value for raster
 * @return dx value
 */
double Rasterdata::dx() const { return this->m_dx; }

/**
 * @brief nodata value used in the raster
 * @return nodata value
 */
template <typename T>
T Rasterdata::nodata() const {
  if (std::is_same<T, double>::value) {
    return this->m_nodata;
  } else if (std::is_same<T, int>::value) {
    return this->m_nodataint;
  }
}

/**
 * @brief Number of pixels in the y-direction
 * @return number of y pixels
 */
size_t Rasterdata::ny() const { return this->m_ny; }

/**
 * @brief Number of pixels in the x-direction
 * @return number of x pixels
 */
size_t Rasterdata::nx() const { return this->m_nx; }

/**
 * @brief Projection string embedded in raster
 * @return projection string
 */
std::string Rasterdata::projectionString() const {
  return this->m_projectionReference;
}

/**
 * @brief Returns the pixel value for the i,j index
 * @param i i-index
 * @param j j-index
 * @return pixel value
 */
template <typename T>
T Rasterdata::pixelValue(size_t i, size_t j) {
  Pixel p = Pixel(i, j);
  return this->pixelValue<T>(p);
}

/**
 * @brief Returns the pixel value for the i,j index
 * @param p pixel with indicies
 * @return pixel value
 */
template <typename T>
T Rasterdata::pixelValue(Pixel &p) {
  if (p.i() > 0 && p.j() > 0 && p.i() < this->nx() && p.j() < this->ny()) {
    T *buf = static_cast<T *>(CPLMalloc(sizeof(T) * 1));
    this->m_band->RasterIO(GF_Read, p.i(), p.j(), 1, 1, buf, 1, 1,
                           static_cast<GDALDataType>(this->m_readType), 0, 0);
    double v = buf[0];
    CPLFree(buf);
    return v;
  } else {
    return this->nodata<T>();
  }
}

/**
 * @brief Returns a search box of a given size around a given
 * std::pair<double,double>
 * @param x x-location
 * @param y y-location
 * @param halfSide half the side of the search box
 * @param upperLeft upper left pixel
 * @param lowerRight lower right pixel
 * @return number of pixels selected
 */
int Rasterdata::searchBoxAroundPoint(double x, double y, double halfSide,
                                     Pixel &upperLeft, Pixel &lowerRight) {
  Pixel p = this->coordinateToPixel(x, y);
  if (p.isValid()) {
    int nx = static_cast<int>(std::round(halfSide / this->m_dx));
    int ny = static_cast<int>(std::round(halfSide / this->m_dy));
    int ibegin = std::max<int>(1, p.i() - nx);
    int iend = std::min<int>(this->m_nx - 1, p.i() + nx);
    int jbegin = std::max<int>(1, p.j() - ny);
    int jend = std::min<int>(this->m_ny - 1, p.j() + ny);
    nx = iend - ibegin + 1;
    ny = jend - jbegin + 1;
    upperLeft = Pixel(ibegin, jbegin);
    lowerRight = Pixel(iend, jend);
    return nx * ny;
  } else {
    upperLeft.setInvalid();
    lowerRight.setInvalid();
    return 0;
  }
}

/**
 * @brief Reads the entire raster into memory to avoid disk access
 *
 * Not always the fastest method, but in some cases it is worthwhile
 */
void Rasterdata::readDoubleRasterToMemory() {
  size_t n = this->nx() * this->ny();
  double *buf = (double *)CPLMalloc(sizeof(double) * n);
  CPLErr e = this->m_band->RasterIO(
      GF_Read, 0, 0, this->nx(), this->ny(), buf, this->nx(), this->ny(),
      static_cast<GDALDataType>(this->m_readType), 0, 0);

  this->m_doubleOnDisk.resize(this->nx());
  for (size_t i = 0; i < this->m_doubleOnDisk.size(); ++i) {
    this->m_doubleOnDisk[i].resize(this->ny());
  }

  size_t k = 0;
  for (size_t j = 0; j < this->ny(); ++j) {
    for (size_t i = 0; i < this->nx(); ++i) {
      this->m_doubleOnDisk[i][j] = buf[k];
      k++;
    }
  }
  CPLFree(buf);
}

/**
 * @brief Reads the entire raster into memory to avoid disk access
 *
 * Not always the fastest method, but in some cases it is worthwhile
 */
void Rasterdata::readIntegerRasterToMemory() {
  size_t n = this->nx() * this->ny();
  int *buf = (int *)CPLMalloc(sizeof(int) * n);
  CPLErr e = this->m_band->RasterIO(
      GF_Read, 0, 0, this->nx(), this->ny(), buf, this->nx(), this->ny(),
      static_cast<GDALDataType>(this->m_readType), 0, 0);

  this->m_intOnDisk.resize(this->nx());
  for (size_t i = 0; i < this->m_intOnDisk.size(); ++i) {
    this->m_intOnDisk[i].resize(this->ny());
  }

  size_t k = 0;
  for (size_t j = 0; j < this->ny(); ++j) {
    for (size_t i = 0; i < this->nx(); ++i) {
      this->m_intOnDisk[i][j] = buf[k];
      k++;
    }
  }
  CPLFree(buf);
}

/**
 * @brief Reads the entire raster into memory to avoid disk access
 *
 * Not always the fastest method, but in some cases it is worthwhile
 */
void Rasterdata::read() {
  if (this->m_isRead) return;
  if (this->m_rasterType == RasterTypes::Double) {
    this->readDoubleRasterToMemory();
  } else {
    this->readIntegerRasterToMemory();
  }
  this->m_isRead = true;
  return;
}

/**
 * @brief Reads the pixel values for the given search box
 * @param ibegin beginning i-index
 * @param jbegin beginning j-index
 * @param iend ending i-index
 * @param jend ending j-index
 * @param x x-location vector
 * @param y y-location vector
 * @param z z-values from raster
 * @return status
 */
template <typename T>
int Rasterdata::pixelValues(size_t ibegin, size_t jbegin, size_t iend,
                            size_t jend, std::vector<double> &x,
                            std::vector<double> &y, std::vector<T> &z) {
  if (this->m_isRead) {
    return this->pixelValuesFromMemory<T>(ibegin, jbegin, iend, jend, x, y, z);
  } else {
    return this->pixelValuesFromDisk<T>(ibegin, jbegin, iend, jend, x, y, z);
  }
}

/**
 * @brief Reads the pixel values for the given search box from memory
 * @param ibegin beginning i-index
 * @param jbegin beginning j-index
 * @param iend ending i-index
 * @param jend ending j-index
 * @param x x-location vector
 * @param y y-location vector
 * @param z z-values from raster
 * @return status
 */
template <typename T>
int Rasterdata::pixelValuesFromMemory(size_t ibegin, size_t jbegin, size_t iend,
                                      size_t jend, std::vector<double> &x,
                                      std::vector<double> &y,
                                      std::vector<T> &z) {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;

  if (x.size() != n) {
    x.resize(n);
    y.resize(n);
    z.resize(n);
  }

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      x[k] = p.first;
      y[k] = p.second;
      if (std::is_same<T, int>::value) {
        z[k] = this->m_intOnDisk[i - 1][j - 1];
      } else if (std::is_same<T, double>::value) {
        z[k] = this->m_doubleOnDisk[i - 1][j - 1];
      } else {
        adcircmodules_throw_exception("Rasterdata: Invalid pixel type");
      }
      k++;
    }
  }
  return 0;
}

/**
 * @brief Reads the pixel values for the given search box from disk
 * @param ibegin beginning i-index
 * @param jbegin beginning j-index
 * @param iend ending i-index
 * @param jend ending j-index
 * @param x x-location vector
 * @param y y-location vector
 * @param z z-values from raster
 * @return status
 */
template <typename T>
int Rasterdata::pixelValuesFromDisk(size_t ibegin, size_t jbegin, size_t iend,
                                    size_t jend, std::vector<double> &x,
                                    std::vector<double> &y, std::vector<T> &z) {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;
  T *buf = static_cast<T *>(CPLMalloc(sizeof(T) * n));
  CPLErr e =
      this->m_band->RasterIO(GF_Read, ibegin, jbegin, nx, ny, buf, nx, ny,
                             static_cast<GDALDataType>(this->m_readType), 0, 0);

  if (x.size() != n) {
    x.resize(n);
    y.resize(n);
    z.resize(n);
  }

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      x[k] = p.first;
      y[k] = p.second;
      z[k] = buf[k];
      k++;
    }
  }
  CPLFree(buf);
  return static_cast<int>(e);
}

/**
 * @brief Determines the raster type via the GDAL specified type
 * @param GDAL raster code
 * @return raster type enum
 */
Rasterdata::RasterTypes Rasterdata::selectRasterType(int d) {
  this->m_readType = d;
  if (d == GDT_Byte) {
    return RasterTypes::Bool;
  } else if (d == GDT_UInt16 || d == GDT_UInt32) {
    return RasterTypes::Unsigned;
  } else if (d == GDT_Int16 || d == GDT_Int32) {
    return RasterTypes::Integer;
  } else if (d == GDT_Float32 || d == GDT_Float64) {
    return RasterTypes::Double;
  } else if (d == GDT_CInt16 || d == GDT_CInt32) {
    return RasterTypes::ComplexInt;
  } else if (d == GDT_CFloat32 || d == GDT_CFloat64) {
    return RasterTypes::ComplexDouble;
  } else {
    return RasterTypes::Unknown;
  }
}

/**
 * @brief Returns the status of the raster
 * @return true if the raster is open
 */
bool Rasterdata::isOpen() const { return this->m_isOpen; }

/**
 * @brief Returns the raster epsg code
 * @return epsg code
 */
int Rasterdata::epsg() const { return this->m_epsg; }

/**
 * @brief Sets the raster epsg code
 * @param epsg epsg code for the raster
 */
void Rasterdata::setEpsg(int epsg) { this->m_epsg = epsg; }
