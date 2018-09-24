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
#include "elementtable.h"
#include "error.h"

using namespace Adcirc::Geometry;

ElementTable::ElementTable() { this->m_mesh = nullptr; }

ElementTable::ElementTable(Mesh *mesh) { this->m_mesh = mesh; }

Adcirc::Geometry::Mesh *ElementTable::mesh() const { return this->m_mesh; }

void ElementTable::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_mesh = mesh;
}

void ElementTable::build() {
  if (this->m_mesh == nullptr) {
    return;
  }
  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    for (int j = 0; j < this->m_mesh->element(i)->n(); ++j) {
      Node *n = this->m_mesh->element(i)->node(j);
      this->m_elementTable[n].push_back(this->m_mesh->element(i));
    }
  }
  return;
}

std::vector<Element *> ElementTable::elementList(Node *n) {
  if (this->m_elementTable.find(n) != this->m_elementTable.end())
    return this->m_elementTable[n];
  else
    Adcirc::Error::throwError("Node pointer not part of mesh");
  return std::vector<Element *>();
}

size_t ElementTable::numElementsAroundNode(Adcirc::Geometry::Node *n) {
  return this->m_elementTable[n].size();
}

size_t ElementTable::numElementsAroundNode(size_t nodeIndex) {
  if (nodeIndex < this->mesh()->numNodes()) {
    return this->m_elementTable[this->mesh()->node(nodeIndex)].size();
  } else {
    Adcirc::Error::throwError("Out of bounds node request");
  }
  return -1;
}

Adcirc::Geometry::Element *ElementTable::elementTable(Adcirc::Geometry::Node *n,
                                                      size_t listIndex) {
  if (listIndex < this->m_elementTable[n].size()) {
    return this->m_elementTable[n].at(listIndex);
  } else {
    Adcirc::Error::throwError("Out of element table request");
  }
  return nullptr;
}

Adcirc::Geometry::Element *ElementTable::elementTable(size_t nodeIndex,
                                                      size_t listIndex) {
  if (nodeIndex < this->mesh()->numNodes()) {
    if (listIndex <
        this->m_elementTable[this->mesh()->node(nodeIndex)].size()) {
      return this->m_elementTable[this->mesh()->node(nodeIndex)].at(listIndex);
    } else {
      Adcirc::Error::throwError("Out of element table request");
    }
  } else {
    Adcirc::Error::throwError("Out of bounds node request");
  }
  return nullptr;
}
