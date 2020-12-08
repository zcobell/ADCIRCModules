/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
#ifndef ADCMOD_MESHPRIVATE_H
#define ADCMOD_MESHPRIVATE_H

#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "adcircmodules_global.h"
#include "adcmap.h"
#include "boundary.h"
#include "element.h"
#include "facetable.h"
#include "filetypes.h"
#include "kdtree.h"
#include "node.h"
#include "topology.h"

using Point = std::pair<double, double>;

namespace Adcirc {
namespace Private {

class MeshPrivate {
 public:
  MeshPrivate();
  MeshPrivate(const std::string &filename);

  ~MeshPrivate();

  MeshPrivate(const MeshPrivate &m);

  MeshPrivate &operator=(const MeshPrivate &m);

  std::unique_ptr<MeshPrivate> clone() const;

  friend class Adcirc::Geometry::Mesh;

  std::vector<double> x();
  std::vector<double> y();
  std::vector<double> z();
  std::vector<std::vector<double>> xyz();
  std::vector<std::vector<size_t>> connectivity();
  std::vector<std::vector<double>> orthogonality();

  void read(
      Adcirc::Geometry::MeshFormat format = Adcirc::Geometry::MeshUnknown);

  void write(const std::string &outputFile,
             Adcirc::Geometry::MeshFormat = Adcirc::Geometry::MeshUnknown);

  std::string filename() const;
  void setFilename(const std::string &filename);

  std::string meshHeaderString() const;
  void setMeshHeaderString(const std::string &meshHeaderString);

  size_t numNodes() const;
  void setNumNodes(const size_t numNodes);

  size_t numElements() const;
  void setNumElements(const size_t numElements);

  size_t numOpenBoundaries() const;
  void setNumOpenBoundaries(const size_t numOpenBoundaries);

  size_t numLandBoundaries() const;
  void setNumLandBoundaries(const size_t numLandBoundaries);

  size_t totalOpenBoundaryNodes();

  size_t totalLandBoundaryNodes();

  void setZ(std::vector<double> &z);

  int projection();
  void defineProjection(int epsg, bool isLatLon);
  void reproject(int epsg);
  bool isLatLon();

  void cpp(double lambda, double phi);
  void inverseCpp(double lambda, double phi);

  void toNodeShapefile(const std::string &outputFile);
  void toConnectivityShapefile(const std::string &outputFile);
  void toElementShapefile(const std::string &outputFile);
  void toBoundaryShapefile(const std::string &outputFile);
  void toBoundaryLineShapefile(const std::string &outputFile,
                               const bool bothSides = false);
  void toWeirPolygonShapefile(const std::string &outputFile);

  void buildNodalSearchTree();
  void buildElementalSearchTree();

  void deleteNodalSearchTree();
  void deleteElementalSearchTree();

  bool nodalSearchTreeInitialized();
  bool elementalSearchTreeInitialized();

  bool nodeOrderingIsLogical();
  bool elementOrderingIsLogical();

  size_t findNearestNode(Point &location);
  size_t findNearestNode(double x, double y);
  size_t findNearestElement(Point &location);
  size_t findNearestElement(double x, double y);
  size_t findElement(Point &location);
  size_t findElement(double x, double y);
  size_t findElement(double x, double y, std::vector<double> &weights);

  std::vector<Adcirc::Geometry::Node *> boundaryNodes();

  Adcirc::Geometry::Node nodeC(size_t index) const;
  Adcirc::Geometry::Element elementC(size_t index) const;
  Adcirc::Geometry::Boundary openBoundaryC(size_t index) const;
  Adcirc::Geometry::Boundary landBoundaryC(size_t index) const;

  Adcirc::Geometry::Node *node(size_t index);
  Adcirc::Geometry::Element *element(size_t index);
  Adcirc::Geometry::Boundary *openBoundary(size_t index);
  Geometry::Boundary *landBoundary(size_t index);

  Adcirc::Geometry::Node *nodeById(size_t id);
  Adcirc::Geometry::Element *elementById(size_t id);

  size_t nodeIndexById(size_t id);
  size_t elementIndexById(size_t id);

  void resizeMesh(size_t numNodes, size_t numElements, size_t numOpenBoundaries,
                  size_t numLandBoundaries);

  void addNode(size_t index, const Adcirc::Geometry::Node &node);
  void addNode(size_t index, const Adcirc::Geometry::Node *node);
  void deleteNode(size_t index);

  void addElement(size_t index, const Adcirc::Geometry::Element &element);
  void deleteElement(size_t index);

  void addOpenBoundary(size_t index, const Adcirc::Geometry::Boundary &bnd);
  void deleteOpenBoundary(size_t index);

  void addLandBoundary(size_t index, const Adcirc::Geometry::Boundary &bnd);
  void deleteLandBoundary(size_t index);

  Adcirc::Kdtree *nodalSearchTree() const;
  Adcirc::Kdtree *elementalSearchTree() const;

  std::vector<double> computeMeshSize();

  std::string hash(bool force = false);

  Adcirc::Cryptography::HashType hashType() const;
  void setHashType(const Adcirc::Cryptography::HashType &hashType);

  std::vector<Adcirc::Geometry::Node> *nodes();
  std::vector<Adcirc::Geometry::Element> *elements();
  std::vector<Adcirc::Geometry::Boundary> *openBoundaries();
  std::vector<Adcirc::Geometry::Boundary> *landBoundaries();

  bool containsNode(const Geometry::Node *n, size_t &index);
  bool containsNode(const Geometry::Node &n, size_t &index);
  bool containsElement(const Adcirc::Geometry::Element *e, size_t &index);
  bool containsElement(const Adcirc::Geometry::Element &e, size_t &index);

  std::vector<double> extent() const;

  void toRaster(const std::string &filename, const std::vector<double> &z,
                const std::vector<double> &extent, const double resolution,
                const float nullvalue, const std::string &description,
                const std::string &units, const bool partialWetting = true);

  Adcirc::Geometry::Topology *topology();

 private:
  static void meshCopier(MeshPrivate *a, const MeshPrivate *b);
  static Adcirc::Geometry::MeshFormat getMeshFormat(
      const std::string &filename);
  void readAdcircMeshAscii();
  void readAdcircMeshNetcdf();
  void readAdcircMeshHeader(std::ifstream &fid);
  void readAdcircNodes(std::ifstream &fid);
  void readAdcircElements(std::ifstream &fid);
  void readAdcircOpenBoundaries(std::ifstream &fid);
  void readAdcircLandBoundaries(std::ifstream &fid);

  void read2dmMesh();
  void read2dmData(std::vector<std::string> &nodes,
                   std::vector<std::string> &elements);
  void read2dmNodes(std::vector<std::string> &nodes);
  void read2dmElements(std::vector<std::string> &elements);

  void readDflowMesh();

  void _init();

  void writeAdcircMesh(const std::string &filename);
  void write2dmMesh(const std::string &filename);
  void writeDflowMesh(const std::string &filename);

  std::vector<std::pair<Adcirc::Geometry::Node *, Adcirc::Geometry::Node *>>
  generateLinkTable();

  size_t getMaxNodesPerElement();
  void buildNodeLookupTable();

  void generateHash(bool force = false);

  void writePrjFile(const std::string &outputFile);

  Adcirc::adcmap<size_t, size_t> m_nodeLookup;
  Adcirc::adcmap<size_t, size_t> m_elementLookup;

  Adcirc::Cryptography::HashType m_hashType;

  std::string m_filename;
  std::string m_meshHeaderString;
  std::unique_ptr<char[]> m_hash;
  std::vector<Adcirc::Geometry::Node> m_nodes;
  std::vector<Adcirc::Geometry::Element> m_elements;
  std::vector<Adcirc::Geometry::Boundary> m_openBoundaries;
  std::vector<Adcirc::Geometry::Boundary> m_landBoundaries;
  int m_epsg;
  bool m_isLatLon;

  bool m_nodeOrderingLogical;
  bool m_elementOrderingLogical;

  std::unique_ptr<Adcirc::Geometry::Topology> m_topology;
  std::unique_ptr<Kdtree> m_nodalSearchTree;
  std::unique_ptr<Kdtree> m_elementalSearchTree;

  std::vector<float> getRasterValues(
      const std::vector<double> &z, const double nullvalue,
      const std::vector<size_t> &elements,
      const std::vector<std::vector<double>> &weights,
      const bool partialWetting = false);

  std::pair<std::vector<std::vector<double>>, std::vector<size_t>>
  computeRasterInterpolationWeights(const std::vector<double> &extent,
                                    const size_t nx, const size_t ny,
                                    const double &resolution);

  static std::pair<double, double> pixelToCoordinate(const size_t i,
                                                     const size_t j,
                                                     const double resolution,
                                                     const double xmin,
                                                     const double ymax);

  float calculateValueWithoutPartialWetting(const double v1, const double v2,
                                            const double v3,
                                            const double nullvalue,
                                            const std::vector<double> &weight);
  float calculateValueWithPartialWetting(const double v1, const double v2,
                                         const double v3,
                                         const double nullvalue,
                                         const std::vector<double> &weight);
};
}  // namespace Private
}  // namespace Adcirc

#endif  // ADCMOD_MESHPRIVATE_H
