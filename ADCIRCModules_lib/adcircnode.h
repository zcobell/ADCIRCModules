//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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
/**
 * @class AdcircNode
 * @author Zachary Cobell
 * @brief The AdcircNode class describes the x, y, z position of a single mesh
 * node
 *
 */
#ifndef ADCIRCNODE_H
#define ADCIRCNODE_H

#include "adcircmodules_global.h"
#include <string>

class AdcircNode {

public:
  explicit AdcircNode();
  explicit AdcircNode(int id, double x, double y, double z);

  void setNode(int id, double x, double y, double z);

  double x() const;
  void setX(double x);

  double y() const;
  void setY(double y);

  double z() const;
  void setZ(double z);

  int id() const;
  void setId(int id);

  std::string toString(bool geographicCoordinates);

private:
  int m_id;
  double m_x;
  double m_y;
  double m_z;
};

#endif // ADCIRCNODE_H
