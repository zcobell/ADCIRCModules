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
#ifndef ADCMOD_TOPOLOGY_H
#define ADCMOD_TOPOLOGY_H

#include <memory>
#include "adcircmodules_global.h"
#include "elementtable.h"
#include "facetable.h"
#include "nodetable.h"

namespace Adcirc {
namespace Private {
class MeshPrivate;
}
namespace Geometry {

/**
 * @class Topology
 * @author Zachary Cobell
 * @copyright Copyright 2015-2020 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 * @brief The Topology class acts as a wrapper for NodeTable, ElementTable, and
 * FaceTable
 *
 */
class Topology {
 public:
  ADCIRCMODULES_EXPORT Topology(Adcirc::Private::MeshPrivate *mesh);

  ADCIRCMODULES_EXPORT Adcirc::Geometry::NodeTable *nodeTable();
  ADCIRCMODULES_EXPORT Adcirc::Geometry::ElementTable *elementTable();
  ADCIRCMODULES_EXPORT Adcirc::Geometry::FaceTable *faceTable();

 private:
  Adcirc::Private::MeshPrivate *m_mesh;
  std::unique_ptr<NodeTable> m_nodeTable;
  std::unique_ptr<ElementTable> m_elementTable;
  std::unique_ptr<FaceTable> m_faceTable;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ADCMOD_TOPOLOGY_H
