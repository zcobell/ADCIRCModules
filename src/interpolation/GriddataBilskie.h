//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATABILSKIE_H_
#define ADCIRCMODULES_SRC_GRIDDATABILSKIE_H_

#include <utility>

#include "GriddataMethod.h"

namespace Adcirc {
namespace Private {

class GriddataBilskie : public GriddataMethod {
 public:
  GriddataBilskie(const Adcirc::Raster::Rasterdata *raster,
                  const GriddataAttribute *attribute,
                  const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;

 private:
  bool useBilskieRadius() const;
};
}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATABILSKIE_H_
