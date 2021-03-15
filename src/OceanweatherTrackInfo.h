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
#ifndef ADCIRCMODULES_OCEANWEATHERTRACKINFO_H
#define ADCIRCMODULES_OCEANWEATHERTRACKINFO_H

#include "Point.h"

namespace Adcirc {

class OceanweatherTrackInfo {
 public:
  OceanweatherTrackInfo()
      : m_min_cp(1013.0),
        m_max_vel(0.0),
        m_min_cp_loc(Point()),
        m_max_vel_loc(Point()),
        m_radius_to_max_winds(0.0){};

  ~OceanweatherTrackInfo() = default;

  double min_central_pressure() const { return m_min_cp; }
  void set_min_central_pressure(double cp) { m_min_cp = cp; }

  double maximum_wind_velocity() const { return m_max_vel; }
  void set_maximum_wind_velocity(double max_vel) { m_max_vel = max_vel; }

  Point storm_center() const { return m_min_cp_loc; }
  void set_storm_center(const Point &center) { m_min_cp_loc = center; }

  Point max_velocity_location() const { return m_max_vel_loc; }
  void set_max_velocity_location(const Point &max_vel_loc) {
    m_max_vel_loc = max_vel_loc;
  }

  double radius_to_max_winds() const { return m_radius_to_max_winds; }
  void set_radius_to_max_winds(double radius) {
    m_radius_to_max_winds = radius;
  }

 private:
  double m_min_cp;
  double m_max_vel;
  Point m_min_cp_loc;
  Point m_max_vel_loc;
  double m_radius_to_max_winds;
};

}  // namespace Adcirc

#endif  // ADCIRCMODULES_OCEANWEATHERTRACKINFO_H
