//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_
#define ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {
class GriddataInverseDistanceWeightedNPoints : public GriddataMethod {
 public:
  GriddataInverseDistanceWeightedNPoints(
      const Adcirc::Raster::Rasterdata *raster,
      const GriddataAttribute *attribute, const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;

 private:
  static double computeInverseDistanceValue(
      std::vector<std::tuple<double, double>> &pts, size_t maxPoints);
};
}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAINVERSEDISTANCEWEIGHTEDNPOINTS_H_
