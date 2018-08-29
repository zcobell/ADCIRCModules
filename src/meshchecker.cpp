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
#include <elementtable.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace Adcirc::Geometry;
using namespace std;

MeshChecker::MeshChecker(Mesh *mesh) { this->m_mesh = mesh; }

bool MeshChecker::checkMesh() {
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
    return false;
  }

  if (!this->checkDisjointNodes(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Disjoint nodes found.");
    return false;
  }

  if (!this->checkOverlappingElements(this->m_mesh)) {
    printf("MeshChecker::checkMesh :: Overlapping elements found.");
    return false;
  }

  if (!this->checkLeveeHeights(this->m_mesh, 0.2)) {
    printf("MeshChecker::checkMesh :: Levee height check failed.\n");
    return false;
  }

  return true;
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
    Boundary *bc, int index, double minimumCrestElevationOverTopography) {
  if (bc->boundaryCode() == 3 || bc->boundaryCode() == 13 ||
      bc->boundaryCode() == 23) {
    printf(
        "[Mesh Error] MeshChecker::checkLeveeHeights :: Node %zd elevation "
        "(%f) greater than crest elevation (%f + %f) on single "
        "sided weir boundary.\n",
        bc->node1(index)->id(), -bc->node1(index)->z(),
        bc->crestElevation(index), minimumCrestElevationOverTopography);
  } else if (bc->boundaryCode() == 4 || bc->boundaryCode() == 24 ||
             bc->boundaryCode() == 5 || bc->boundaryCode() == 25) {
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
  vector<Element *> overlappingList;

  ElementTable elementsAroundNode = ElementTable(mesh);
  elementsAroundNode.build();

  for (size_t i = 0; i < mesh->numElements(); ++i) {
    for (int j = 0; j < mesh->element(i)->n(); ++j) {
      size_t n1, n2;
      int count = 0;

      MeshChecker::eline(mesh, i, j, n1, n2);

      for (size_t i1 = 0; i1 < elementsAroundNode.elementList(n1).size();
           ++i1) {
        Element *m1 = elementsAroundNode.elementList(n1).at(i1);
        for (size_t i2 = 0; i2 < elementsAroundNode.elementList(n2).size();
             ++i2) {
          Element *m2 = elementsAroundNode.elementList(n2).at(i2);
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

void MeshChecker::eline(Mesh *mesh, size_t i, size_t j, size_t &n1,
                        size_t &n2) {
  assert(j >= 0 && j < 3);
  switch (j) {
    case 0:
      n1 = mesh->nodeIndexById(mesh->element(i)->node(0)->id());
      n2 = mesh->nodeIndexById(mesh->element(i)->node(1)->id());
      break;
    case 1:
      n1 = mesh->nodeIndexById(mesh->element(i)->node(1)->id());
      n2 = mesh->nodeIndexById(mesh->element(i)->node(2)->id());
      break;
    case 2:
      n1 = mesh->nodeIndexById(mesh->element(i)->node(2)->id());
      n2 = mesh->nodeIndexById(mesh->element(i)->node(0)->id());
      break;
  }
  return;
}

bool MeshChecker::checkDisjointNodes(Mesh *mesh) {
  bool passed = true;
  vector<bool> found;
  found.resize(mesh->numNodes());
  std::fill(found.begin(), found.end(), false);

  for (size_t i = 0; i < mesh->numElements(); i++) {
    for (int j = 0; j < mesh->element(i)->n(); j++) {
      int index = mesh->nodeIndexById(mesh->element(i)->node(j)->id());
      found[index] = true;
    }
  }

  for (size_t i = 0; i < mesh->numNodes(); i++) {
    if (found[i] = false) {
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
