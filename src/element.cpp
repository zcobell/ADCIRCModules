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
#include "boost/format.hpp"
#include "error.h"

using namespace std;
using namespace Adcirc::Geometry;

/**
 * @name Element::Element
 * @brief Default constructor
 */
Element::Element() {
  this->m_id = 0;
  this->m_nodes.resize(this->n());
}

/**
 * @name Element::Element
 * @brief Constructor using references to three Node objects
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
Element::Element(size_t id, Node *n1, Node *n2, Node *n3) {
  this->m_nodes.resize(3);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @name Element::setElement
 * @brief Function that sets up the element using three pointers and the element
 * id/label
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
void Element::setElement(size_t id, Node *n1, Node *n2, Node *n3) {
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @name Element::n
 * @brief Number of nodes in this element. In this case it is always 3, but this
 * is implemented for future use
 * @return number of nodes in element
 */
int Element::n() const { return this->m_n; }

/**
 * @name Element::setNode
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
 * @name Element::id
 * @brief Returns the element id/flag
 * @return element id/flag
 */
size_t Element::id() const { return this->m_id; }

/**
 * @name Element::setId
 * @brief Sets the element id/flag
 * @param id element id/flag
 */
void Element::setId(size_t id) { this->m_id = id; }

/**
 * @name Element::node
 * @brief returns a pointer to the node at the specified position
 * @param i position in node vector
 * @return Node pointer
 */
Node *Element::node(int i) {
  if (i < this->n()) {
    return this->m_nodes.at(i);
  }
  Adcirc::Error::throwError("Index out of bounds");
  return nullptr;
}

/**
 * @name Element::toString
 * @brief Formats the element for writing to an Adcirc ASCII mesh file
 * @return formatted string
 */
string Element::toString() {
  return boost::str(boost::format("%11i %3i %11i %11i %11i") % this->id() %
                    this->n() % this->node(0)->id() % this->node(1)->id() %
                    this->node(2)->id());
}
