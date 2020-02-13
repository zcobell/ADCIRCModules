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
#ifndef LOCATION_H
#define LOCATION_H

#include <cstddef>
#include <vector>
#include "stationinterpolation.h"

class Location {
 public:
  Location();

  double x() const;
  void setX(double x);

  double y() const;
  void setY(double y);

  double measured() const;
  void setMeasured(double measured);

  double modeled() const;
  void setModeled(double modeled);

  double difference() const;

  size_t meshNode(const size_t &index) const;
  void setMeshNode(const size_t &index, const size_t &meshNode);

  double meshWeight(const size_t index) const;
  void setMeshWeight(const size_t index, double weight);

  Adcirc::Output::StationInterpolation::Weight *weighting();

  double movedDist() const;
  void setMovedDist(double movedDist);

private:
  double m_x;
  double m_y;
  double m_measured;
  double m_modeled;
  double m_movedDist;
  Adcirc::Output::StationInterpolation::Weight m_weight;
};

#endif  // LOCATION_H
