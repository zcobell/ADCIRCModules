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
#ifndef ADCMOD_STATIONINTERPOLATION_H
#define ADCMOD_STATIONINTERPOLATION_H

#include <array>
#include <limits>
#include <string>
#include <tuple>

#include "adcircmodules_global.h"
#include "cdate.h"
#include "hmdf.h"
#include "mesh.h"
#include "readoutput.h"
#include "stationinterpolationoptions.h"

namespace Adcirc {
namespace Output {

class StationInterpolation {
 public:
  ADCIRCMODULES_EXPORT StationInterpolation(
      const Adcirc::Output::StationInterpolationOptions &options);

  void ADCIRCMODULES_EXPORT run();

  struct Weight {
    bool found;
    std::array<size_t, 3> node_index;
    std::array<double, 3> weight;
    Weight() : found(false), node_index{0, 0, 0}, weight{0.0, 0.0, 0.0} {}
  };

  double interpScalar(Adcirc::Output::ReadOutput &data, Weight &w,
                      const double positive_direction = -9999.0);

  static double interpolateDryValues(double v1, double w1, double v2, double w2,
                                     double v3, double w3, double defaultVal);

 private:
  void reprojectStationOutput();
  CDate getColdstartDate();
  Adcirc::Geometry::Mesh readMesh(const Adcirc::Output::OutputFormat &filetype);
  Adcirc::Output::Hmdf readStationLocations(bool vector = false);
  Adcirc::Output::Hmdf readStationList(bool vector = false);
  Adcirc::Output::Hmdf copyStationList(Adcirc::Output::Hmdf &list,
                                       const bool vector = false);

  void interpolateTimeSnapToStations(const size_t snap, const bool writeVector,
                                     const CDate &coldstart,
                                     Adcirc::Output::ReadOutput &globalFile);
  double interpScalarFromVector(Adcirc::Output::ReadOutput &data, Weight &w,
                                const double positive_direction = -9999.0);
  double interpScalarFromVectorWithoutFlowDirection(
      Adcirc::Output::ReadOutput &data, Weight &w);
  double interpScalarFromVectorWithFlowDirection(
      Adcirc::Output::ReadOutput &data, Weight &w,
      const double positive_direction);
  double interpDirectionFromVector(Adcirc::Output::ReadOutput &data, Weight &w);

  static std::tuple<double, double> interpVector(
      Adcirc::Output::ReadOutput &data, Weight &w);
  void allocateStationArrays();
  void generateInterpolationWeights(Adcirc::Geometry::Mesh &m);

  CDate dateFromString(const std::string &dateString);

  std::vector<Weight> m_weights;
  Adcirc::Output::StationInterpolationOptions m_options;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_STATIONINTERPOLATION_H
