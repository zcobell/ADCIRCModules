#include "rasterdata.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include "cpl_conv.h"
#include "cpl_error.h"
#include "gdal_priv.h"

Rasterdata::Rasterdata() {
  this->m_filename = std::string();
  this->init();
  return;
}

Rasterdata::Rasterdata(std::string filename) {
  this->m_filename = filename;
  this->init();
  return;
}

Rasterdata::~Rasterdata() { this->close(); }

void Rasterdata::init() {
  this->m_file = nullptr;
  this->m_band = nullptr;
  this->m_isOpen = false;
  this->m_epsg = 4326;
  this->m_nx = std::numeric_limits<int>::min();
  this->m_ny = std::numeric_limits<int>::min();
  this->m_nodata = std::numeric_limits<double>::min();
  return;
}

bool Rasterdata::open() {
  GDALAllRegister();
  this->m_file = (GDALDataset *)GDALOpen(this->m_filename.c_str(), GA_ReadOnly);
  if (this->m_file == nullptr) {
    return false;
  } else {
    this->m_isOpen = true;
    return this->getRasterMetadata();
  }
}

bool Rasterdata::getRasterMetadata() {
  int successNoData;
  assert(this->m_file != nullptr);

  this->m_band = this->m_file->GetRasterBand(1);
  this->m_nodata = this->m_band->GetNoDataValue(&successNoData);

  if (!successNoData) {
    this->m_nodata = std::numeric_limits<double>::min();
  }

  GDALDataType d = this->m_band->GetRasterDataType();
  this->m_rasterType = this->selectRasterType(d);

  double adfGeoTransform[6];
  if (this->m_file->GetGeoTransform(adfGeoTransform) == CE_None) {
    this->m_nx = (size_t)this->m_file->GetRasterXSize();
    this->m_ny = (size_t)this->m_file->GetRasterYSize();
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

int Rasterdata::rasterType() const { return this->m_rasterType; }

std::string Rasterdata::filename() const { return this->m_filename; }

void Rasterdata::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

bool Rasterdata::close() {
  if (this->m_file != nullptr) {
    GDALClose((GDALDatasetH)this->m_file);
    this->m_isOpen = false;
    return true;
  }
  return false;
}

Point Rasterdata::pixelToCoordinate(size_t i, size_t j) {
  return Point(
      static_cast<double>(i) * this->m_dx + this->m_xmin + 0.50 * this->m_dx,
      this->m_ymax - (static_cast<double>(j) * this->m_dy) + 0.50 * this->m_dy);
}

Point Rasterdata::pixelToCoordinate(Pixel &p) {
  return this->pixelToCoordinate(p.i(), p.j());
}

Pixel Rasterdata::coordinateToPixel(double x, double y) {
  if (x > this->xmax() || x < this->xmin() || y > this->ymax() || y < ymin()) {
    return Pixel();
  } else {
    return Pixel(static_cast<int>(std::round((x - this->m_xmin) / this->m_dx)),
                 static_cast<int>(std::round((this->m_ymax - y) / this->m_dy)));
  }
}

Pixel Rasterdata::coordinateToPixel(Point &p) {
  return this->coordinateToPixel(p.x(), p.y());
}

double Rasterdata::ymax() const { return this->m_ymax; }

double Rasterdata::ymin() const { return this->m_ymin; }

double Rasterdata::xmax() const { return this->m_xmax; }

double Rasterdata::xmin() const { return this->m_xmin; }

double Rasterdata::dy() const { return this->m_dy; }

double Rasterdata::dx() const { return this->m_dx; }

double Rasterdata::nodata() const { return this->m_nodata; }

size_t Rasterdata::ny() const { return this->m_ny; }

size_t Rasterdata::nx() const { return this->m_nx; }

std::string Rasterdata::projectionString() const {
  return this->m_projectionReference;
}

double Rasterdata::pixelValueDouble(size_t i, size_t j) {
  Pixel p = Pixel(i, j);
  return this->pixelValueDouble(p);
}

int Rasterdata::pixelValueInt(size_t i, size_t j) {
  Pixel p = Pixel(i, j);
  return this->pixelValueInt(p);
}

int Rasterdata::pixelValueInt(Pixel p) {
  int *buf = (int *)CPLMalloc(sizeof(int) * 1);
  this->m_band->RasterIO(GF_Read, p.i(), p.j() - 1, 1, 1, buf, 1, 1, GDT_Int32,
                         0, 0);
  int v = buf[0];
  CPLFree(buf);
  return v;
}

double Rasterdata::pixelValueDouble(Pixel &p) {
  double *buf = (double *)CPLMalloc(sizeof(double) * 1);
  this->m_band->RasterIO(GF_Read, p.i(), p.j() - 1, 1, 1, buf, 1, 1,
                         GDT_Float64, 0, 0);
  double v = buf[0];
  CPLFree(buf);
  return v;
}

int Rasterdata::searchBoxAroundPoint(double x, double y, double width,
                                     Pixel &upperLeft, Pixel &lowerRight) {
  Pixel p = this->coordinateToPixel(x, y);
  if (p.isValid()) {
    size_t nx = static_cast<size_t>(std::round(width / this->m_dx));
    size_t ny = static_cast<size_t>(std::round(width / this->m_dy));
    size_t ibegin = std::max<size_t>(0, p.i() - nx);
    size_t iend = std::min<size_t>(this->m_nx, p.i() + nx);
    size_t jbegin = std::max<size_t>(0, p.j() - ny);
    size_t jend = std::min<size_t>(this->m_ny, p.j() + ny);
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

int Rasterdata::pixelValues(size_t ibegin, size_t jbegin, size_t iend,
                            size_t jend, std::vector<double> &x,
                            std::vector<double> &y, std::vector<double> &z) {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;
  double *buf = (double *)CPLMalloc(sizeof(double) * n);
  CPLErr e = this->m_band->RasterIO(GF_Read, ibegin, jbegin - 1, nx, ny, buf,
                                    nx, ny, GDT_Float64, 0, 0);

  if (x.size() != n) {
    x.resize(n);
    y.resize(n);
    z.resize(n);
  }

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      x[k] = p.x();
      y[k] = p.y();
      z[k] = buf[k];
      k++;
    }
  }
  CPLFree(buf);
  return static_cast<int>(e);
}

int Rasterdata::pixelValues(size_t ibegin, size_t jbegin, size_t iend,
                            size_t jend, std::vector<double> &x,
                            std::vector<double> &y, std::vector<int> &z) {
  size_t nx = iend - ibegin + 1;
  size_t ny = jend - jbegin + 1;
  size_t n = nx * ny;
  int *buf = (int *)CPLMalloc(sizeof(int) * n);
  CPLErr e = this->m_band->RasterIO(GF_Read, ibegin, jbegin - 1, nx, ny, buf,
                                    nx, ny, GDT_Int32, 0, 0);

  if (x.size() != n) {
    x.resize(n);
    y.resize(n);
    z.resize(n);
  }

  size_t k = 0;
  for (size_t j = jbegin; j <= jend; ++j) {
    for (size_t i = ibegin; i <= iend; ++i) {
      Point p = this->pixelToCoordinate(i, j);
      x[k] = p.x();
      y[k] = p.y();
      z[k] = buf[k];
      k++;
    }
  }
  CPLFree(buf);
  return static_cast<int>(e);
}

Rasterdata::RasterTypes Rasterdata::selectRasterType(int d) {
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

bool Rasterdata::isOpen() const { return this->m_isOpen; }

int Rasterdata::epsg() const { return this->m_epsg; }

void Rasterdata::setEpsg(int epsg) { this->m_epsg = epsg; }
