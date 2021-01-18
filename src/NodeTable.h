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
#ifndef ADCMOD_NODETABLE_H
#define ADCMOD_NODETABLE_H

#include <vector>

#include "AdcircModules_Global.h"
#include "Node.h"

namespace Adcirc {
namespace Private {
class MeshPrivate;
}

namespace Geometry {
class NodeTable {
 public:
  ADCIRCMODULES_EXPORT NodeTable(Adcirc::Private::MeshPrivate *mesh);

  size_t ADCIRCMODULES_EXPORT numNodesAroundNode(Adcirc::Geometry::Node *node);
  size_t ADCIRCMODULES_EXPORT numNodesAroundNode(size_t node);

  std::vector<Adcirc::Geometry::Node *> ADCIRCMODULES_EXPORT
  nodeList(Adcirc::Geometry::Node *node);

  std::vector<Adcirc::Geometry::Node *> ADCIRCMODULES_EXPORT
  nodeList(size_t index);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node(
      Adcirc::Geometry::Node *node, size_t index);
  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node(size_t node, size_t index);

  void ADCIRCMODULES_EXPORT build();

 private:
  std::vector<std::vector<Adcirc::Geometry::Node *>> m_nodeTable;
  Adcirc::Private::MeshPrivate *m_mesh;
};

}  // namespace Geometry
}  // namespace Adcirc
#endif  // NODETABLE_H
