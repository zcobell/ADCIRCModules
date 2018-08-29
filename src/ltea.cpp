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
#include "ltea.h"

using namespace Adcirc::Geometry;

Ltea::Ltea(Mesh *mesh) {
  this->m_mesh = mesh;
  if (!this->m_mesh->elementalSearchTreeInitialized()) {
    this->m_mesh->buildElementalSearchTree();
  }
  this->createInverseConnectivityTable();
}

void Ltea::createInverseConnectivityTable() {
  this->m_inverseConnectivity.resize(this->m_mesh->numNodes());

  for (size_t i = 0; i < this->m_mesh->numNodes(); i++) {
    this->m_inverseConnectivity[i].node = this->m_mesh->node(i);
  }

  for (size_t i = 0; i < this->m_mesh->numElements(); i++) {
    for (size_t j = 0; j < this->m_mesh->element(i)->n(); j++) {
      size_t index =
          this->m_mesh->nodeIndexById(this->m_mesh->element(i)->node(j)->id());
      this->m_inverseConnectivity[index].element.push_back(
          this->m_mesh->element(i));
    }
  }

  for (size_t i = 0; i < this->m_inverseConnectivity.size(); i++) {
    this->m_inverseConnectivity[i].n =
        this->m_inverseConnectivity[i].element.size();
  }

  return;
}
