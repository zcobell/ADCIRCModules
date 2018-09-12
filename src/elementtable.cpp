#include "elementtable.h"

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
  return this->m_elementTable[n];
}
