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
#ifndef ADCMOD_MESH_H
#define ADCMOD_MESH_H

#include <limits>
#include <memory>
#include <string>
#include <vector>
#include "adcircmodules_global.h"
#include "boundary.h"
#include "element.h"
#include "filetypes.h"
#include "node.h"

class Kdtree;
class MeshImpl;

using Point = std::pair<double, double>;

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
  ADCIRCMODULES_EXPORT Mesh();
  ADCIRCMODULES_EXPORT Mesh(const std::string &filename);
  ADCIRCMODULES_EXPORT ~Mesh();

  friend class ElementTable;

  std::vector<double> ADCIRCMODULES_EXPORT x();
  std::vector<double> ADCIRCMODULES_EXPORT y();
  std::vector<double> ADCIRCMODULES_EXPORT z();
  std::vector<std::vector<double>> ADCIRCMODULES_EXPORT xyz();
  std::vector<std::vector<size_t>> ADCIRCMODULES_EXPORT connectivity();
  std::vector<std::vector<double>> ADCIRCMODULES_EXPORT orthogonality();

  void ADCIRCMODULES_EXPORT
  read(Adcirc::Geometry::MeshFormat format = MeshUnknown);

  void ADCIRCMODULES_EXPORT
  write(const std::string &outputFile,
        Adcirc::Geometry::MeshFormat format = MeshUnknown);

  std::string ADCIRCMODULES_EXPORT filename() const;
  void ADCIRCMODULES_EXPORT setFilename(const std::string &filename);

  std::string ADCIRCMODULES_EXPORT meshHeaderString() const;
  void ADCIRCMODULES_EXPORT
  setMeshHeaderString(const std::string &meshHeaderString);

  size_t ADCIRCMODULES_EXPORT numNodes() const;
  void ADCIRCMODULES_EXPORT setNumNodes(size_t numNodes);

  size_t ADCIRCMODULES_EXPORT numElements() const;
  void ADCIRCMODULES_EXPORT setNumElements(size_t numElements);

  size_t ADCIRCMODULES_EXPORT numOpenBoundaries() const;
  void ADCIRCMODULES_EXPORT setNumOpenBoundaries(size_t numOpenBoundaries);

  size_t ADCIRCMODULES_EXPORT numLandBoundaries() const;
  void ADCIRCMODULES_EXPORT setNumLandBoundaries(size_t numLandBoundaries);

  size_t ADCIRCMODULES_EXPORT totalOpenBoundaryNodes();

  size_t ADCIRCMODULES_EXPORT totalLandBoundaryNodes();

  void ADCIRCMODULES_EXPORT setZ(std::vector<double> z);

  int ADCIRCMODULES_EXPORT projection();
  void ADCIRCMODULES_EXPORT defineProjection(int epsg, bool isLatLon);
  void ADCIRCMODULES_EXPORT reproject(int epsg);
  bool ADCIRCMODULES_EXPORT isLatLon();

  void ADCIRCMODULES_EXPORT cpp(double lambda, double phi);
  void ADCIRCMODULES_EXPORT inverseCpp(double lambda, double phi);

  void ADCIRCMODULES_EXPORT toNodeShapefile(const std::string &outputFile);
  void ADCIRCMODULES_EXPORT
  toConnectivityShapefile(const std::string &outputFile);
  void ADCIRCMODULES_EXPORT toElementShapefile(const std::string &outputFile);

  void ADCIRCMODULES_EXPORT buildNodalSearchTree();
  void ADCIRCMODULES_EXPORT buildElementalSearchTree();

  void ADCIRCMODULES_EXPORT deleteNodalSearchTree();
  void ADCIRCMODULES_EXPORT deleteElementalSearchTree();

  bool ADCIRCMODULES_EXPORT nodalSearchTreeInitialized();
  bool ADCIRCMODULES_EXPORT elementalSearchTreeInitialized();

  bool ADCIRCMODULES_EXPORT nodeOrderingIsLogical();
  bool ADCIRCMODULES_EXPORT elementOrderingIsLogical();

  size_t ADCIRCMODULES_EXPORT findNearestNode(Point &location);
  size_t ADCIRCMODULES_EXPORT findNearestNode(double x, double y);
  size_t ADCIRCMODULES_EXPORT findNearestElement(Point &location);
  size_t ADCIRCMODULES_EXPORT findNearestElement(double x, double y);
  size_t ADCIRCMODULES_EXPORT findElement(Point location);
  size_t ADCIRCMODULES_EXPORT findElement(double x, double y);
  size_t ADCIRCMODULES_EXPORT findElement(double x, double y,
                                          std::vector<double> &weights);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node(size_t index);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *element(size_t index);
  Adcirc::Geometry::Boundary ADCIRCMODULES_EXPORT *openBoundary(size_t index);
  Adcirc::Geometry::Boundary ADCIRCMODULES_EXPORT *landBoundary(size_t index);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *nodeById(size_t id);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *elementById(size_t id);

  size_t ADCIRCMODULES_EXPORT nodeIndexById(size_t id);
  size_t ADCIRCMODULES_EXPORT elementIndexById(size_t id);

  void ADCIRCMODULES_EXPORT resizeMesh(size_t numNodes, size_t numElements,
                                       size_t numOpenBoundaries,
                                       size_t numLandBoundaries);

  void ADCIRCMODULES_EXPORT addNode(size_t index, Adcirc::Geometry::Node node);
  void ADCIRCMODULES_EXPORT deleteNode(size_t index);

  void ADCIRCMODULES_EXPORT addElement(size_t index,
                                       Adcirc::Geometry::Element &element);
  void ADCIRCMODULES_EXPORT deleteElement(size_t index);

  Kdtree ADCIRCMODULES_EXPORT *nodalSearchTree() const;
  Kdtree ADCIRCMODULES_EXPORT *elementalSearchTree() const;

  std::vector<double> ADCIRCMODULES_EXPORT computeMeshSize();

  void ADCIRCMODULES_EXPORT buildElementTable();

  size_t ADCIRCMODULES_EXPORT numElementsAroundNode(Adcirc::Geometry::Node *n);
  size_t ADCIRCMODULES_EXPORT numElementsAroundNode(size_t nodeIndex);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *elementTable(
      Adcirc::Geometry::Node *n, size_t listIndex);
  Adcirc::Geometry::Element ADCIRCMODULES_EXPORT *elementTable(
      size_t nodeIndex, size_t listIndex);
  std::vector<Adcirc::Geometry::Element *> ADCIRCMODULES_EXPORT
  elementsAroundNode(Adcirc::Geometry::Node *n);

  std::vector<Adcirc::Geometry::Node *> ADCIRCMODULES_EXPORT boundaryNodes();

  std::string ADCIRCMODULES_EXPORT hash(bool force = false);

  HashType hashType() const;
  void setHashType(const HashType &hashType);

 private:
  std::unique_ptr<MeshImpl> m_impl;
};

}  // namespace Geometry

}  // namespace Adcirc

#endif  // ADCMOD_MESH_H
