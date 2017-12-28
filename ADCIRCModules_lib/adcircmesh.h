//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#ifndef ADCIRCMESH_H
#define ADCIRCMESH_H

#include "adcircboundary.h"
#include "adcircelement.h"
#include "adcircenum.h"
#include "adcircmodules_global.h"
#include "adcircnode.h"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class QKdtree2;

class AdcircMesh {
public:
  explicit AdcircMesh();
  explicit AdcircMesh(std::string filename);
  explicit AdcircMesh(const char *filename);

  ~AdcircMesh();

  int read();

  int write(std::string outputFile);
  int write(const char *outputFile);

  std::string filename() const;
  void setFilename(const std::string &filename);
  void setFilename(const char *filename);

  std::string meshHeaderString() const;
  void setMeshHeaderString(const std::string &meshHeaderString);

  int numNodes() const;
  void setNumNodes(int numNodes);

  int numElements() const;
  void setNumElements(int numElements);

  int numOpenBoundaries() const;
  void setNumOpenBoundaries(int numOpenBoundaries);

  int numLandBoundaries() const;
  void setNumLandBoundaries(int numLandBoundaries);

  int totalOpenBoundaryNodes() const;
  void setTotalOpenBoundaryNodes(int totalOpenBoundaryNodes);

  int totalLandBoundaryNodes() const;
  void setTotalLandBoundaryNodes(int totalLandBoundaryNodes);

  int projection();
  void defineProjection(int epsg, bool isLatLon);
  int reproject(int epsg);
  bool isLatLon();

  int toShapefile(std::string outputFile);
  int toShapefile(const char *outputFile);

  int buildNodalSearchTree();
  int buildElementalSearchTree();

  bool nodalSearchTreeInitialized();
  bool elementalSearchTreeInitialized();

  AdcircNode *node(int index);
  AdcircElement *element(int index);
  AdcircBoundary *openBoundary(int index);
  AdcircBoundary *landBoundary(int index);

  void resizeMesh(int numNodes, int numElements, int numOpenBoundaries,
                  int numLandBoundaries);

  void addNode(int index, AdcircNode &node);
  void deleteNode(int index);

  void addElement(int index, AdcircElement &element);
  void deleteElement(int index);

  QKdtree2 *nodalSearchTree() const;
  QKdtree2 *elementalSearchTree() const;

private:
  int _readMeshHeader(std::fstream &fid);
  int _readNodes(std::fstream &fid);
  int _readElements(std::fstream &fid);
  int _readOpenBoundaries(std::fstream &fid);
  int _readLandBoundaries(std::fstream &fid);
  void _init();

  std::string m_filename;
  std::string m_meshHeaderString;
  std::vector<AdcircNode> m_nodes;
  std::vector<AdcircElement> m_elements;
  std::vector<AdcircBoundary> m_openBoundaries;
  std::vector<AdcircBoundary> m_landBoundaries;
  std::unordered_map<int, int> m_nodeLookup;
  std::unordered_map<int, int> m_elementLookup;
  int m_numNodes;
  int m_numElements;
  int m_numOpenBoundaries;
  int m_numLandBoundaries;
  int m_totalOpenBoundaryNodes;
  int m_totalLandBoundaryNodes;
  int m_epsg;
  bool m_isLatLon;

  bool m_nodeOrderingLogical;
  bool m_elementOrderingLogical;

  QKdtree2 *m_nodalSearchTree;
  QKdtree2 *m_elementalSearchTree;
};

#endif // ADCIRCMESH_H
