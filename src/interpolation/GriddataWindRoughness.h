//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_
#define ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_

#include <array>

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {

class GriddataWindRoughness : public GriddataMethod {
 public:
  GriddataWindRoughness(const Adcirc::Raster::Rasterdata *raster,
                        const GriddataAttribute *attribute,
                        const GriddataConfig *config);

  std::vector<double> computeMultipleFromRaster() const override;

  std::vector<double> computeMultipleFromLookup() const override;

  static constexpr double distanceFactor() { return 1.0 / 1000.0; }

  static constexpr double windRadius() { return 10000.0; }

  static constexpr double windSigma() { return 6.0; }

 private:
  static void computeWeightedDirectionalWindValues(
      const std::array<double, 12> &weight, std::vector<double> &wind,
      double nearWeight);

  static std::tuple<bool, double, char> computeWindDirectionAndWeight(
      const Point &p1, const Point &p2);

  static double gaussian(double distance);

  template <typename T>
  static int sgn(T val) {
    return (T(0) < val) - (val < T(0));
  }
};

}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAWINDROUGHNESS_H_
