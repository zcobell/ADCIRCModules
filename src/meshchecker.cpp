#include "meshchecker.h"
#include <iostream>
#include <vector>

using namespace Adcirc::Geometry;

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
  }

  if (!this->checkLeveeHeights(this->m_mesh, 0.2)) {
    printf("MeshChecker::checkMesh :: Levee height check failed.\n");
    return false;
  }

  return true;
}

bool MeshChecker::checkNodeNumbering(Mesh *mesh) {
  for (size_t i = 0; i < mesh->numNodes(); ++i) {
    if (mesh->node(i)->id() != i + 1)
      return false;
  }
  return true;
}

bool MeshChecker::checkElementNumbering(Mesh *mesh) {
  for (size_t i = 0; i < mesh->numElements(); ++i) {
    if (mesh->element(i)->id() != i + 1)
      return false;
  }
  return true;
}

bool MeshChecker::checkNodalElevations(Mesh *mesh,
                                       double minimumNodalElevation) {
  bool passed = true;
  for (size_t i = 0; i < mesh->numNodes(); ++i) {
    if (mesh->node(i)->z() < minimumNodalElevation) {
      printf("[Mesh Error] MeshChecker::checkNodalElevations :: Node %zd has "
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
    printf("Mesh Error: MeshChecker::checkLeveeHeights :: Node %zd elevation "
           "(%6.2f) greater than crest elevation (%6.2f) + %6.2f on single "
           "sided weir "
           "boundary.\n",
           bc->node1(index)->id(), bc->node1(index)->z(),
           bc->crestElevation(index), minimumCrestElevationOverTopography);
  } else if (bc->boundaryCode() == 4 || bc->boundaryCode() == 24 ||
             bc->boundaryCode() == 5 || bc->boundaryCode() == 25) {
    if (-bc->node1(index)->z() > bc->crestElevation(index)) {
      printf("Mesh Error: MeshChecker::checkLeveeHeights :: Node %zd elevation "
             "(%6.2f) greater than crest elevation (%6.2f) + %6.2f on weir "
             "boundary.\n",
             bc->node1(index)->id(), bc->node1(index)->z(),
             bc->crestElevation(index), minimumCrestElevationOverTopography);
    }
    if (-bc->node2(index)->z() > bc->crestElevation(index)) {
      printf("Mesh Error: MeshChecker::checkLeveeHeights :: Node %zd elevation "
             "(%6.2f) greater than crest elevation (%6.2f) + %6.2f on weir "
             "boundary.\n",
             bc->node2(index)->id(), bc->node2(index)->z(),
             bc->crestElevation(index), minimumCrestElevationOverTopography);
    }
  }
  return;
}

bool MeshChecker::checkOverlappingElements(Mesh *mesh) {
  std::vector<std::vector<Element *>> elementsAroundNode;
  std::vector<Element*> overlappingList;
  elementsAroundNode.resize(mesh->numNodes());

  for (size_t i = 0; i < mesh->numElements(); ++i) {
    for (int j = 0; j < mesh->element(i)->n(); ++j) {
      size_t n = mesh->nodeIndexById(mesh->element(i)->node(j)->id());
      elementsAroundNode[n].push_back(mesh->element(i));
    }
  }

  for (size_t i = 0; i < mesh->numElements(); ++i) {
    for (int j = 0; j < mesh->element(i)->n(); ++j) {
        int n1, n2;
        MeshChecker::eline(mesh,i,j,n1,n2);
        int count = 0;
        for(size_t i1 = 0 ; i1 < elementsAroundNode[n1].size(); ++i1){
            Element *m1 = elementsAroundNode[n1][i1];
            for(size_t i2 = 0; i2 < elementsAroundNode[n2].size(); ++i2){
                Element *m2 = elementsAroundNode[n2][i2];
                if(m1==m2){
                    count++;
                    break;
                }
            }
        }
        if (count > 2) overlappingList.push_back(mesh->element(i));

    }
  }

  if(overlappingList.size()!=0){
    for(size_t i=0;i<overlappingList.size();++i){
      printf("[Mesh Error] MeshChecker::checkOverlappingElements :: "
             "Overlapping element detected on element %zd \n",
             overlappingList[i]->id());
    }
    return false;
  }else{
    return true;
  }

}

void MeshChecker::eline(Mesh *mesh, size_t i, size_t j, int &n1, int &n2) {
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
