#ifndef ELEMENTTABLE_H
#define ELEMENTTABLE_H

#include <vector>
#include "element.h"
#include "mesh.h"

namespace Adcirc {
namespace Geometry {

class ElementTable {
 public:
  ElementTable();
  ElementTable(Adcirc::Geometry::Mesh *mesh);

  std::vector<Adcirc::Geometry::Element *> elementList(size_t nodeIndex);

  void build();

  Adcirc::Geometry::Mesh *mesh() const;
  void setMesh(Adcirc::Geometry::Mesh *mesh);

 private:
  std::vector<std::vector<Adcirc::Geometry::Element *>> m_elementTable;

  Adcirc::Geometry::Mesh *m_mesh;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ELEMENTTABLE_H
