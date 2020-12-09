#ifndef ADCMOD_PROJECTION_H
#define ADCMOD_PROJECTION_H
//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2020 Zachary Cobell
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
//------------------------------------------------------------------------//
#include <string>
#include <tuple>
#include <vector>

#include "adcircmodules_global.h"

namespace Adcirc {

class Projection {
 public:
  using projection_epsg_result = std::tuple<bool, int, std::string>;

  static std::string ADCIRCMODULES_EXPORT projVersion();

  static bool ADCIRCMODULES_EXPORT containsEpsg(int epsg);

  static std::string ADCIRCMODULES_EXPORT epsgDescription(int epsg);

  static void ADCIRCMODULES_EXPORT
  setProjDatabaseLocation(const std::string &dblocation);

  static std::string ADCIRCMODULES_EXPORT projDatabaseLocation();

  static int ADCIRCMODULES_EXPORT transform(int epsgInput, int epsgOutput,
                                            double x, double y, double &outx,
                                            double &outy, bool &isLatLon);

  static int ADCIRCMODULES_EXPORT transform(int epsgInput, int epsgOutput,
                                            const std::vector<double> &x,
                                            const std::vector<double> &y,
                                            std::vector<double> &outx,
                                            std::vector<double> &outy,
                                            bool &isLatLon);

  static int ADCIRCMODULES_EXPORT
  transform(int epsgInput, int epsgOutput,
            const std::vector<std::pair<double, double>> &points,
            std::vector<std::pair<double, double>> &output, bool &isLatLon);

  static int ADCIRCMODULES_EXPORT cpp(double lambda, double phi, double x,
                                      double y, double &outx, double &outy);
  static int ADCIRCMODULES_EXPORT cpp(double lambda, double phi,
                                      const std::vector<double> &x,
                                      const std::vector<double> &y,
                                      std::vector<double> &outx,
                                      std::vector<double> &outy);

  static int ADCIRCMODULES_EXPORT inverseCpp(double lambda, double phi,
                                             double x, double y, double &outx,
                                             double &outy);

  static int ADCIRCMODULES_EXPORT inverseCpp(double lambda, double phi,
                                             const std::vector<double> &x,
                                             const std::vector<double> &y,
                                             std::vector<double> &outx,
                                             std::vector<double> &outy);

 private:
  static int queryProjDatabase(int epsg, projection_epsg_result &result);
};
}  // namespace Adcirc
#endif  // ADCMOD_PROJECTION_H
