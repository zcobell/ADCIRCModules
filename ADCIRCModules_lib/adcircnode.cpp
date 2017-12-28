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
#include "adcircnode.h"
#include "boost/format.hpp"

/**
 * @name AdcircNode::AdcircNode
 * @brief Default constructor
 */
AdcircNode::AdcircNode() {
  this->m_id = 0;
  this->m_x = -9999.0;
  this->m_y = -9999.0;
  this->m_z = -9999.0;
}

/**
 * @overload AdcircNode::AdcircNode
 * @brief Constructor taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
AdcircNode::AdcircNode(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
}

/**
 * @name AdcircNode::setNode
 * @brief Function taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
void AdcircNode::setNode(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
  return;
}

/**
 * @name AdcircNode::x
 * @brief Returns the x-location of the node
 * @return x-location
 */
double AdcircNode::x() const { return this->m_x; }

/**
 * @name AdcircNode::setX
 * @brief Sets the x-location of the node
 * @param x x-location
 */
void AdcircNode::setX(double x) { this->m_x = x; }

/**
 * @name AdcircNode::y
 * @brief Returns the y-location of the node
 * @return y-location
 */
double AdcircNode::y() const { return this->m_y; }

/**
 * @name AdcircNode::setY
 * @brief Sets the y-location of the node
 * @param y y-location
 */
void AdcircNode::setY(double y) { this->m_y = y; }

/**
 * @name AdcircNode::z
 * @brief Returns the z-elevation of the node
 * @return y-elevation
 */
double AdcircNode::z() const { return this->m_z; }

/**
 * @name AdcircNode::setZ
 * @brief Sets the z-elevation of the node
 * @param z z-location
 */
void AdcircNode::setZ(double z) { this->m_z = z; }

/**
 * @name AdcircNode::id
 * @brief Returns the nodal id/label
 * @return nodal id/label
 */
int AdcircNode::id() const { return this->m_id; }

/**
 * @name AdcircNode::setId
 * @brief Sets the nodal id/label
 * @param id nodal id/label
 */
void AdcircNode::setId(int id) { this->m_id = id; }

/**
 * @name AdcircNode::toString
 * @brief Formats the node for writing into an Adcirc ASCII mesh file
 * @param geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
std::string AdcircNode::toString(bool geographicCoordinates) {
  if (geographicCoordinates)
    return boost::str(boost::format("%11i   %14.10f   %14.10f  %14.10f") %
                      this->id() % this->x() % this->y() % this->z());
  else
    return boost::str(boost::format("%11i   %14.4f   %14.4f  %14.4f") %
                      this->id() % this->x() % this->y() % this->z());
}
