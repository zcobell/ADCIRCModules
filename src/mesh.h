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
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "boundary.h"
#include "element.h"
#include "meshformats.h"
#include "node.h"

class QKdtree2;
class MeshImpl;

namespace Adcirc {
namespace Geometry {

/**
 * @class Mesh
 * @author Zachary Cobell
 * @brief Class that handles operations using Adcirc mesh files
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 *
 * The Mesh class handles functions related to reading an
 * unstructured mesh into memory and provides some facilities for
 * manipulation. The code is designed to be functional
 * with the python interface to the code.
 *
 * I/O functions are provided for three unstructured mesh formats:
 *   1) ADCIRC ASCII formatted mesh
 *   2) Aquaveo 2dm formatted mesh
 *   3) DFlow-FM netcdf formatted mesh
 * These meshes can be translated between formats using the i/o routines
 * however, quadrilaterals are not officially supported in the adcirc format.
 * Meshes may only contain triangles and quadrilaterals.
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
 * This class uses the PIMPL idiom and the implementation of the
 * class is contained in the MeshImpl class.
 *
 */
class Mesh {
 public:
  Mesh();
  explicit Mesh(const std::string &filename);

  ~Mesh();

  const size_t ELEMENT_NOT_FOUND = std::numeric_limits<size_t>::max();

  std::vector<double> x();
  std::vector<double> y();
  std::vector<double> z();
  std::vector<std::vector<double>> xyz();
  std::vector<std::vector<size_t>> connectivity();
  std::vector<std::vector<double>> orthogonality();

  void read(Adcirc::Geometry::MeshFormat format = MESH_UNKNOWN);

  void write(const std::string &outputFile,
             Adcirc::Geometry::MeshFormat format = MESH_UNKNOWN);

  std::string filename() const;
  void setFilename(const std::string &filename);

  std::string meshHeaderString() const;
  void setMeshHeaderString(const std::string &meshHeaderString);

  size_t numNodes() const;
  void setNumNodes(size_t numNodes);

  size_t numElements() const;
  void setNumElements(size_t numElements);

  size_t numOpenBoundaries() const;
  void setNumOpenBoundaries(size_t numOpenBoundaries);

  size_t numLandBoundaries() const;
  void setNumLandBoundaries(size_t numLandBoundaries);

  size_t totalOpenBoundaryNodes();

  size_t totalLandBoundaryNodes();

  void setZ(std::vector<double> z);

  int projection();
  void defineProjection(int epsg, bool isLatLon);
  void reproject(int epsg);
  bool isLatLon();

  void cpp(double lambda, double phi);
  void inverseCpp(double lambda, double phi);

  void toNodeShapefile(const std::string &outputFile);
  void toConnectivityShapefile(const std::string &outputFile);
  void toElementShapefile(const std::string &outputFile);

  void buildNodalSearchTree();
  void buildElementalSearchTree();

  void deleteNodalSearchTree();
  void deleteElementalSearchTree();

  bool nodalSearchTreeInitialized();
  bool elementalSearchTreeInitialized();

  bool nodeOrderingIsLogical();
  bool elementOrderingIsLogical();

  size_t findNearestNode(Point location);
  size_t findNearestNode(double x, double y);
  size_t findNearestElement(Point location);
  size_t findNearestElement(double x, double y);
  size_t findElement(Point location);
  size_t findElement(double x, double y);

  Adcirc::Geometry::Node *node(size_t index);
  Adcirc::Geometry::Element *element(size_t index);
  Adcirc::Geometry::Boundary *openBoundary(size_t index);
  Adcirc::Geometry::Boundary *landBoundary(size_t index);

  Adcirc::Geometry::Node *nodeById(size_t id);
  Adcirc::Geometry::Element *elementById(size_t id);

  size_t nodeIndexById(size_t id);
  size_t elementIndexById(size_t id);

  void resizeMesh(size_t numNodes, size_t numElements, size_t numOpenBoundaries,
                  size_t numLandBoundaries);

  void addNode(size_t index, Adcirc::Geometry::Node &node);
  void deleteNode(size_t index);

  void addElement(size_t index, Adcirc::Geometry::Element &element);
  void deleteElement(size_t index);

  QKdtree2 *nodalSearchTree() const;
  QKdtree2 *elementalSearchTree() const;

  std::vector<double> computeMeshSize();

  void buildElementTable();

  size_t numElementsAroundNode(Adcirc::Geometry::Node *n);
  size_t numElementsAroundNode(size_t nodeIndex);
  Adcirc::Geometry::Element *elementTable(Adcirc::Geometry::Node *n,
                                          size_t listIndex);
  Adcirc::Geometry::Element *elementTable(size_t nodeIndex, size_t listIndex);
  std::vector<Adcirc::Geometry::Element *> elementsAroundNode(
      Adcirc::Geometry::Node *n);

 private:
  MeshImpl *m_impl;
};

}  // namespace Geometry

}  // namespace Adcirc

#endif  // MESH_H
