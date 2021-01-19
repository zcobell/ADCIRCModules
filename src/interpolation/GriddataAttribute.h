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

#ifndef ADCIRCMODULES_SRC_GRIDDATAATTRIBUTE_H_
#define ADCIRCMODULES_SRC_GRIDDATAATTRIBUTE_H_

#include "InterpolationMethods.h"
#include "Point.h"

namespace Adcirc {
namespace Private {

class GriddataAttribute {
 public:
  GriddataAttribute(Adcirc::Point t_location, double t_filter,
                    double t_resolution, Interpolation::Method t_interp_type,
                    Interpolation::Method t_backup_interp)
      : m_point(t_location),
        m_filterSize(t_filter),
        m_resolution(t_resolution),
        m_interpolationFlag(t_interp_type),
        m_backupFlag(t_backup_interp) {}

  Adcirc::Point point() const { return m_point; }
  void setPoint(const Adcirc::Point &p) { m_point = p; }

  double filterSize() const { return m_filterSize; }
  void setFilterSize(const double size) { m_filterSize = size; }

  double queryResolution() const { return m_resolution * m_filterSize * 0.5; }

  void setResolution(const double resolution) { m_resolution = resolution; }
  double resolution() const { return m_resolution; }

  Adcirc::Interpolation::Method interpolationFlag() const {
    return m_interpolationFlag;
  }
  void setInterpolationFlag(const Adcirc::Interpolation::Method &flag) {
    m_interpolationFlag = flag;
  }

  Adcirc::Interpolation::Method backupFlag() const { return m_backupFlag; }
  void setBackupFlag(const Adcirc::Interpolation::Method flag) {
    m_backupFlag = flag;
  }

 private:
  Adcirc::Point m_point;
  double m_filterSize;
  double m_resolution;
  Adcirc::Interpolation::Method m_interpolationFlag;
  Adcirc::Interpolation::Method m_backupFlag;
};
}  // namespace Private
}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_GRIDDATAATTRIBUTE_H_
