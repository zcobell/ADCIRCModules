//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATAAVERAGE_H_
#define ADCIRCMODULES_SRC_GRIDDATAAVERAGE_H_

#include "GriddataMethod.h"
namespace Adcirc {
namespace Private {

class GriddataAverage : public GriddataMethod {
 public:
  GriddataAverage(const Adcirc::Raster::Rasterdata *raster,
                  const GriddataAttribute *attribute,
                  const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;

};
}  // namespace Private
}  // namespace Adcirc

#endif  // ADCIRCMODULES_SRC_GRIDDATAAVERAGE_H_
