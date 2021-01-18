//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATASTANDARDDEVIATION_H_
#define ADCIRCMODULES_SRC_GRIDDATASTANDARDDEVIATION_H_

#include "GriddataMethod.h"
namespace Adcirc {
namespace Private {

class GriddataStandardDeviation : public GriddataMethod {
 public:
  GriddataStandardDeviation(const Adcirc::Raster::Rasterdata *raster,
                            const GriddataAttribute *attribute,
                            const GriddataConfig *config);

  double computeFromRaster() const override;

  double computeFromLookup() const override;

 private:
  static double doAverageOutsideStandardDeviation(const std::vector<double> &v,
                                                  int n);
  const double m_n = 2.0;
};
}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATASTANDARDDEVIATION_H_
