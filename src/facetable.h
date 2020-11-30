#ifndef ADCMOD_FACETABLE_H
#define ADCMOD_FACETABLE_H

#include "adcircmodules_global.h"
#include "face.h"

namespace Adcirc {
namespace Private {
class MeshPrivate;
}

namespace Geometry {

/**
 * @class FaceTable
 * @author Zachary Cobell
 * @brief Class that handles relation of element faces
 * @copyright Copyright 2015-2020 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 *
 */
class FaceTable {
 public:
  ADCIRCMODULES_EXPORT FaceTable(Adcirc::Private::MeshPrivate *mesh);

  size_t ADCIRCMODULES_EXPORT
  numSharedFaces(Adcirc::Geometry::Element *element) const;
  size_t ADCIRCMODULES_EXPORT numSharedFaces(size_t index) const;

  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *neighbor(
      Adcirc::Geometry::Element *element, size_t index) const;

  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *neighbor(size_t element,
                                                           size_t index) const;

  std::vector<Adcirc::Geometry::Element *> ADCIRCMODULES_EXPORT
  neighbors(Adcirc::Geometry::Element *element) const;

  std::vector<Adcirc::Geometry::Element *> ADCIRCMODULES_EXPORT
  neighbors(size_t index) const;

  std::pair<Node *, Node *> ADCIRCMODULES_EXPORT
  sharedFace(Adcirc::Geometry::Element *element, size_t index) const;

  std::pair<Node *, Node *> ADCIRCMODULES_EXPORT sharedFace(size_t element,
                                                            size_t index) const;

  void ADCIRCMODULES_EXPORT build();

  bool ADCIRCMODULES_EXPORT initialized() const;

 private:
  bool m_initialized;
  std::vector<Face> m_table;
  std::vector<std::vector<Element *>> m_elementNeighbors;
  std::vector<std::vector<Face *>> m_sharedFaces;
  Adcirc::Private::MeshPrivate *m_mesh;
};
}  // namespace Geometry
}  // namespace Adcirc

#endif  // ADCMOD_FACETABLE_H
