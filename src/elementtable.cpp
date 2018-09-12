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

  this->m_elementTable.resize(this->m_mesh->numNodes());

  for (size_t i = 0; i < this->m_mesh->numElements(); ++i) {
    for (int j = 0; j < this->m_mesh->element(i)->n(); ++j) {
      size_t n =
          this->m_mesh->nodeIndexById(this->m_mesh->element(i)->node(j)->id());
      this->m_elementTable[n].push_back(this->m_mesh->element(i));
    }
  }
  return;
}

std::vector<Element *> ElementTable::elementList(size_t nodeIndex) {
  if (nodeIndex < this->m_mesh->numNodes()) {
    return this->m_elementTable[nodeIndex];
  } else {
    std::vector<Element *> a;
    return a;
  }
}
