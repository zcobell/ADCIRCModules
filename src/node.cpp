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
#include "node.h"
#include "boost/format.hpp"
#include "default_values.h"
#include "hash.h"

using namespace Adcirc::Geometry;

/**
 * @brief Default constructor
 */
Node::Node()
    : m_id(0),
      m_x(adcircmodules_default_value<double>()),
      m_y(adcircmodules_default_value<double>()),
      m_z(adcircmodules_default_value<double>()),
      m_hash(nullptr),
      m_positionHash(std::string()) {}

/**
 * @brief Constructor taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
Node::Node(size_t id, double x, double y, double z)
    : m_id(id),
      m_x(x),
      m_y(y),
      m_z(z),
      m_hash(nullptr),
      m_positionHash(std::string()) {}

Node::~Node() {
  if (this->m_hash != nullptr) delete[] this->m_hash;
}

/**
 * @brief Function taking the id, x, y, and z for the node
 * @param id nodal identifier. Can be either array index or label
 * @param x x positoin
 * @param y y position
 * @param z z elevation
 */
void Node::setNode(size_t id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
  if (this->m_hash != nullptr) this->generateHash();
  return;
}

/**
 * @brief Returns the x-location of the node
 * @return x-location
 */
double Node::x() const { return this->m_x; }

/**
 * @brief Sets the x-location of the node
 * @param x x-location
 */
void Node::setX(double x) { this->m_x = x; }

/**
 * @brief Returns the y-location of the node
 * @return y-location
 */
double Node::y() const { return this->m_y; }

/**
 * @brief Sets the y-location of the node
 * @param y y-location
 */
void Node::setY(double y) { this->m_y = y; }

/**
 * @brief Returns the z-elevation of the node
 * @return y-elevation
 */
double Node::z() const { return this->m_z; }

/**
 * @brief Sets the z-elevation of the node
 * @param z z-location
 */
void Node::setZ(double z) { this->m_z = z; }

/**
 * @brief Returns the nodal id/label
 * @return nodal id/label
 */
size_t Node::id() const { return this->m_id; }

/**
 * @brief Sets the nodal id/label
 * @param id nodal id/label
 */
void Node::setId(size_t id) { this->m_id = id; }

/**
 * @brief Formats the node for writing into an Adcirc ASCII mesh file
 * @param geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
std::string Node::toAdcircString(bool geographicCoordinates) {
  if (geographicCoordinates) {
    return boost::str(boost::format("%11i   %14.10f   %14.10f  %14.10f") %
                      this->id() % this->x() % this->y() % this->z());
  } else {
    return boost::str(boost::format("%11i   %14.4f   %14.4f  %14.4f") %
                      this->id() % this->x() % this->y() % this->z());
  }
}

/**
 * @brief Formats the node for writing into an Aquaveo 2dm ASCII mesh file
 * @param geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
std::string Node::to2dmString(bool geographicCoordinates) {
  return boost::str(boost::format("%s  %i %14.8e %14.8e %14.8e") % "ND" %
                    this->id() % this->x() % this->y() % this->z());
}

/**
 * @brief Generates a point object from a node
 * @return Point (x,y) using node coordinates
 */
Point Node::toPoint() { return Point(this->m_x, this->m_y); }

/**
 * @brief Returns the hash of this node based upon it's position and elevation
 * @return hash formatted as a string
 *
 * No two adcirc nodes will have an identical hash (assuming
 * there are no hash collisions) since the hash is based upon
 * the node's position and z-elevation
 */
std::string Node::hash(HashType h, bool force) {
  if (this->m_hash == nullptr || force) this->generateHash(h);
  return std::string(this->m_hash);
}

/**
 * @brief Returns the hash of this node based upon it's position
 * @return hash formatted as a string
 *
 * No two adcirc nodes will have an identical hash (assuming
 * there are no hash collisions) since the hash is based upon
 * the node's position. Identical hashes are computed when the
 * z-elevation is the same.
 */
std::string Node::positionHash(HashType h, bool force) {
  if (this->m_positionHash == std::string() || force)
    this->generatePositionHash();
  return this->m_positionHash;
}

/**
 * @brief Generates a hash of the ADCIRC node's attributes
 * @param h type of hash to use
 */
void Node::generateHash(HashType h) {
  Hash hash(h);
  hash.addData(boost::str(boost::format("%16.10f") % this->x()));
  hash.addData(boost::str(boost::format("%16.10f") % this->y()));
  hash.addData(boost::str(boost::format("%16.10f") % this->z()));
  this->m_hash = hash.getHash();
  return;
}

/**
 * @brief Generates a hash of only the node's position
 * @param h type of hash to use
 */
void Node::generatePositionHash(HashType h) {
  Hash hash(h);
  hash.addData(boost::str(boost::format("%16.10f") % this->x()));
  hash.addData(boost::str(boost::format("%16.10f") % this->y()));
  this->m_positionHash = hash.getHash();
  return;
}
