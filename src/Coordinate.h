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

#ifndef ADCIRCMODULES_COORDINATE_H
#define ADCIRCMODULES_COORDINATE_H

namespace Adcirc {
namespace Output {

class Coordinate {
 public:
  Coordinate() : m_longitude(0.0), m_latitude(0.0) {}

  Coordinate(double longitude, double latitude)
      : m_longitude(longitude), m_latitude(latitude) {}

  double longitude() const { return m_longitude; }
  void setLongitude(double longitude) { m_longitude = longitude; }

  double latitude() const { return m_latitude; }
  void setLatitude(double latitude) { m_latitude = latitude; }

 private:
  double m_latitude;
  double m_longitude;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCIRCMODULES_COORDINATE_H
