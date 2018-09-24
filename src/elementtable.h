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

  std::vector<Adcirc::Geometry::Element *> elementList(
      Adcirc::Geometry::Node *n);

  size_t numElementsAroundNode(Adcirc::Geometry::Node *n);
  size_t numElementsAroundNode(size_t nodeIndex);
  Adcirc::Geometry::Element *elementTable(Adcirc::Geometry::Node *n,
                                          size_t listIndex);
  Adcirc::Geometry::Element *elementTable(size_t nodeIndex, size_t listIndex);

  void build();

  Adcirc::Geometry::Mesh *mesh() const;
  void setMesh(Adcirc::Geometry::Mesh *mesh);

 private:
  std::unordered_map<Adcirc::Geometry::Node *,
                     std::vector<Adcirc::Geometry::Element *>>
      m_elementTable;

  Adcirc::Geometry::Mesh *m_mesh;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ELEMENTTABLE_H
