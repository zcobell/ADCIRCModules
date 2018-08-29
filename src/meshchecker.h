

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

private:
  Adcirc::Geometry::Mesh *m_mesh;

  static void
  printFailedLeveeStatus(Adcirc::Geometry::Boundary *bc, int index,
                         double minimumCrestElevationOverTopography);

  static void eline(Adcirc::Geometry::Mesh *mesh, size_t i, size_t j, int &n1, int &n2);
};
