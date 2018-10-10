/*------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------*/
#ifndef ELEMENT_H
#define ELEMENT_H

#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include "adcircmodules_global.h"
#include "node.h"
#include "point.h"

namespace Adcirc {
namespace Geometry {

/**
 * @class Element
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The Element class describes an Element as an array
 * of Node pointers
 *
 */

class Element {
 public:
  explicit Element();
  explicit Element(size_t id, Adcirc::Geometry::Node *n1,
                   Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3);
  explicit Element(size_t id, Adcirc::Geometry::Node *n1,
                   Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3,
                   Adcirc::Geometry::Node *n4);

  void setElement(size_t id, Adcirc::Geometry::Node *n1,
                  Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3);
  void setElement(size_t id, Adcirc::Geometry::Node *n1,
                  Adcirc::Geometry::Node *n2, Adcirc::Geometry::Node *n3,
                  Adcirc::Geometry::Node *n4);

  int n() const;
  void resize(size_t nVertex);

  Adcirc::Geometry::Node *node(int i);
  void setNode(int i, Adcirc::Geometry::Node *node);

  size_t id() const;
  void setId(size_t id);

  std::string toAdcircString();
  std::string to2dmString();

  bool isInside(Point location);
  bool isInside(double x, double y);

  double elementSize(bool geodesic = true);

  void sortVerticiesAboutCenter();

  std::pair<Adcirc::Geometry::Node *, Adcirc::Geometry::Node *> elementLeg(
      size_t i);

  void getElementCenter(double &xc, double &yc);

 private:
  bool isInsideTriangle(Point location);
  bool isInsideQuad(Point location);
  double triangleArea(Point &a, Point &b, Point &c);

  size_t m_id;
  std::vector<Adcirc::Geometry::Node *> m_nodes;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ELEMENT_H
