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
 * @class Node
 * @author Zachary Cobell
 * @brief The Node class describes the x, y, z position of a single mesh
 * node
 *
 */
#ifndef NODE_H
#define NODE_H

#include <string>
#include "adcirc/adcircmodules_global.h"

namespace Adcirc {
namespace Geometry {

class Node {
 public:
  explicit Node();
  explicit Node(size_t id, double x, double y, double z);

  void setNode(size_t id, double x, double y, double z);

  double x() const;
  void setX(double x);

  double y() const;
  void setY(double y);

  double z() const;
  void setZ(double z);

  size_t id() const;
  void setId(size_t id);

  std::string toString(bool geographicCoordinates);

 private:
  size_t m_id;
  double m_x;
  double m_y;
  double m_z;
};
}
}
#endif  // NODE_H
