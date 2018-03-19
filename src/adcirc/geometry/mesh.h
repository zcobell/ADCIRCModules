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
/**
 * @class Mesh
 * @author Zachary Cobell
 * @brief Class that handles operations using Adcirc mesh files
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved.
 * @license This project is released under the terms of the GNU General Public
 * License v3
 *
 * The Mesh class handles functions related to reading an
 * adcirc mesh into memory and provides some facilities for
 * manipulation. The code is designed to be functional
 * with the python interface to the code.
 *
 * The code is able to handle meshes that are traditional, that
 * is they contained order indicies. When the code detects unordered
 * indicies, a translation table is automatically generated that allows
 * the code to continue to function appropriately and the id column of
 * the mesh will be considered a label.
 *
 * Performance is considered to be of the greatest importance and
 * therefore the Boost libraries have been utilized for reading
 * and writing of mesh data over the standard C++ functions.
 * All edits to this code should be benchmarked against a prior
 * version to ensure that memory usage and cpu time is not adversely
 * affected.
 *
 *
 */
#ifndef MESH_H
#define MESH_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "adcirc/adcircmodules_global.h"
#include "adcirc/geometry/boundary.h"
#include "adcirc/geometry/element.h"
#include "adcirc/geometry/node.h"
#include "adcirc/point/point.h"

class QKdtree2;

namespace Adcirc {
namespace Geometry {

class Mesh {
 public:
  explicit Mesh();
  explicit Mesh(std::string filename);
  explicit Mesh(const char *filename);

  ~Mesh();

  int read();

  int write(std::string outputFile);
  int write(const char *outputFile);

  std::string filename() const;
  void setFilename(const std::string &filename);
  void setFilename(const char *filename);

  std::string meshHeaderString() const;
  void setMeshHeaderString(const std::string &meshHeaderString);
  void setMeshHeaderString(const char *meshHeaderString);

  int numNodes() const;
  void setNumNodes(int numNodes);

  int numElements() const;
  void setNumElements(int numElements);

  int numOpenBoundaries() const;
  void setNumOpenBoundaries(int numOpenBoundaries);

  int numLandBoundaries() const;
  void setNumLandBoundaries(int numLandBoundaries);

  int totalOpenBoundaryNodes();

  int totalLandBoundaryNodes();

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

  bool nodeOrderingIsLogical();
  bool elementOrderingIsLogical();

  Adcirc::Geometry::Node *node(int index);
  Adcirc::Geometry::Element *element(int index);
  Adcirc::Geometry::Boundary *openBoundary(int index);
  Adcirc::Geometry::Boundary *landBoundary(int index);

  Adcirc::Geometry::Node *nodeById(int id);
  Adcirc::Geometry::Element *elementById(int id);

  int nodeIndexById(int id);
  int elementIndexById(int id);

  void resizeMesh(int numNodes, int numElements, int numOpenBoundaries,
                  int numLandBoundaries);

  void addNode(int index, Adcirc::Geometry::Node &node);
  void deleteNode(int index);

  void addElement(int index, Adcirc::Geometry::Element &element);
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
  std::vector<Adcirc::Geometry::Node> m_nodes;
  std::vector<Adcirc::Geometry::Element> m_elements;
  std::vector<Adcirc::Geometry::Boundary> m_openBoundaries;
  std::vector<Adcirc::Geometry::Boundary> m_landBoundaries;
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
}
}

#endif  // MESH_H
