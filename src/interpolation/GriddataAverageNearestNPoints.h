//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATAAVERAGENEARESTNPOINTS_H_
#define ADCIRCMODULES_SRC_GRIDDATAAVERAGENEARESTNPOINTS_H_

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {

class GriddataAverageNearestNPoints : public GriddataMethod {
 public:
  GriddataAverageNearestNPoints(const Adcirc::Raster::Rasterdata *raster,
                                const GriddataAttribute *attribute,
                                const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;
};

}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAAVERAGENEARESTNPOINTS_H_
