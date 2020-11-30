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
#ifndef ADCMOD_ELEMENTTABLE_H
#define ADCMOD_ELEMENTTABLE_H

#include <memory>
#include <vector>
#include "adcircmodules_global.h"
#include "element.h"

namespace Adcirc {

namespace Private {
class MeshPrivate;
}  // namespace Private

namespace Geometry {
class Mesh;

/**
 * @class ElementTable
 * @author Zachary Cobell
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 * @brief The ElementTable class is used to develop a list of the elements that
 * surround each node
 *
 */

class ElementTable {
 public:
  ADCIRCMODULES_EXPORT ElementTable(Adcirc::Private::MeshPrivate *mesh);

  std::vector<Adcirc::Geometry::Element *> ADCIRCMODULES_EXPORT
  elementList(Adcirc::Geometry::Node *n);

  size_t ADCIRCMODULES_EXPORT numElementsAroundNode(Adcirc::Geometry::Node *n);
  size_t ADCIRCMODULES_EXPORT numElementsAroundNode(size_t nodeIndex);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *elementTable(
      Adcirc::Geometry::Node *n, size_t listIndex);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *elementTable(
      size_t nodeIndex, size_t listIndex);

  void ADCIRCMODULES_EXPORT build();

  bool ADCIRCMODULES_EXPORT initialized();

  Adcirc::Private::MeshPrivate ADCIRCMODULES_EXPORT *mesh() const;
  void ADCIRCMODULES_EXPORT setMesh(Adcirc::Private::MeshPrivate *mesh);

 private:
  std::vector<std::vector<Adcirc::Geometry::Element *>> m_elementTable;
  Adcirc::Private::MeshPrivate *m_mesh;

  bool m_initialized;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ADCMOD_ELEMENTTABLE_H
