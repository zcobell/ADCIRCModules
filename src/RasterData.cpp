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
#include "RasterData.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>

#include "Logging.h"

using namespace Adcirc::Raster;

//...Template Instantiation
template int Adcirc::Raster::Rasterdata::pixelValue<int>(Pixel &p) const;

template double Adcirc::Raster::Rasterdata::pixelValue<double>(Pixel &p) const;

template Adcirc::PixelValueVector<int>
Adcirc::Raster::Rasterdata::pixelValues<int>(size_t ibegin, size_t jbegin,
                                             size_t iend, size_t jend) const;

template Adcirc::PixelValueVector<double>
Adcirc::Raster::Rasterdata::pixelValues<double>(size_t ibegin, size_t jbegin,
                                                size_t iend, size_t jend) const;

template int Adcirc::Raster::Rasterdata::nodata<int>() const;

template double Adcirc::Raster::Rasterdata::nodata<double>() const;

// Macro to initialize constructors
#define RASTERDATACLASSINIT                                                   \
  m_file(nullptr), m_band(nullptr), m_isOpen(false), m_isRead(false),         \
      m_epsg(4326), m_nx(-std::numeric_limits<size_t>::max()),                \
      m_ny(-std::numeric_limits<size_t>::max()),                              \
      m_xmin(std::numeric_limits<double>::max()),                             \
      m_xmax(-std::numeric_limits<double>::max()),                            \
      m_ymin(std::numeric_limits<double>::max()),                             \
      m_ymax(-std::numeric_limits<double>::max()), m_dx(0.0), m_dy(0.0),      \
      m_nodata(-std::numeric_limits<double>::max()),                          \
      m_nodataint(-std::numeric_limits<int>::max()), m_readType(GDT_Unknown), \
      m_rasterType(RasterTypes::Unknown)

/**
 * @brief Default constructor without a filename
 */
Rasterdata::Rasterdata() : m_filename(std::string()), RASTERDATACLASSINIT {}

/**
 * @brief Default constructor that takes a filename
 * @param filename name of raster file
 */
Rasterdata::Rasterdata(std::string filename)
    : m_filename(std::move(filename)), RASTERDATACLASSINIT {}

/**
 * @brief Destructor
 */
Rasterdata::~Rasterdata() { this->close(); }

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
Adcirc::Point Rasterdata::pixelToCoordinate(size_t i, size_t j) const {
  return i <= m_nx && j <= m_ny
             ? Point(i * this->m_dx + this->m_xmin + 0.50 * this->m_dx,
                     this->m_ymax - (j + 1) * this->m_dy + 0.50 * this->m_dy)
             : Point();
}

/**
 * @brief Calculates the coordinate of the pixel center
 * @param p Pixel object with i,j indices
 * @return std::pair<double,double> object with coordinate information for the
 * pixel
 */
Adcirc::Point Rasterdata::pixelToCoordinate(Pixel &p) const {
  return this->pixelToCoordinate(p.i(), p.j());
}

/**
 * @brief Converts an x,y coordinate to the pixel i,j
 * @param x x-coordinate
 * @param y y-coordinate
 * @return Pixel object with i,j indicies for given x,y
 */
Pixel Rasterdata::coordinateToPixel(double x, double y) const {
  if (x > this->xmax() || x < this->xmin() || y > this->ymax() || y < ymin()) {
    return {};
  } else {
    return {static_cast<size_t>((x - this->m_xmin) / this->m_dx),
            static_cast<size_t>((this->m_ymax - y) / this->m_dy)};
  }
}

/**
 * @brief Converts an x,y coordinate to the pixel i,j
 * @param p std::pair<double,double> object with x,y-coordinates
 * @return Pixel object with i,j indices for given x,y
 */
Pixel Rasterdata::coordinateToPixel(const Point &p) const {
  return this->coordinateToPixel(p.x(), p.y());
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
T Rasterdata::pixelValue(size_t i, size_t j) const {
  Pixel p = Pixel(i, j);
  return this->pixelValue<T>(p);
}

/**
 * @brief Returns the pixel value for the i,j index
 * @param p pixel with indices
 * @return pixel value
 */
template <typename T>
T Rasterdata::pixelValue(Pixel &p) const {
  if (p.i() > 0 && p.j() > 0 && p.i() < this->nx() && p.j() < this->ny()) {
    T buf;
    auto err = CPLErr();

#pragma omp critical
    {
      err = this->m_band->RasterIO(GF_Read, p.i(), p.j(), 1, 1, &buf, 1, 1,
                                   static_cast<GDALDataType>(this->m_readType),
                                   0, 0);
    }
    if (err != 0) return this->nodata<T>();
    return buf;
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
                                     Pixel &upperLeft,
                                     Pixel &lowerRight) const {
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
  this->m_doubleOnDisk.resize(boost::extents[this->ny()][this->nx()]);
#pragma omp critical
  {
    CPLErr e = this->m_band->RasterIO(
        GF_Read, 0, 0, this->nx(), this->ny(), this->m_doubleOnDisk.data(),
        this->nx(), this->ny(), static_cast<GDALDataType>(this->m_readType), 0,
        0);
  }
}

/**
 * @brief Reads the entire raster into memory to avoid disk access
 *
 * Not always the fastest method, but in some cases it is worthwhile
 */
void Rasterdata::readIntegerRasterToMemory() {
  this->m_intOnDisk.resize(boost::extents[this->ny()][this->nx()]);
#pragma omp critical
  {
    CPLErr e = this->m_band->RasterIO(
        GF_Read, 0, 0, this->nx(), this->ny(), this->m_intOnDisk.data(),
        this->nx(), this->ny(), static_cast<GDALDataType>(this->m_readType), 0,
        0);
  }
}

/**
 * @brief Reads the entire raster into memory to avoid disk access
 *
 * Not always the fastest method, but in some cases it is worthwhile
 */
void Rasterdata::read() {
  if (!this->m_isRead) {
    if (this->m_rasterType == RasterTypes::Double) {
      this->readDoubleRasterToMemory();
    } else {
      this->readIntegerRasterToMemory();
    }
    this->m_isRead = true;
  }
}

/**
 * @brief Reads the pixel values for the given search box
 * @param ibegin beginning i-index
 * @param jbegin beginning j-index
 * @param iend ending i-index
 * @param jend ending j-index
 * @return PixelValueVector object with embedded status
 */
template <typename T>
Adcirc::PixelValueVector<T> Rasterdata::pixelValues(const size_t ibegin,
                                                    const size_t jbegin,
                                                    const size_t iend,
                                                    const size_t jend) const {
  if (this->m_isRead) {
    return this->pixelValuesFromMemory<T>(ibegin, jbegin, iend, jend);
  } else {
    return this->pixelValuesFromDisk<T>(ibegin, jbegin, iend, jend);
  }
}

/**
 * @brief Reads the pixel values for the given search box from memory
 * @param ibegin beginning i-index
 * @param jbegin beginning j-index
 * @param iend ending i-index
 * @param jend ending j-index
 * @return PixelValueVector
 */
template <typename T>
Adcirc::PixelValueVector<T> Rasterdata::pixelValuesFromMemory(
    const size_t ibegin, const size_t jbegin, const size_t iend,
    const size_t jend) const {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;

  Adcirc::PixelValueVector<T> values;
  values.reserve(n);

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      T z;
      if (std::is_same<T, int>::value) {
        z = this->m_intOnDisk[j][i];
      } else if (std::is_same<T, double>::value) {
        z = this->m_doubleOnDisk[j][i];
      } else {
        adcircmodules_throw_exception("Rasterdata: Invalid pixel type");
      }
      bool valid = z != this->nodata<T>();
      values.push_back(PixelValue<T>(p, valid, z));
      k++;
    }
  }
  return values;
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
Adcirc::PixelValueVector<T> Rasterdata::pixelValuesFromDisk(size_t ibegin,
                                                            size_t jbegin,
                                                            size_t iend,
                                                            size_t jend) const {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;

  PixelValueVector<T> values;
  values.reserve(n);
  std::vector<T> z(n);

  CPLErr e = CPLErr();

#pragma omp critical
  {
    e = this->m_band->RasterIO(GF_Read, ibegin, jbegin, nx, ny, z.data(), nx,
                               ny, static_cast<GDALDataType>(this->m_readType),
                               0, 0);
  }

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      bool valid = z[k] != this->nodata<T>();
      values.push_back(
          PixelValue<T>(this->pixelToCoordinate(i, j), valid, z[k]));
      k++;
    }
  }
  return values;
}

/**
 * @brief Determines the raster type via the GDAL specified type
 * @param GDAL raster code
 * @return raster type enum
 */
Rasterdata::RasterTypes Rasterdata::selectRasterType(int d) {
  if (d == GDT_Byte) {
    this->m_readType = GDT_Int32;
    return RasterTypes::Integer;
  } else if (d == GDT_UInt16 || d == GDT_UInt32) {
    this->m_readType = GDT_Int32;
    return RasterTypes::Unsigned;
  } else if (d == GDT_Int16 || d == GDT_Int32) {
    this->m_readType = GDT_Int32;
    return RasterTypes::Integer;
  } else if (d == GDT_Float32 || d == GDT_Float64) {
    this->m_readType = GDT_Float64;
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
