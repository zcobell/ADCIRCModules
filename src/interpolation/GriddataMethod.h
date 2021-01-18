//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATAMETHOD_H_
#define ADCIRCMODULES_SRC_GRIDDATAMETHOD_H_

#include <cassert>
#include <limits>
#include <vector>

#include "Constants.h"
#include "GriddataAttribute.h"
#include "GriddataConfig.h"
#include "Logging.h"
#include "PixelValueVector.h"
#include "Point.h"
#include "RasterData.h"

namespace Adcirc {
namespace Private {

class GriddataMethod {
 public:
  GriddataMethod(const Adcirc::Raster::Rasterdata *raster,
                 const GriddataAttribute *attribute,
                 const GriddataConfig *config);

  const Adcirc::Private::GriddataAttribute *attribute() const;

  const Adcirc::Raster::Rasterdata *raster() const;

  const GriddataConfig *config() const;

  static constexpr double methodErrorValue() {
    return -std::numeric_limits<double>::max();
  }

  double compute() const;

  std::vector<double> computeMultiple() const;

 protected:
  virtual double computeFromRaster() const { return config()->defaultValue(); }

  virtual double computeFromLookup() const { return config()->defaultValue(); }

  virtual std::vector<double> computeMultipleFromRaster() const {
    return std::vector<double>(12, config()->defaultValue());
  };

  virtual std::vector<double> computeMultipleFromLookup() const {
    return std::vector<double>(12, config()->defaultValue());
  }

  template <typename T>
  Adcirc::PixelValueVector<T> pixelDataInSpecifiedRadius(double radius) const {
    Adcirc::Raster::Pixel ul, lr;
    this->m_raster->searchBoxAroundPoint(
        attribute()->point().x(), attribute()->point().y(), radius, ul, lr);

    if (ul.isValid() && lr.isValid()) {
      auto values = m_raster->pixelValues<T>(ul.i(), ul.j(), lr.i(), lr.j());
      int outCode = 1;
      for (auto &v : values.pixels()) {
        if (v.valid()) {
          if (Adcirc::Constants::distance(attribute()->point(), v.location()) >
              radius) {
            v.setValid(false);
          } else {
            outCode = 0;
          }
        }
      }
      values.setCode(outCode);

      if (this->config()->thresholdMethod() !=
          Interpolation::Threshold::NoThreshold) {
        this->thresholdData(values);
      }
      return values;
    }
    return PixelValueVector<T>();
  }

  template <typename T>
  Adcirc::PixelValueVector<T> pixelDataInRadius() const {
    return this->pixelDataInSpecifiedRadius<T>(attribute()->queryResolution());
  }

  template <typename T>
  void thresholdData(Adcirc::PixelValueVector<T> &values) const {
    if (std::is_same<T, int>::value)
      adcircmodules_throw_exception(
          "Cannot use thresholding and integer rasters");

    if (this->config()->thresholdMethod() ==
        Interpolation::Threshold::NoThreshold)
      return;

    if (this->config()->thresholdMethod() ==
        Interpolation::Threshold::ThresholdAbove) {
      for (auto &v : values.pixels()) {
        if (!v.valid()) continue;
        auto zz = v.value() * this->config()->rasterMultiplier() +
                  this->config()->datumShift();
        if (zz < this->config()->thresholdValue()) {
          v.setValue(this->config()->defaultValue());
          v.setValid(false);
        }
      }
    } else if (this->config()->thresholdMethod() ==
               Interpolation::Threshold::ThresholdBelow) {
      for (auto &v : values.pixels()) {
        if (!v.valid()) continue;
        auto zz = v.value() * this->config()->rasterMultiplier() +
                  this->config()->datumShift();
        if (zz > this->config()->thresholdValue()) {
          v.setValue(this->config()->defaultValue());
          v.setValid(false);
        }
      }
    }
  }

  double calculateExpansionLevelForPoints(size_t n) const {
    //...This tries to build out a box around a point at the
    // resolution of the raster. The hope is that by going 2
    // additional levels outside of what would be needed for the
    // requested number of points, we'll always hit the request
    // unless we're in a severe nodata region
    int levels = static_cast<int>(std::floor(static_cast<double>(n) / 8.0)) + 2;
    return raster()->dx() * static_cast<double>(levels);
  }

  static bool sortPointsByIncreasingDistance(
      const std::tuple<double, double> &a,
      const std::tuple<double, double> &b) {
    return std::get<0>(a) < std::get<0>(b);
  }

 private:
  const Adcirc::Raster::Rasterdata *m_raster;
  const Adcirc::Private::GriddataAttribute *m_attribute;
  const GriddataConfig *m_config;
};
}  // namespace Private
}  // namespace Adcirc

#endif  // ADCIRCMODULES_SRC_GRIDDATAMETHOD_H_
