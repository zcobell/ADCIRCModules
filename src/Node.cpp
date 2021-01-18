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
#include "Node.h"

#include "AdcHash.h"
#include "DefaultValues.h"
#include "FPCompare.h"
#include "boost/format.hpp"

using namespace Adcirc::Geometry;

/**
 * @brief Default constructor
 */
Node::Node()
    : m_id(0),
      m_hash(nullptr),
      m_positionHash(nullptr),
      m_position{adcircmodules_default_value<double>(),
                 adcircmodules_default_value<double>(),
                 adcircmodules_default_value<double>()},
      m_isBoundaryNode(false) {}

/**
 * @brief Constructor taking the id, x, y, and z for the node
 * @param[in] id nodal identifier. Can be either array index or label
 * @param[in] x x positoin
 * @param[in] y y position
 * @param[in] z z elevation
 */
Node::Node(size_t id, double x, double y, double z)
    : m_id(id),
      m_hash(nullptr),
      m_positionHash(nullptr),
      m_position{x, y, z},
      m_isBoundaryNode(false) {}

/**
 * @brief Copies a Node object
 * @param[inout] a target for Node copy
 * @param[in] b Node to be copied
 */
void Node::nodeCopier(Node *a, const Node *b) {
  a->m_id = b->id();
  a->m_position = {b->x(), b->y(), b->z()};
  a->m_hash.reset(nullptr);
  a->m_positionHash.reset(nullptr);
  a->m_isBoundaryNode = b->isBoundaryNode();
}

/**
 * @brief Copy constructor
 * @param n copied Node
 */
Node::Node(const Node &n) { Node::nodeCopier(this, &n); }

/**
 * @brief Copy assignment operator
 * @param[in] n node to copy
 * @return reference to copied node
 */
Node &Node::operator=(const Node &n) {
  Node::nodeCopier(this, &n);
  return *this;
}

bool Node::operator==(const Node &n) {
  return n.x() == this->x() && n.y() == this->y() && n.z() == this->z();
}

bool Node::operator==(const Node *n) {
  return n->x() == this->x() && n->y() == this->y() && n->z() == this->z();
}

/**
 * @brief Function taking the id, x, y, and z for the node
 * @param[in] id nodal identifier. Can be either array index or label
 * @param[in] x x positoin
 * @param[in] y y position
 * @param[in] z z elevation
 */
void Node::setNode(size_t id, double x, double y, double z) {
  this->m_id = id;
  this->m_position = {x, y, z};
  if (this->m_hash.get() != nullptr) this->generateHash();
  return;
}

/**
 * @brief Returns the x-location of the node
 * @return x-location
 */
double Node::x() const { return this->m_position[0]; }

/**
 * @brief Sets the x-location of the node
 * @param[in] x x-location
 */
void Node::setX(double x) { this->m_position[0] = x; }

/**
 * @brief Returns the y-location of the node
 * @return y-location
 */
double Node::y() const { return this->m_position[1]; }

/**
 * @brief Sets the y-location of the node
 * @param[in] y y-location
 */
void Node::setY(double y) { this->m_position[1] = y; }

/**
 * @brief Returns the z-elevation of the node
 * @return y-elevation
 */
double Node::z() const { return this->m_position[2]; }

/**
 * @brief Sets the z-elevation of the node
 * @param[in] z z-location
 */
void Node::setZ(double z) { this->m_position[2] = z; }

/**
 * @brief Returns the nodal id/label
 * @return nodal id/label
 */
size_t Node::id() const { return this->m_id; }

/**
 * @brief Sets the nodal id/label
 * @param[in] id nodal id/label
 */
void Node::setId(size_t id) { this->m_id = id; }

/**
 * @brief Formats the node for writing into an Adcirc ASCII mesh file
 * @param[in] geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
std::string Node::toAdcircString(bool geographicCoordinates) {
  return boost::str(boost::format("%11i   %16.10f   %16.10f  %16.10f") %
                    this->id() % this->x() % this->y() % this->z());
}

/**
 * @brief Formats the node for writing into an Aquaveo 2dm ASCII mesh file
 * @param[in] geographicCoordinates determins if the node is in geographic or
 * cartesian coordinate for significant figures
 * @return formatted string
 */
std::string Node::to2dmString(bool geographicCoordinates) {
  return boost::str(boost::format("%s  %i %16.10e %16.10e %16.10e") % "ND" %
                    this->id() % this->x() % this->y() % this->z());
}

/**
 * @brief Generates a point object from a node
 * @return Point (x,y) using node coordinates
 */
Adcirc::Point Node::toPoint() {
  return Adcirc::Point(this->m_position[0], this->m_position[1]);
}

/**
 * @brief Returns the hash of this node based upon it's position and elevation
 * @return hash formatted as a string
 *
 * No two adcirc nodes will have an identical hash (assuming
 * there are no hash collisions) since the hash is based upon
 * the node's position and z-elevation
 */
std::string Node::hash(Adcirc::Cryptography::HashType h, bool force) {
  if (this->m_hash.get() == nullptr || force) this->generateHash(h);
  return std::string(this->m_hash.get());
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
std::string Node::positionHash(Adcirc::Cryptography::HashType h, bool force) {
  if (this->m_positionHash.get() == nullptr || force)
    this->generatePositionHash();
  return std::string(this->m_positionHash.get());
}

/**
 * @brief Generates a hash of the ADCIRC node's attributes
 * @param[in] h type of hash to use
 */
void Node::generateHash(Adcirc::Cryptography::HashType h) {
  Adcirc::Cryptography::Hash hash(h);
  hash.addData(boost::str(boost::format("%16.10f") % this->x()));
  hash.addData(boost::str(boost::format("%16.10f") % this->y()));
  hash.addData(boost::str(boost::format("%16.10f") % this->z()));
  this->m_hash.reset(hash.getHash());
  return;
}

/**
 * @brief Generates a hash of only the node's position
 * @param[in] h type of hash to use
 */
void Node::generatePositionHash(Adcirc::Cryptography::HashType h) {
  Adcirc::Cryptography::Hash hash(h);
  hash.addData(boost::str(boost::format("%16.10f") % this->x()));
  hash.addData(boost::str(boost::format("%16.10f") % this->y()));
  this->m_positionHash.reset(hash.getHash());
  return;
}

/**
 * @brief Returns true if a node lies on an imposed boundary condition in the
 * mesh
 * @return true if boundary node
 */
bool Node::isBoundaryNode() const { return m_isBoundaryNode; }

/**
 * @brief Sets the boundary node status
 * @param b true if node is on the boundary
 */
void Node::setIsBoundaryNode(bool b) { m_isBoundaryNode = b; }
