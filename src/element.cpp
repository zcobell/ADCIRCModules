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
#include "element.h"
#include <cmath>
#include "boost/format.hpp"
#include "constants.h"
#include "error.h"

using namespace std;
using namespace Adcirc::Geometry;

/**
 * @fn Element::Element
 * @brief Default constructor
 */
Element::Element() {
  this->m_id = 0;
  this->resize(3);
}

/**
 * @brief Resizes the internal array of verticies
 * @param nVertex number of verticies. Must be 3 or 4.
 */
void Element::resize(size_t nVertex) {
  if (nVertex != 3 && nVertex != 4) {
    adcircmodules_throw_exception("Invalid number of verticies");
  }
  this->m_nodes.resize(nVertex);
}

/**
 * @brief Constructor using references to three Node objects
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
Element::Element(size_t id, Node *n1, Node *n2, Node *n3) {
  this->resize(3);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @brief Constructor using references to three Node objects
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 * @param n4 pointer to node 4
 */
Element::Element(size_t id, Node *n1, Node *n2, Node *n3, Node *n4) {
  this->resize(4);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
  this->m_nodes[3] = n4;
}

/**
 * @brief Function that sets up the element using three pointers and the element
 * id/label
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
void Element::setElement(size_t id, Node *n1, Node *n2, Node *n3) {
  this->m_id = id;
  this->m_nodes.resize(3);
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @brief Function that sets up the element using three pointers and the element
 * id/label
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 * @param n4 pointer to node 4
 */
void Element::setElement(size_t id, Node *n1, Node *n2, Node *n3, Node *n4) {
  this->m_id = id;
  this->m_nodes.resize(4);
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
  this->m_nodes[3] = n4;
}

/**
 * @brief Number of verticies in this element
 * @return number of nodes in element
 */
int Element::n() const { return this->m_nodes.size(); }

/**
 * @brief Sets the node at the specified position to the supplied pointer
 * @param i location in the node vector for this element
 * @param node pointer to an Node object
 */
void Element::setNode(int i, Node *node) {
  if (i < this->n()) {
    this->m_nodes[i] = node;
  }
  return;
}

/**
 * @brief Returns the element id/flag
 * @return element id/flag
 */
size_t Element::id() const { return this->m_id; }

/**
 * @brief Sets the element id/flag
 * @param id element id/flag
 */
void Element::setId(size_t id) { this->m_id = id; }

/**
 * @brief returns a pointer to the node at the specified position
 * @param i position in node vector
 * @return Node pointer
 */
Node *Element::node(int i) {
  if (i < this->n()) {
    return this->m_nodes.at(i);
  }
  adcircmodules_throw_exception("Index out of bounds");
  return nullptr;
}

/**
 * @brief Formats the element for writing to an Adcirc ASCII mesh file
 * @return formatted string
 */
string Element::toString() {
  if (this->n() == 3) {
    return boost::str(boost::format("%11i %3i %11i %11i %11i") % this->id() %
                      this->n() % this->node(0)->id() % this->node(1)->id() %
                      this->node(2)->id());
  } else if (this->n() == 4) {
    return boost::str(boost::format("%11i %3i %11i %11i %11i %11i") %
                      this->id() % this->n() % this->node(0)->id() %
                      this->node(1)->id() % this->node(2)->id() %
                      this->node(3)->id());
  } else {
    adcircmodules_throw_exception("Invalid number of nodes in element");
    return string();
  }
}

/**
 * @brief Calculates the average size of the legs of an element
 * @return average element leg size
 */
double Element::elementSize(bool geodesic) {
  double size = 0.0;
  for (int i = 0; i < this->n(); ++i) {
    std::pair<Node *, Node *> p = this->elementLeg(i);
    Node *n1 = p.first;
    Node *n2 = p.second;
    size += Constants::distance(n1->x(), n1->y(), n2->x(), n2->y(), geodesic);
  }
  return size / this->n();
}

/**
 * @brief Returns a pair of nodes representing the leg of an element
 * @param i index of the leg around the element
 * @return pair of nodes
 */
std::pair<Node *, Node *> Element::elementLeg(size_t i) {
  assert(i < this->n());

  size_t j1 = i;
  size_t j2 = i + 1;

  if (j2 == this->n()) {
    j2 = 0;
  }

  Node *np1 = this->node(j1);
  Node *np2 = this->node(j2);

  return std::make_pair(np1, np2);
}

/**
 * @brief Calculates the area of a triangle described by points a, b, and c
 * @param a First point of triangle
 * @param b Second point of triangle
 * @param c Third point of triangle
 * @return Area of triangle
 */
double Element::triangleArea(Point &a, Point &b, Point &c) {
  return std::abs(a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) +
                  c.x() * (a.y() - b.y()));
}

/**
 * @brief Determine if a point lies within an element
 * @param x x-coordinate
 * @param y y-coordinate
 * @return
 */
bool Element::isInside(double x, double y) {
  return this->isInside(Point(x, y));
}

/**
 * @param location Point to check
 * @return
 */
bool Element::isInside(Point location) {
  if (this->n() == 3) {
    return this->isInsideTriangle(location);
  } else if (this->n() == 4) {
    return this->isInsideQuad(location);
  }
  return false;
}

/**
 * @brief Checks if a point lies within a triangle
 * @param location location to check
 * @return true if inside, false if outside
 */
bool Element::isInsideTriangle(Point location) {
  Point a = this->node(0)->toPoint();
  Point b = this->node(1)->toPoint();
  Point c = this->node(2)->toPoint();
  double s1 = this->triangleArea(a, b, location);
  double s2 = this->triangleArea(a, c, location);
  double s3 = this->triangleArea(c, b, location);
  double ta = this->triangleArea(a, b, c);
  return (s1 + s2 + s3 <= 1.001 * ta);
}

/**
 * @brief Checks if a point lies within a quadrilateral
 * @param location location to check
 * @return true if inside, false if outside
 */
bool Element::isInsideQuad(Point location) {
  Point a = this->node(0)->toPoint();
  Point b = this->node(1)->toPoint();
  Point c = this->node(2)->toPoint();
  Point d = this->node(3)->toPoint();
  double s1 = this->triangleArea(a, b, location);
  double s2 = this->triangleArea(b, c, location);
  double s3 = this->triangleArea(c, d, location);
  double s4 = this->triangleArea(d, a, location);
  double ta = this->triangleArea(a, b, c) + this->triangleArea(c, d, a);
  return (s1 + s2 + s3 + s4 <= 1.001 * ta);
}
