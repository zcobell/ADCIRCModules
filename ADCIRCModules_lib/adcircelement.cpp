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

AdcircElement::AdcircElement() { this->m_nodes.resize(this->n()); }

AdcircElement::AdcircElement(int id, AdcircNode *n1, AdcircNode *n2,
                             AdcircNode *n3) {
  this->m_nodes.resize(3);
  this->m_id = id;
  this->m_nodes[0] = n1;
  this->m_nodes[1] = n2;
  this->m_nodes[2] = n3;
}

int AdcircElement::n() const { return this->m_n; }

void AdcircElement::setNode(int i, AdcircNode *node) {
  if (i < this->n())
    this->m_nodes[i] = node;
  return;
}

int AdcircElement::id() const { return this->m_id; }

void AdcircElement::setId(int id) { this->m_id = id; }

AdcircNode *AdcircElement::node(int i) {
  if (i < this->n())
    return this->m_nodes.at(i);
  return nullptr;
}

std::string AdcircElement::toString() {
  return boost::str(boost::format("%11i %3i %11i %11i %11i") % this->id() %
                    this->n() % this->node(0)->id() % this->node(1)->id() %
                    this->node(2)->id());
}
