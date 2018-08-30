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
#include "mesh.h"

class MeshChecker {
 public:
  MeshChecker(Adcirc::Geometry::Mesh *mesh);

  bool checkMesh();

  static bool checkLeveeHeights(Adcirc::Geometry::Mesh *mesh,
                                double minimumCrestElevationOverTopography);
  static bool checkNodeNumbering(Adcirc::Geometry::Mesh *mesh);
  static bool checkElementNumbering(Adcirc::Geometry::Mesh *mesh);
  static bool checkNodalElevations(Adcirc::Geometry::Mesh *mesh,
                                   double minimumNodalelevation);
  static bool checkOverlappingElements(Adcirc::Geometry::Mesh *mesh);
  static bool checkDisjointNodes(Adcirc::Geometry::Mesh *mesh);
  static bool checkPipeHeights(Adcirc::Geometry::Mesh *mesh);
  static bool checkElementSizes(Adcirc::Geometry::Mesh *mesh,
                                double minimumElementSize);

 private:
  Adcirc::Geometry::Mesh *m_mesh;

  static void printFailedLeveeStatus(
      Adcirc::Geometry::Boundary *bc, int index,
      double minimumCrestElevationOverTopography);
};
