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
#include "topology.h"

using namespace Adcirc::Geometry;

/**
 * @brief Constructor
 * @param mesh pointer to MeshPrivate object
 */
Topology::Topology(Adcirc::Private::MeshPrivate *mesh)
    : m_mesh(mesh),
      m_nodeTable(std::make_unique<NodeTable>(m_mesh)),
      m_elementTable(std::make_unique<ElementTable>(m_mesh)),
      m_faceTable(std::make_unique<FaceTable>(m_mesh)) {}

/**
 * @brief Returns pointer to the node table
 * @return node table pointer
 */
NodeTable *Topology::nodeTable() { return m_nodeTable.get(); }

/**
 * @brief Returns pointer to the element table
 * @return element table pointer
 */
ElementTable *Topology::elementTable() { return m_elementTable.get(); }

/**
 * @brief Returns pointer to the face table
 * @return face table pointer
 */
FaceTable *Topology::faceTable() { return m_faceTable.get(); }
