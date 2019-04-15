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
#include "meshchecker.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

namespace Adcirc {
namespace Utility {

using namespace Adcirc::Geometry;

MeshChecker::MeshChecker(Mesh *mesh) : m_mesh(mesh) {}

bool MeshChecker::checkMesh() {
  bool passed = true;
  if (!this->checkNodeNumbering(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Node numbering check failed.\n");
    return false;
  }

  if (!this->checkElementNumbering(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Element numbering check failed.\n");
    return false;
  }

  if (!this->checkNodalElevations(this->m_mesh, -200.0)) {
    printf("MeshChecker::checkMesh :: Nodal elevation check failed.\n");
    passed = false;
  }

  if (!this->checkDisjointNodes(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Disjoint nodes found.");
    passed = false;
  }

  if (!this->checkOverlappingElements(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Overlapping elements found.");
    return false;
  }

  if (!this->checkLeveeHeights(this->m_mesh, 0.2)) {
    printf("MeshChecker::checkMesh :: Levee height check failed.\n");
    return false;
  }

  if (!this->checkPipeHeights((this->m_mesh))) {
    printf("MeshChecker::checkMesh :: Pipe height check failed.\n");
    return false;
  }

  if (!this->checkElementSizes(this->m_mesh, 20.0)) {
    printf("MeshChecker::checkMesh :: Element size check failed.");
    passed = false;
  }

  return passed;
}

bool MeshChecker::checkNodeNumbering(Mesh *mesh) {
  for (size_t i = 0; i < mesh->numNodes(); ++i) {
    if (mesh->node(i)->id() != i + 1) return false;
  }
  return true;
}

bool MeshChecker::checkElementNumbering(Mesh *mesh) {
  for (size_t i = 0; i < mesh->numElements(); ++i) {
    if (mesh->element(i)->id() != i + 1) return false;
  }
  return true;
}

bool MeshChecker::checkNodalElevations(Mesh *mesh,
                                       double minimumNodalElevation) {
  bool passed = true;
  for (size_t i = 0; i < mesh->numNodes(); ++i) {
    if (mesh->node(i)->z() < minimumNodalElevation) {
      printf(
          "[Mesh Error] MeshChecker::checkNodalElevations :: Node %zd has "
          "elevation %9.2f which is greater than specified minimum "
          "elevation of %9.2f.\n",
          mesh->node(i)->id(), mesh->node(i)->z(), minimumNodalElevation);
      passed = false;
    }
  }
  return passed;
}

bool MeshChecker::checkLeveeHeights(
    Mesh *mesh, double minimumCrestElevationOverTopography) {
  bool passed = true;
  for (size_t i = 0; i < mesh->numLandBoundaries(); ++i) {
    if (mesh->landBoundary(i)->boundaryCode() == 4 ||
        mesh->landBoundary(i)->boundaryCode() == 24 ||
        mesh->landBoundary(i)->boundaryCode() == 5 ||
        mesh->landBoundary(i)->boundaryCode() == 25) {
      for (size_t j = 0; j < mesh->landBoundary(i)->length(); ++j) {
        if (-mesh->landBoundary(i)->node1(j)->z() >
                mesh->landBoundary(i)->crestElevation(j) -
                    minimumCrestElevationOverTopography ||
            -mesh->landBoundary(i)->node2(j)->z() >
                mesh->landBoundary(i)->crestElevation(j) -
                    minimumCrestElevationOverTopography) {
          MeshChecker::printFailedLeveeStatus(
              mesh->landBoundary(i), j, minimumCrestElevationOverTopography);
          passed = false;
        }
      }
    } else if (mesh->landBoundary(i)->boundaryCode() == 3 ||
               mesh->landBoundary(i)->boundaryCode() == 13 ||
               mesh->landBoundary(i)->boundaryCode() == 23) {
      for (size_t j = 0; j < mesh->landBoundary(i)->length(); ++j) {
        if (-mesh->landBoundary(i)->node1(j)->z() >
            mesh->landBoundary(i)->crestElevation(j) -
                minimumCrestElevationOverTopography) {
          MeshChecker::printFailedLeveeStatus(
              mesh->landBoundary(i), j, minimumCrestElevationOverTopography);
          passed = false;
        }
      }
    }
  }
  return passed;
}

void MeshChecker::printFailedLeveeStatus(
    Boundary *bc, size_t index, double minimumCrestElevationOverTopography) {
  if (bc->isExternalWeir()) {
    printf(
        "[Mesh Error] MeshChecker::checkLeveeHeights :: Node %zd elevation "
        "(%f) greater than crest elevation (%f + %f) on single "
        "sided weir boundary.\n",
        bc->node1(index)->id(), -bc->node1(index)->z(),
        bc->crestElevation(index), minimumCrestElevationOverTopography);
  } else if (bc->isInternalWeir()) {
    if (-bc->node1(index)->z() >
        bc->crestElevation(index) - minimumCrestElevationOverTopography) {
      printf(
          "[Mesh Error] MeshChecker::checkLeveeHeights :: Node %zd elevation "
          "(%f) greater than crest elevation (%f + %f) on weir "
          "boundary.\n",
          bc->node1(index)->id(), -bc->node1(index)->z(),
          bc->crestElevation(index), minimumCrestElevationOverTopography);
    }
    if (-bc->node2(index)->z() >
        bc->crestElevation(index) - minimumCrestElevationOverTopography) {
      printf(
          "[Mesh Error] MeshChecker::checkLeveeHeights :: Node %zd elevation "
          "(%f) greater than crest elevation (%f + %f) on weir "
          "boundary.\n",
          bc->node2(index)->id(), -bc->node2(index)->z(),
          bc->crestElevation(index), minimumCrestElevationOverTopography);
    }
  }
  return;
}

bool MeshChecker::checkOverlappingElements(Mesh *mesh) {
  std::vector<Element *> overlappingList;
  mesh->buildElementTable();

  for (size_t i = 0; i < mesh->numElements(); ++i) {
    for (size_t j = 0; j < mesh->element(i)->n(); ++j) {
      int count = 0;

      std::pair<Node *, Node *> p = mesh->element(i)->elementLeg(j);
      Node *n1 = p.first;
      Node *n2 = p.second;

      for (size_t i1 = 0; i1 < mesh->elementsAroundNode(n1).size(); ++i1) {
        Element *m1 = mesh->elementsAroundNode(n1).at(i1);
        for (size_t i2 = 0; i2 < mesh->elementsAroundNode(n2).size(); ++i2) {
          Element *m2 = mesh->elementsAroundNode(n2).at(i2);
          if (m1->id() == m2->id()) {
            count++;
            break;
          }
        }
      }
      if (count > 2) overlappingList.push_back(mesh->element(i));
    }
  }

  if (overlappingList.size() != 0) {
    for (size_t i = 0; i < overlappingList.size(); ++i) {
      printf(
          "[Mesh Error] MeshChecker::checkOverlappingElements :: "
          "Overlapping element detected on element %zd \n",
          overlappingList[i]->id());
    }
    return false;
  } else {
    return true;
  }
}

bool MeshChecker::checkDisjointNodes(Mesh *mesh) {
  bool passed = true;
  std::vector<bool> found;
  found.resize(mesh->numNodes());
  std::fill(found.begin(), found.end(), false);

  for (size_t i = 0; i < mesh->numElements(); i++) {
    for (size_t j = 0; j < mesh->element(i)->n(); j++) {
      size_t index = mesh->nodeIndexById(mesh->element(i)->node(j)->id());
      found[index] = true;
    }
  }

  for (size_t i = 0; i < mesh->numNodes(); i++) {
    if (found[i] == false) {
      printf(
          "[Mesh Error] MeshChecker::checkDisjointNodes :: Node %zd is not "
          "connected to any element.",
          mesh->node(i)->id());
      passed = false;
    }
  }

  return passed;
}

bool MeshChecker::checkPipeHeights(Mesh *mesh) {
  bool passed = true;
  for (size_t i = 0; i < mesh->numLandBoundaries(); i++) {
    if (mesh->landBoundary(i)->boundaryCode() == 5 ||
        mesh->landBoundary(i)->boundaryCode() == 25) {
      for (size_t j = 0; j < mesh->landBoundary(i)->length(); j++) {
        double topOfPipe = mesh->landBoundary(i)->pipeHeight(j) +
                           0.5 * mesh->landBoundary(i)->pipeDiameter(j);
        double bottomOfPipe = mesh->landBoundary(i)->pipeHeight(j) -
                              0.5 * mesh->landBoundary(i)->pipeDiameter(j);
        double crest = mesh->landBoundary(i)->crestElevation(j);
        if (topOfPipe > crest) {
          passed = false;
          printf(
              "[Mesh Error] MeshChecker::checkPipeHeights :: Top of pipe > "
              "weir crest elevation at nodes %zd and %zd.",
              mesh->landBoundary(i)->node1(j)->id(),
              mesh->landBoundary(i)->node2(j)->id());
        }
        if (bottomOfPipe < mesh->landBoundary(i)->node1(j)->z() ||
            bottomOfPipe < mesh->landBoundary(i)->node2(j)->z()) {
          passed = false;
          printf(
              "[Mesh Error] MeshChecker::checkPipeHeights :: Bottom of pipe < "
              "nodal elevation at nodes %zd and %zd.",
              mesh->landBoundary(i)->node1(j)->id(),
              mesh->landBoundary(i)->node2(j)->id());
        }
      }
    }
  }
  return passed;
}

bool MeshChecker::checkElementSizes(Mesh *mesh, double minimumElementSize) {
  bool passed = true;
  std::vector<double> element_size;
  element_size.resize(mesh->numElements());
  for (size_t i = 0; i < mesh->numElements(); i++) {
    double size = mesh->element(i)->elementSize();
    if (size < minimumElementSize) {
      passed = false;
      printf(
          "[Mesh Error] MeshChecker::checkElementSizes :: Element %zd has size "
          "%f, which is less than %f\n",
          mesh->element(i)->id(), size, minimumElementSize);
    }
  }
  return passed;
}
}  // namespace Utility
}  // namespace Adcirc
