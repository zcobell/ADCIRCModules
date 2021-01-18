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
#include "Element.h"

#include <cmath>

#include "AdcHash.h"
#include "Constants.h"
#include "Logging.h"
#include "boost/format.hpp"
#include "boost/geometry.hpp"

using namespace Adcirc::Geometry;
namespace bg = boost::geometry;
typedef bg::model::point<double, 2, bg::cs::cartesian> point_t;
typedef bg::model::polygon<point_t> polygon_t;

/**
 * @brief Default constructor
 */
Element::Element()
    : m_id(std::numeric_limits<size_t>::max()),
      m_nodes{nullptr, nullptr, nullptr},
      m_hash(nullptr) {}

/**
 * @brief Constructor using references to three Node objects
 * @param[in] id element id/label
 * @param[in] n1 pointer to node 1
 * @param[in] n2 pointer to node 2
 * @param[in] n3 pointer to node 3
 */
Element::Element(size_t id, Node *n1, Node *n2, Node *n3)
    : m_id(id), m_nodes{n1, n2, n3}, m_hash(nullptr) {}

/**
 * @brief Constructor using references to three Node objects
 * @param[in] id element id/label
 * @param[in] n1 pointer to node 1
 * @param[in] n2 pointer to node 2
 * @param[in] n3 pointer to node 3
 * @param[in] n4 pointer to node 4
 */
Element::Element(size_t id, Node *n1, Node *n2, Node *n3, Node *n4)
    : m_id(id), m_nodes{n1, n2, n3, n4}, m_hash(nullptr) {}

/**
 * @brief Element::elementCopier
 * @param[inout] a element pointer to copy into
 * @param[in] b element to copy
 */
void Element::elementCopier(Element *a, const Element *b) {
  a->m_id = b->id();
  a->m_nodes.resize(b->n());
  for (size_t i = 0; i < b->n(); ++i) {
    a->m_nodes[i] = b->node(i);
  }
}

/**
 * @brief Copy constructor
 * @param[in] e element to copy
 */
Element::Element(const Element &e) { Element::elementCopier(this, &e); }

/**
 * @brief Copy assignment operator
 * @param[in] e element to copy
 * @return copied element reference
 */
Element &Element::operator=(const Element &e) {
  Element::elementCopier(this, &e);
  return *this;
}

bool Element::operator==(const Element &e) {
  if (this->n() != e.n()) return false;
  for (size_t i = 0; i < this->n(); ++i) {
    if (e.node(i) != this->node(i)) return false;
  }
  return true;
}

bool Element::operator==(const Element *e) {
  if (this->n() != e->n()) return false;
  for (size_t i = 0; i < this->n(); ++i) {
    if (e->node(i) != this->node(i)) return false;
  }
  return true;
}

/**
 * @brief Resizes the internal array of verticies
 * @param[in] nVertex number of verticies. Must be 3 or 4.
 */
void Element::resize(size_t nVertex) {
  if (nVertex != 3 && nVertex != 4) {
    adcircmodules_throw_exception("Invalid number of verticies");
  }
  this->m_nodes.resize(nVertex);
}

/**
 * @brief Function that sets up the element using three pointers and the
 * element id/label
 * @param[in] id element id/label
 * @param[in] n1 pointer to node 1
 * @param[in] n2 pointer to node 2
 * @param[in] n3 pointer to node 3
 */
void Element::setElement(size_t id, Node *n1, Node *n2, Node *n3) {
  this->m_id = id;
  this->m_nodes = {n1, n2, n3};
}

/**
 * @brief Function that sets up the element using three pointers and the
 * element id/label
 * @param[in] id element id/label
 * @param[in] n1 pointer to node 1
 * @param[in] n2 pointer to node 2
 * @param[in] n3 pointer to node 3
 * @param[in] n4 pointer to node 4
 */
void Element::setElement(size_t id, Node *n1, Node *n2, Node *n3, Node *n4) {
  this->m_id = id;
  this->m_nodes = {n1, n2, n3, n4};
}

/**
 * @brief Number of verticies in this element
 * @return number of nodes in element
 */
size_t Element::n() const { return this->m_nodes.size(); }

/**
 * @brief Sets the node at the specified position to the supplied pointer
 * @param[in] i location in the node vector for this element
 * @param[in] node pointer to an Node object
 */
void Element::setNode(size_t i, Node *node) {
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
 * @param[in] id element id/flag
 */
void Element::setId(size_t id) { this->m_id = id; }

/**
 * @brief returns a pointer to the node at the specified position
 * @param[in] i position in node vector
 * @return Node pointer
 */
Node *Element::node(size_t i) const {
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
std::string Element::toAdcircString() const {
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
    return std::string();
  }
}

/**
 * @brief Formats the element for writing to an Aquaveo 2dm ASCII mesh file
 * @return formatted string
 */
std::string Element::to2dmString() const {
  if (this->n() == 3) {
    return boost::str(boost::format("%s %i %i %i %i %i") % "E3T" % this->id() %
                      this->node(0)->id() % this->node(1)->id() %
                      this->node(2)->id() % 0);
  } else if (this->n() == 4) {
    return boost::str(boost::format("%s %i %i %i %i %i") % "E4Q" % this->id() %
                      this->node(0)->id() % this->node(1)->id() %
                      this->node(2)->id() % this->node(3)->id());
  } else {
    adcircmodules_throw_exception("Invalid number of nodes in element");
    return std::string();
  }
}

/**
 * @brief Calculates the average size of the legs of an element
 * @return average element leg size
 */
double Element::elementSize(bool geodesic) const {
  double size = 0.0;
  for (size_t i = 0; i < this->n(); ++i) {
    std::pair<Node *, Node *> p = this->elementLeg(i);
    Node *n1 = p.first;
    Node *n2 = p.second;
    size += Constants::distance(n1->x(), n1->y(), n2->x(), n2->y(), geodesic);
  }
  return size / this->n();
}

/**
 * @brief Sorts the verticies in clockwise order around the element center
 */
void Element::sortVerticesAboutCenter(bool clockwise) {
  double xc = 0.0, yc = 0.0;
  this->getElementCenter(xc, yc);

  auto compareClockwise = [&](Node *a, Node *b) -> bool {
    if (a->x() - xc >= 0.0 && b->x() - xc < 0.0) return true;
    if (a->x() - xc < 0.0 && b->x() - xc >= 0) return false;
    if (a->x() - xc == 0.0 && b->x() - xc == 0.0) {
      if (a->y() - yc >= 0 || b->y() - yc >= 0) return a->y() > b->y();
      return b->y() > a->y();
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    double det = (a->x() - xc) * (b->y() - yc) - (b->x() - xc) * (a->y() - yc);
    if (det < 0.0) return true;
    if (det > 0.0) return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    double d1 = (a->x() - xc) * (a->x() - xc) + (a->y() - yc) * (a->y() - yc);
    double d2 = (b->x() - xc) * (b->x() - xc) + (b->y() - yc) * (b->y() - yc);
    return d1 > d2;
  };

  auto compareAntiClockwise = [&](Node *b, Node *a) -> bool {
    if (a->x() - xc >= 0 && b->x() - xc < 0) return true;
    if (a->x() - xc < 0 && b->x() - xc >= 0) return false;
    if (a->x() - xc == 0.0 && b->x() - xc == 0.0) {
      if (a->y() - yc >= 0.0 || b->y() - yc >= 0.0) return a->y() > b->y();
      return b->y() > a->y();
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    double det = (a->x() - xc) * (b->y() - yc) - (b->x() - xc) * (a->y() - yc);
    if (det < 0) return true;
    if (det > 0) return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    double d1 = (a->x() - xc) * (a->x() - xc) + (a->y() - yc) * (a->y() - yc);
    double d2 = (b->x() - xc) * (b->x() - xc) + (b->y() - yc) * (b->y() - yc);
    return d1 > d2;
  };

  if (clockwise) {
    std::sort(this->m_nodes.begin(), this->m_nodes.end(), compareClockwise);
  } else {
    std::sort(this->m_nodes.begin(), this->m_nodes.end(), compareAntiClockwise);
  }

  return;
}

/**
 * @brief Returns a pair of nodes representing the leg of an element
 * @param[in] i index of the leg around the element
 * @return pair of nodes
 */
std::pair<Node *, Node *> Element::elementLeg(size_t i) const {
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
 * @brief Creates a boost::geometry element from the element
 * @param[in] n vector of node pointers to use to create the geometry
 * @return boost::geometry polygon
 */
polygon_t element2polygon(std::vector<Node *> n) {
  polygon_t a;
  for (auto i : n) {
    bg::append(a, point_t(i->x(), i->y()));
  }
  bg::append(a, point_t(n[0]->x(), n[0]->y()));
  bg::correct(a);
  return a;
}

/**
 * @brief Computes the center position of the element
 * @param[out] xc x-coordinate of element center
 * @param[out] yc y-coordinate of element center
 */
void Element::getElementCenter(double &xc, double &yc) const {
  point_t p;
  bg::centroid(element2polygon(this->m_nodes), p);
  xc = p.get<0>();
  yc = p.get<1>();
  return;
}

/**
 * @brief Calculates the area of the element
 * @return Area of triangle
 */
double Element::area() const {
  return bg::area(element2polygon(this->m_nodes));
}

/**
 * @brief Determine if a point lies within an element
 * @param[in] x x-coordinate
 * @param[in] y y-coordinate
 * @return true if point lies within element, false otherwise
 */
bool Element::isInside(double x, double y) const {
  return bg::covered_by(point_t(x, y), element2polygon(this->m_nodes));
}

/**
 * @param[in] location Point to check
 * @return true if point lies within element, false otherwise
 *
 * This function uses the boost::geometry library to determine
 * if a point lies within an element
 */
bool Element::isInside(Point location) const {
  return bg::covered_by(point_t(location.x(), location.y()),
                        element2polygon(this->m_nodes));
}

/**
 * @brief Returns the interpolation weights for computing the value of a given
 * point inside an element
 * @param[in] x station location
 * @param[in] y station location
 * @return weights vector of interpolation weights for each vertex
 */
std::vector<double> Element::interpolationWeights(double x, double y) const {
  if (this->n() == 3) {
    return this->triangularInterpolation(x, y);
  } else {
    return this->polygonInterpolation(x, y);
  }
}

/**
 * @brief Gets the hash of the element
 * @param[in] h type of cryptographic hash to generate
 * @return hash formatted as a string
 *
 * Element hashes are based upon the nodes that
 * make them up and therefore will change when a node
 * moves its position.
 */
std::string Element::hash(Adcirc::Cryptography::HashType h, bool force) {
  if (this->m_hash.get() == nullptr || force) this->generateHash(h);
  return std::string(this->m_hash.get());
}

/**
 * @brief Returns a list of the faces in the element as a vector of node pairs
 * @return vector of node pairs making up faces
 */
std::vector<std::pair<Node *, Node *>> Element::faces() const {
  std::vector<std::pair<Node *, Node *>> face_list;
  face_list.reserve(this->n());
  for (size_t i = 0; i < this->n() - 1; ++i) {
    face_list.emplace_back(m_nodes[i], m_nodes[i + 1]);
  }
  face_list.emplace_back(m_nodes[n() - 1], m_nodes[0]);
  return face_list;
}

/**
 * @brief Returns a pointer to the internal array of node pointers
 * @return pointer to vector of nodes
 */
std::vector<Adcirc::Geometry::Node *> *Adcirc::Geometry::Element::nodes() {
  return &m_nodes;
}

/**
 * @brief Generates the hash for this element
 * @param[in] h type of cryptographic hash to generate
 */
void Element::generateHash(Adcirc::Cryptography::HashType h) {
  Adcirc::Cryptography::Hash hash(h);
  for (auto &n : this->m_nodes) {
    hash.addData(n->positionHash());
  }
  this->m_hash.reset(hash.getHash());
}

/**
 * @brief Performs a barycentric interpolation
 * @param[in] x station location
 * @param[in] y station location
 * @return weights vector of interpolation weights for each vertex
 */
std::vector<double> Element::triangularInterpolation(double x, double y) const {
  std::vector<double> weights(3);
  std::fill(weights.begin(), weights.end(), 0.0);

  const double x1 = this->node(0)->x();
  const double x2 = this->node(1)->x();
  const double x3 = this->node(2)->x();

  const double y1 = this->node(0)->y();
  const double y2 = this->node(1)->y();
  const double y3 = this->node(2)->y();

  const double denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);

  weights[0] = (((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denom);
  weights[1] = (((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denom);
  weights[2] = (1.0 - weights[0] - weights[1]);

  return weights;
}

/**
 * @brief Performs an interpolation on a polygon using triangles
 *
 * The polygon is broken into triangles using a 5th point at its
 * center. Then, the triangles are checked for which has the
 * query point inside. The new triangles then use a barycentric
 * interpolation to provide interpolation weights. The weight from the center
 * point is weighted equally from all points in the polygon. Note that this
 * should work for n-sided objects that are convex (I think) as long as
 * the element isn't poorly formed. The verticies are sorted around the center
 * to make sure that the ordering is logical, but this is done on a copy of
 * the element so that the parent structure isn't altered.
 *
 * @param[in] x station location
 * @param[in] y station location
 * @return weights vector of interpolation weights for each vertex
 *
 */
std::vector<double> Element::polygonInterpolation(double x, double y) const {
  std::vector<double> weights(3);
  std::fill(weights.begin(), weights.end(), 0.0);

  //...Make copy of this element
  Element e(*this);

  //...Sort the copy around the center
  e.sortVerticesAboutCenter();

  //...Generate a point at the middle of the polygon
  double xm = 0.0;
  double ym = 0.0;
  for (size_t i = 0; i < this->n(); ++i) {
    xm += this->node(i)->x();
    ym += this->node(i)->y();
  }
  xm = xm / this->n();
  ym = ym / this->n();

  Node midpoint(0, xm, ym, 0.0);

  //...Find the sub-triangle that the point resides in
  Element ee;
  for (size_t i = 0; i < this->n(); ++i) {
    size_t i1 = i;
    size_t i2 = i + 1;
    if (i2 > this->n()) i2 = 0;
    Element ec(i, e.node(i1), e.node(i2), &midpoint);
    if (ec.isInside(x, y)) {
      ee = ec;
      break;
    }
  }

  //...Generate the sub-triangle weights
  std::vector<double> w2 = ee.interpolationWeights(x, y);

  //...Rectify the weights with the correct ordering into the global array
  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < this->n(); ++j) {
      if (ee.node(i) == this->node(j)) weights[j] = w2[i];
    }
  }

  //...Add weight from midpoint
  const double w3 = w2[2] / this->n();
  for (size_t i = 0; i < this->n(); ++i) {
    weights[i] += w3;
  }

  return weights;
}
