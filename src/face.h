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
#ifndef ADCMOD_FACE_H
#define ADCMOD_FACE_H

#include <utility>
#include "constants.h"
#include "element.h"
#include "node.h"

namespace Adcirc {
namespace Geometry {

/**
 * @class Face
 * @author Zachary Cobell
 * @brief Class that allows a basic and fast sorting of adcirc nodes
 * @copyright Copyright 2015-2020 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 */
class FaceNode {
 public:
  FaceNode(Adcirc::Geometry::Node *node)
      : m_node(node), m_reference(generateReference(node)) {}
  Adcirc::Geometry::Node *node() const { return m_node; }
  double reference() const { return m_reference; }

  static double generateReference(Adcirc::Geometry::Node *node) {
    return Constants::distance(node->x(), node->y(), 0.0, 0.0);
  }

  bool operator<(const FaceNode &ref) const {
    return this->reference() < ref.reference();
  }

 private:
  Adcirc::Geometry::Node *m_node;
  double m_reference;
};

/**
 * @class Face
 * @author Zachary Cobell
 * @brief Class that describes a shared element face
 * @copyright Copyright 2015-2020 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 */
class Face {
 public:
  Face() : m_nodes(nullptr, nullptr), m_elements(nullptr, nullptr){};

  Face(Adcirc::Geometry::Node *n1 = nullptr,
       Adcirc::Geometry::Node *n2 = nullptr,
       Adcirc::Geometry::Element *e1 = nullptr,
       Adcirc::Geometry::Element *e2 = nullptr)
      : m_nodes(n1, n2), m_elements(e1, e2) {}

  Face(std::pair<Adcirc::Geometry::Node *, Adcirc::Geometry::Node *> nodes,
       std::pair<Adcirc::Geometry::Element *, Adcirc::Geometry::Element *>
           elements)
      : m_nodes(FaceNode(nodes.first), FaceNode(nodes.second)),
        m_elements(elements) {}

  Face(std::pair<Adcirc::Geometry::FaceNode, Adcirc::Geometry::FaceNode> n,
       std::pair<Adcirc::Geometry::Element *, Adcirc::Geometry::Element *> e)
      : m_nodes(n), m_elements(e) {}

  std::pair<FaceNode, FaceNode> raw_nodes() const {
    return std::make_pair(m_nodes.first, m_nodes.second);
  }

  std::pair<Adcirc::Geometry::Node *, Adcirc::Geometry::Node *> nodes() const {
    return std::make_pair(m_nodes.first.node(), m_nodes.second.node());
  }
  void setNodes(const std::pair<Adcirc::Geometry::Node *,
                                Adcirc::Geometry::Node *> &nodes);

  std::pair<Adcirc::Geometry::Element *, Adcirc::Geometry::Element *> elements()
      const {
    return m_elements;
  }
  void setElements(const std::pair<Adcirc::Geometry::Element *,
                                   Adcirc::Geometry::Element *> &elements) {
    m_elements = elements;
  }

  void sort() {
    if (m_nodes.first.reference() < m_nodes.second.reference())
      std::swap(m_nodes.first, m_nodes.second);
  }

  bool sorted() {
    return m_nodes.first.reference() < m_nodes.second.reference();
  }

  bool operator==(const Face &f) const {
    return m_nodes.first.node() == f.nodes().first &&
           m_nodes.second.node() == f.nodes().second;
  }

  bool operator<(const Face &f) const {
    return m_nodes.first.reference() + m_nodes.second.reference() <
           f.raw_nodes().first.reference() + f.raw_nodes().second.reference();
  }

 private:
  std::pair<Adcirc::Geometry::FaceNode, Adcirc::Geometry::FaceNode> m_nodes;
  std::pair<Adcirc::Geometry::Element *, Adcirc::Geometry::Element *>
      m_elements;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ADCMOD_FACE_H
