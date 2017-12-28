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
#include "adcircelement.h"
#include "boost/format.hpp"

/**
 * @name AdcircElement::AdcircElement
 * @brief Default constructor
 */
AdcircElement::AdcircElement() {
  this->m_id = 0;
  this->m_nodes.resize(this->n());
}

/**
 * @name AdcircElement::AdcircElement
 * @brief Constructor using references to three AdcircNode objects
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
AdcircElement::AdcircElement(int id, AdcircNode *n1, AdcircNode *n2,
                             AdcircNode *n3) {
  this->m_nodes.resize(3);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @name AdcircElement::setElement
 * @brief Function that sets up the element using three pointers and the element
 * id/label
 * @param id element id/label
 * @param n1 pointer to node 1
 * @param n2 pointer to node 2
 * @param n3 pointer to node 3
 */
void AdcircElement::setElement(int id, AdcircNode *n1, AdcircNode *n2,
                               AdcircNode *n3) {
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

/**
 * @name AdcircElement::n
 * @brief Number of nodes in this element. In this case it is always 3, but this
 * is implemented for future use
 * @return number of nodes in element
 */
int AdcircElement::n() const { return this->m_n; }

/**
 * @name AdcircElement::setNode
 * @brief Sets the node at the specified position to the supplied pointer
 * @param i location in the node vector for this element
 * @param node pointer to an AdcircNode object
 */
void AdcircElement::setNode(int i, AdcircNode *node) {
  if (i < this->n())
    this->m_nodes[i] = node;
  return;
}

/**
 * @name AdcircElement::id
 * @brief Returns the element id/flag
 * @return element id/flag
 */
int AdcircElement::id() const { return this->m_id; }

/**
 * @name AdcircElement::setId
 * @brief Sets the element id/flag
 * @param id element id/flag
 */
void AdcircElement::setId(int id) { this->m_id = id; }

/**
 * @name AdcircElement::node
 * @brief returns a pointer to the node at the specified position
 * @param i position in node vector
 * @return AdcircNode pointer
 */
AdcircNode *AdcircElement::node(int i) {
  if (i < this->n())
    return this->m_nodes.at(i);
  return nullptr;
}

/**
 * @name AdcircElement::toString
 * @brief Formats the element for writing to an Adcirc ASCII mesh file
 * @return formatted string
 */
std::string AdcircElement::toString() {
  return boost::str(boost::format("%11i %3i %11i %11i %11i") % this->id() %
                    this->n() % this->node(0)->id() % this->node(1)->id() %
                    this->node(2)->id());
}
