/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2020 Zachary Cobell
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
#include "FaceTable.h"

#include <algorithm>
#include <cassert>

#include "Logging.h"
#include "Mesh.h"
#include "MeshPrivate.h"

using namespace Adcirc::Geometry;

/**
 * @brief Constructor which uses a private mesh object
 * @param[in] mesh pointer to MeshPrivate object
 */
FaceTable::FaceTable(Adcirc::Private::MeshPrivate *mesh)
    : m_initialized(false), m_mesh(mesh) {}

/**
 * @brief Returns the number of shared faces for a given element
 * @param[in] index index of element in the mesh
 * @return number of shared faces
 */
size_t FaceTable::numSharedFaces(size_t index) const {
  return m_elementNeighbors[index].size();
}

/**
 * @overload
 * @param[in] element pointer to the element to query
 */
size_t FaceTable::numSharedFaces(Adcirc::Geometry::Element *element) const {
  return m_elementNeighbors[m_mesh->elementIndexById(element->id())].size();
}

/**
 * @brief Returns the ith neighbor element
 * @param[in] element pointer to an element
 * @param[in] index index of neighbor to return
 * @return pointer to neighbor element
 */
Element *FaceTable::neighbor(Element *element, size_t index) const {
  size_t pos = m_mesh->elementIndexById(element->id());
  assert(index < m_elementNeighbors[pos].size());
  return m_elementNeighbors[pos][index];
}

/**
 * @overload
 * @param[in] element index of element
 * @param[in] index index of neighbor to return
 */
Element *FaceTable::neighbor(size_t element, size_t index) const {
  assert(element < m_elementNeighbors.size());
  assert(index < m_elementNeighbors[element].size());
  return m_elementNeighbors[element][index];
}

/**
 * @brief Returns all face sharing neighbors of an element
 * @param[in] element pointer to element
 * @return vector of element pointers
 */
std::vector<Adcirc::Geometry::Element *> FaceTable::neighbors(
    Adcirc::Geometry::Element *element) const {
  size_t index = m_mesh->elementIndexById(element->id());
  return m_elementNeighbors[index];
}

/**
 * @overload
 * @param index[in] element index in the mesh to query
 */
std::vector<Adcirc::Geometry::Element *> FaceTable::neighbors(
    size_t index) const {
  assert(index < m_elementNeighbors.size());
  return m_elementNeighbors[index];
}

/**
 * @brief Returns a shared face (two connected nodes)
 * @param[in] element pointer to element to search
 * @param[in] index return the ith shared face on the element
 * @return pair of node pointers making up a shared face
 */
std::pair<Node *, Node *> FaceTable::sharedFace(
    Adcirc::Geometry::Element *element, size_t index) const {
  size_t pos = m_mesh->elementIndexById(element->id());
  assert(index < m_elementNeighbors[pos].size());
  return m_sharedFaces[pos][index]->nodes();
}

/**
 * @overload
 * @param[in] element index to search
 * @param[in] index return the ith shared face on the element
 */
std::pair<Node *, Node *> FaceTable::sharedFace(size_t element,
                                                size_t index) const {
  assert(element < m_mesh->numElements());
  assert(index < m_sharedFaces.size());
  return m_sharedFaces[element][index]->nodes();
}

/**
 * @brief Constructs the internal table of element faces
 */
void FaceTable::build() {
  if (!m_mesh) {
    adcircmodules_throw_exception("No mesh defined");
  }

  std::vector<Face> tempTable;
  tempTable.reserve(m_mesh->numElements() * 4);
  for (size_t i = 0; i < m_mesh->numElements(); ++i) {
    auto face = m_mesh->element(i)->faces();
    for (auto &j : face) {
      auto fn1 = Adcirc::Geometry::FaceNode(j.first);
      auto fn2 = Adcirc::Geometry::FaceNode(j.second);
      if (fn1 < fn2) std::swap(fn1, fn2);
      tempTable.emplace_back(
          std::pair<FaceNode, FaceNode>(fn1, fn2),
          std::pair<Element *, Element *>(m_mesh->element(i), nullptr));
    }
  }
  std::sort(tempTable.begin(), tempTable.end());

  size_t nf = 0;
  for (size_t i = 0; i < tempTable.size() - 1; ++i) {
    if (tempTable[i] == tempTable[i + 1]) {
      tempTable[i].setElements(
          {tempTable[i].elements().first, tempTable[i + 1].elements().first});
      nf++;
    }
  }

  m_table.reserve(nf);
  for (size_t i = tempTable.size(); i > 0; --i) {
    if (tempTable[i].elements().second) {
      m_table.push_back(std::move(tempTable[i]));
    }
  }
  tempTable.clear();

  auto it = std::unique(m_table.begin(), m_table.end());
  m_table = std::vector<Face>(m_table.begin(), it);

  m_elementNeighbors.resize(m_mesh->numElements());
  m_sharedFaces.resize(m_mesh->numElements());
  for (size_t i = 0; i < m_mesh->numElements(); ++i) {
    m_elementNeighbors.reserve(m_mesh->element(i)->n());
    m_sharedFaces[i].reserve(m_mesh->element(i)->n());
  }

  for (auto &f : m_table) {
    if (f.elements().second == nullptr) continue;
    size_t id1 = m_mesh->elementIndexById(f.elements().first->id());
    size_t id2 = m_mesh->elementIndexById(f.elements().second->id());
    m_elementNeighbors[id1].push_back(f.elements().second);
    m_elementNeighbors[id2].push_back(f.elements().first);
    m_sharedFaces[m_mesh->elementIndexById(f.elements().first->id())].push_back(
        &f);
    m_sharedFaces[m_mesh->elementIndexById(f.elements().second->id())]
        .push_back(&f);
  }

  for (auto &f : m_elementNeighbors) {
    std::sort(f.begin(), f.end());
    auto it = std::unique(f.begin(), f.end());
    f = std::vector<Adcirc::Geometry::Element *>(f.begin(), it);
  }

  this->m_initialized = true;
}

/**
 * @brief Checks the initialization status of the object
 * @return true if this table has been initialized
 */
bool FaceTable::initialized() const { return m_initialized; }
