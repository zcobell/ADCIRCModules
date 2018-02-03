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
#include "adcirc/geometry/node.h"
#include "boost/format.hpp"

using namespace Adcirc::Geometry;

/**
 * @name Node::Node
 * @brief Default constructor
 */
Node::Node() {
  this->m_id = 0;
  this->m_x = -9999.0;
  this->m_y = -9999.0;
  this->m_z = -9999.0;
}

/**
 * @overload Node::Node
 * @brief Constructor taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
Node::Node(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
}

/**
 * @name Node::setNode
 * @brief Function taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
void Node::setNode(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
  return;
}

/**
 * @name Node::x
 * @brief Returns the x-location of the node
 * @return x-location
 */
double Node::x() const { return this->m_x; }

/**
 * @name Node::setX
 * @brief Sets the x-location of the node
 * @param x x-location
 */
void Node::setX(double x) { this->m_x = x; }

/**
 * @name Node::y
 * @brief Returns the y-location of the node
 * @return y-location
 */
double Node::y() const { return this->m_y; }

/**
 * @name Node::setY
 * @brief Sets the y-location of the node
 * @param y y-location
 */
void Node::setY(double y) { this->m_y = y; }

/**
 * @name Node::z
 * @brief Returns the z-elevation of the node
 * @return y-elevation
 */
double Node::z() const { return this->m_z; }

/**
 * @name Node::setZ
 * @brief Sets the z-elevation of the node
 * @param z z-location
 */
void Node::setZ(double z) { this->m_z = z; }

/**
 * @name Node::id
 * @brief Returns the nodal id/label
 * @return nodal id/label
 */
int Node::id() const { return this->m_id; }

/**
 * @name Node::setId
 * @brief Sets the nodal id/label
 * @param id nodal id/label
 */
void Node::setId(int id) { this->m_id = id; }

/**
 * @name Node::toString
 * @brief Formats the node for writing into an Adcirc ASCII mesh file
 * @param geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
string Node::toString(bool geographicCoordinates) {
  if (geographicCoordinates)
    return boost::str(boost::format("%11i   %14.10f   %14.10f  %14.10f") %
                      this->id() % this->x() % this->y() % this->z());
  else
    return boost::str(boost::format("%11i   %14.4f   %14.4f  %14.4f") %
                      this->id() % this->x() % this->y() % this->z());
}
