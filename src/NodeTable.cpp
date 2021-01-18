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
#include "NodeTable.h"

#include <algorithm>

#include "MeshPrivate.h"

using namespace Adcirc::Geometry;

NodeTable::NodeTable(Adcirc::Private::MeshPrivate *mesh) : m_mesh(mesh) {}

size_t NodeTable::numNodesAroundNode(Node *node) {
  size_t idx = m_mesh->nodeIndexById(node->id());
  return m_nodeTable[idx].size();
}

size_t NodeTable::numNodesAroundNode(size_t node) {
  return m_nodeTable[node].size();
}

void NodeTable::build() {
  this->m_nodeTable.resize(m_mesh->numNodes());
  for (auto &t : m_nodeTable) {
    t.reserve(20);
  }

  for (auto &e : *m_mesh->elements()) {
    Node *n1 = e.node(0);
    Node *n2 = e.node(1);
    Node *n3 = e.node(2);
    size_t id1 = m_mesh->nodeIndexById(n1->id());
    size_t id2 = m_mesh->nodeIndexById(n2->id());
    size_t id3 = m_mesh->nodeIndexById(n3->id());
    m_nodeTable[id1].push_back(n2);
    m_nodeTable[id1].push_back(n3);
    m_nodeTable[id2].push_back(n1);
    m_nodeTable[id2].push_back(n3);
    m_nodeTable[id3].push_back(n1);
    m_nodeTable[id3].push_back(n2);
    if (e.n() == 3) {
    } else if (e.n() == 4) {
      Node *n4 = e.node(3);
      size_t id4 = m_mesh->nodeIndexById(n4->id());
      m_nodeTable[id1].push_back(n2);
      m_nodeTable[id1].push_back(n4);
      m_nodeTable[id2].push_back(n3);
      m_nodeTable[id2].push_back(n1);
      m_nodeTable[id3].push_back(n2);
      m_nodeTable[id3].push_back(n4);
      m_nodeTable[id4].push_back(n1);
      m_nodeTable[id4].push_back(n3);
    }
  }

  for (auto &v : m_nodeTable) {
    std::sort(v.begin(), v.end());
    auto it = std::unique(v.begin(), v.end());
    v = std::vector<Adcirc::Geometry::Node *>(v.begin(), it);
  }
}

std::vector<Adcirc::Geometry::Node *> NodeTable::nodeList(
    Adcirc::Geometry::Node *node) {
  size_t idx = m_mesh->nodeIndexById(node->id());
  return m_nodeTable[idx];
}

std::vector<Adcirc::Geometry::Node *> NodeTable::nodeList(size_t index) {
  return m_nodeTable[index];
}

Adcirc::Geometry::Node *NodeTable::node(Adcirc::Geometry::Node *node,
                                        size_t index) {
  return this->nodeList(node)[index];
}

Adcirc::Geometry::Node *NodeTable::node(size_t node, size_t index) {
  return this->nodeList(node)[index];
}
