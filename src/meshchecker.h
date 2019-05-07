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
#ifndef ADCMOD_MESHCHECKER_H
#define ADCMOD_MESHCHECKER_H

#include "adcircmodules_global.h"
#include "mesh.h"

namespace Adcirc {

namespace Utility {

class MeshChecker {
 public:
  ADCIRCMODULES_EXPORT MeshChecker(Adcirc::Geometry::Mesh *mesh);

  bool ADCIRCMODULES_EXPORT checkMesh();

  static bool ADCIRCMODULES_EXPORT checkLeveeHeights(
      Adcirc::Geometry::Mesh *mesh, double minimumCrestElevationOverTopography);
  static bool ADCIRCMODULES_EXPORT
  checkNodeNumbering(Adcirc::Geometry::Mesh *mesh);
  static bool ADCIRCMODULES_EXPORT
  checkElementNumbering(Adcirc::Geometry::Mesh *mesh);
  static bool ADCIRCMODULES_EXPORT checkNodalElevations(
      Adcirc::Geometry::Mesh *mesh, double minimumNodalelevation);
  static bool ADCIRCMODULES_EXPORT
  checkOverlappingElements(Adcirc::Geometry::Mesh *mesh);
  static bool ADCIRCMODULES_EXPORT checkDisjointNodes(
      Adcirc::Geometry::Mesh *mesh, const std::string &logFile = "none");
  static bool ADCIRCMODULES_EXPORT
  checkPipeHeights(Adcirc::Geometry::Mesh *mesh);
  static bool ADCIRCMODULES_EXPORT
  checkElementSizes(Adcirc::Geometry::Mesh *mesh, double minimumElementSize);
  static bool ADCIRCMODULES_EXPORT checkMissingBoundaryConditions(
      Adcirc::Geometry::Mesh *mesh, const std::string &logFile = "none");

 private:
  Adcirc::Geometry::Mesh *m_mesh;

  static void printFailedLeveeStatus(
      Adcirc::Geometry::Boundary *bc, size_t index,
      double minimumCrestElevationOverTopography);
};
}  // namespace Utility
}  // namespace Adcirc

#endif  // ADCMOD_MESHCHECKER_H
