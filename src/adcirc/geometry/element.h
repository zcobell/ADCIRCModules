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
 * @class Element
 * @author Zachary Cobell
 * @brief The Element class describes an AdcircElement as an array
 * of Node pointers
 *
 */
#ifndef ELEMENT_H
#define ELEMENT_H

#include "adcirc/adcircmodules_global.h"
#include "adcirc/geometry/element.h"
#include "adcirc/geometry/node.h"
#include <string>
#include <vector>

namespace Adcirc {
namespace Geometry {

class Element {
public:
  explicit Element();
  explicit Element(int id, Adcirc::Geometry::Node *n1,
                   Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3);

  void setElement(int id, Adcirc::Geometry::Node *n1,
                  Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3);

  int n() const;

  Adcirc::Geometry::Node *node(int i);
  void setNode(int i, Adcirc::Geometry::Node *node);

  int id() const;
  void setId(int id);

  std::string toString();

private:
  int m_n = 3;

  int m_id;
  std::vector<Adcirc::Geometry::Node *> m_nodes;
};
}
}

#endif // ELEMENT_H
