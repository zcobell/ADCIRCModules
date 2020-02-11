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
#include "mesh.h"
#include "mesh_private.h"

using namespace Adcirc::Geometry;

using Point = std::pair<double, double>;

Mesh::Mesh() : m_impl(new Adcirc::Private::MeshPrivate) {}

Mesh::Mesh(const std::string &filename)
    : m_impl(new Adcirc::Private::MeshPrivate(filename)) {}

Mesh &Mesh::operator=(const Mesh &m) {
  this->m_impl.reset(m.m_impl->clone());
  return *this;
}

Mesh::Mesh(const Mesh &m) { this->m_impl.reset(m.m_impl->clone()); }

/**
 * @brief Returns a vector of the x-coordinates
 * @return vector of x coordinates
 *
 * Implemented mostly for the python interface
 */
std::vector<double> Mesh::x() { return this->m_impl->x(); }

/**
 * @brief Returns a vector of the y-coordinates
 * @return vector of y coordinates
 *
 * Implemented mostly for the python interface
 */
std::vector<double> Mesh::y() { return this->m_impl->y(); }

/**
 * @brief Returns a vector of the z-coordinates
 * @return vector of z coordinates
 *
 * Implemented mostly for the python interface
 */
std::vector<double> Mesh::z() { return this->m_impl->z(); }

/**
 * @brief Returns a 2d-vector of the xyz-coordinates
 * @return 2d-vector of xyz coordinates
 *
 * Implemented mostly for the python interface
 */
std::vector<std::vector<double>> Mesh::xyz() { return this->m_impl->xyz(); }

/**
 * @brief Returns a 2d-vector of the mesh connectivity
 * @return 2d-vector of mesh connectivity
 *
 * Implemented mostly for the python interface
 */
std::vector<std::vector<size_t>> Mesh::connectivity() {
  return this->m_impl->connectivity();
}

/**
 * @brief Calculates the element orthogonality
 * @return vector containing orthogonality values between 0 and 1 and the x, y
 * of the midpoint for the element edge
 *
 * Orthogonality is the angle between an element edge and the a line
 * between the two element centers on either side of the edge. Orthogonality
 * is measured as deviation from cos(0) and is important in finite volume
 * calculations
 */
std::vector<std::vector<double>> Mesh::orthogonality() {
  return this->m_impl->orthogonality();
}

/**
 * @brief Reads a specified mesh format
 * @param[optional] format MeshFormat enum describing the format of the mesh
 *
 * Reads the unstructured mesh into a mesh object. If no format is
 * specified, then it will be guessed from the file extension
 */
void Mesh::read(Adcirc::Geometry::MeshFormat format) {
  this->m_impl->read(format);
}

/**
 * @brief Writes an Mesh object to disk in ASCII format
 * @param[in] filename name of the output file to write
 * @param[in] format format specified for the output file
 *
 * If no output specifier is supplied, format is guessed from file extension.
 */
void Mesh::write(const std::string &outputFile,
                 Adcirc::Geometry::MeshFormat format) {
  this->m_impl->write(outputFile, format);
}

/**
 * @brief Filename of the mesh to be read
 * @return Return the name of the mesh to be read
 */
std::string Mesh::filename() const { return this->m_impl->filename(); }

/**
 * @brief Sets the name of the mesh to be read
 * @param[in] filename Name of the mesh
 */
void Mesh::setFilename(const std::string &filename) {
  this->m_impl->setFilename(filename);
}

/**
 * @brief Returns the mesh header from the processed mesh
 * @return mesh header
 */
std::string Mesh::meshHeaderString() const {
  return this->m_impl->meshHeaderString();
}

/**
 * @brief Sets the header for the mesh
 * @param[in] meshHeaderString header
 */
void Mesh::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_impl->setMeshHeaderString(meshHeaderString);
}

/**
 * @brief Returns the number of nodes currently in the mesh
 * @return number of nodes
 */
size_t Mesh::numNodes() const { return this->m_impl->numNodes(); }

/**
 * @brief Sets the number of nodes in the mesh. Note this does not resize the
 * mesh
 * @param[in] numNodes number of nodes
 */
void Mesh::setNumNodes(size_t numNodes) { this->m_impl->setNumNodes(numNodes); }

/**
 * @brief Returns the number of elements in the mesh
 * @return number of elements
 */
size_t Mesh::numElements() const { return this->m_impl->numElements(); }

/**
 * @brief Sets the number of elements in the mesh. Note: This does not resize
 * the mesh
 * @param[in] numElements Number of elements
 */
void Mesh::setNumElements(size_t numElements) {
  this->m_impl->setNumElements(numElements);
}

/**
 * @brief Returns the number of open boundaries in the model
 * @return number of open boundaries
 */
size_t Mesh::numOpenBoundaries() const {
  return this->m_impl->numOpenBoundaries();
}

/**
 * @brief Sets the number of open boundaries in the model. Note this does not
 * resize the boundary array
 * @param[in] numOpenBoundaries Number of open boundaries
 */
void Mesh::setNumOpenBoundaries(size_t numOpenBoundaries) {
  this->m_impl->setNumOpenBoundaries(numOpenBoundaries);
}

/**
 * @brief Returns the number of land boundaries in the model
 * @return number of land boundaries
 */
size_t Mesh::numLandBoundaries() const {
  return this->m_impl->numLandBoundaries();
}

/**
 * @brief Sets the number of land boundaries in the model. Note this does not
 * resize the boundary array
 * @param[in] numLandBoundaries Number of land boundaries
 */
void Mesh::setNumLandBoundaries(size_t numLandBoundaries) {
  this->m_impl->setNumLandBoundaries(numLandBoundaries);
}

/**
 * @brief Returns the number of open boundary nodes in the mesh
 * @return Number of open boundary nodes
 */
size_t Mesh::totalOpenBoundaryNodes() {
  return this->m_impl->totalOpenBoundaryNodes();
}

/**
 * @brief Returns the number of land boundary nodes in the mesh
 * @return Number of nodes that fall on a land boundary
 */
size_t Mesh::totalLandBoundaryNodes() {
  return this->m_impl->totalLandBoundaryNodes();
}

/**
 * @brief Sets the z-values of the mesh to the values found within a vector
 * @param[in] z values that will be set to the mesh nodes z attribute
 */
void Mesh::setZ(std::vector<double> z) { this->m_impl->setZ(z); }

/**
 * @brief Returns the EPSG code for the current mesh projection
 * @return EPSG code
 */
int Mesh::projection() { return this->m_impl->projection(); }

/**
 * @brief Sets the mesh projection using an EPSG code. Note this does not
 * reproject the mesh.
 * @param[in] epsg EPSG code for the mesh
 * @param[in] isLatLon defines if the current projection is a lat/lon
 * projection. This helps define the significant digits when writing the mesh
 * file
 */
void Mesh::defineProjection(int epsg, bool isLatLon) {
  this->m_impl->defineProjection(epsg, isLatLon);
}

/**
 * @brief Reprojects a mesh into the specified projection
 * @param[in] epsg EPSG coordinate system to convert the mesh into
 */
void Mesh::reproject(int epsg) { this->m_impl->reproject(epsg); }

/**
 * @brief Returns true if the mesh is in a geographic projection
 * @return boolean value for mesh projection type
 */
bool Mesh::isLatLon() { return this->m_impl->isLatLon(); }

/**
 * @brief Convertes mesh to the carte parallelogrammatique projection
 * @param[in] lambda  lambda used in cpp projection
 * @param[in] phi phi used in cpp projection
 *
 * This is the projection used within adcirc internally
 */
void Mesh::cpp(double lambda, double phi) { this->m_impl->cpp(lambda, phi); }

/**
 * @brief Convertes mesh back from the carte parallelogrammatique projection
 * @param[in] lambda  lambda used in cpp projection
 * @param[in] phi phi used in cpp projection
 */
void Mesh::inverseCpp(double lambda, double phi) {
  this->m_impl->inverseCpp(lambda, phi);
}

/**
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toNodeShapefile(const std::string &outputFile) {
  this->m_impl->toNodeShapefile(outputFile);
}

/**
 * @brief Writes the mesh connectivity into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toConnectivityShapefile(const std::string &outputFile) {
  this->m_impl->toConnectivityShapefile(outputFile);
}

/**
 * @brief Writes the mesh elements as polygons into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toElementShapefile(const std::string &outputFile) {
  this->m_impl->toElementShapefile(outputFile);
}

/**
 * @brief Writes the boundary conditions of the mesh to ESRI shapefile format as
 * points
 * @param[in] outputFile output file with .shp extension
 */
void Mesh::toBoundaryShapefile(const std::string &outputFile) {
  this->m_impl->toBoundaryShapefile(outputFile);
}

/**
 * @brief Builds a kd-tree object with the mesh nodes as the search locations
 */
void Mesh::buildNodalSearchTree() { this->m_impl->buildNodalSearchTree(); }

/**
 * @brief Builds a kd-tree object with the element centers as the search
 * locations
 */
void Mesh::buildElementalSearchTree() {
  this->m_impl->buildElementalSearchTree();
}

/**
 * @brief Deletes the nodal search tree
 */
void Mesh::deleteNodalSearchTree() { this->m_impl->deleteNodalSearchTree(); }

/**
 * @brief Deletes the elemental search tree
 */
void Mesh::deleteElementalSearchTree() {
  this->m_impl->deleteElementalSearchTree();
}

/**
 * @brief Returns a boolean value determining if the nodal search tree has
 * been initialized
 * @return true if the search tree is initialized
 */
bool Mesh::nodalSearchTreeInitialized() {
  return this->m_impl->nodalSearchTreeInitialized();
}

/**
 * @brief Returns a boolean value determining if the elemental search tree has
 * been initialized
 * @return true of the search tree is initialized
 */
bool Mesh::elementalSearchTreeInitialized() {
  return this->m_impl->elementalSearchTreeInitialized();
}

/**
 * @brief Allows the user to know if the code has determined that the node
 * ordering is logcical (i.e. sequential) or not
 * @return true if node ordering is sequential
 */
bool Mesh::nodeOrderingIsLogical() {
  return this->m_impl->nodeOrderingIsLogical();
}

/**
 * @brief Allows the user to know if the code has determined that the element
 * ordering is logcical (i.e. sequential) or not
 * @return true if element ordering is sequential
 */
bool Mesh::elementOrderingIsLogical() {
  return this->m_impl->elementOrderingIsLogical();
}

/**
 * @brief Finds the nearest mesh node to the provided x,y
 * @param[in] x location to search
 * @param[in] y location to search
 * @return nearest node index
 */
size_t Mesh::findNearestNode(Point &location) {
  return this->m_impl->findNearestNode(location);
}

/**
 * @param[in] location location to search
 * @return nearest node index
 */
size_t Mesh::findNearestNode(double x, double y) {
  return this->m_impl->findNearestNode(x, y);
}

/**
 * @brief Finds the nearest mesh element to the provided x,y
 * @param[in] x location to search
 * @param[in] y location to search
 * @return nearest element index
 */
size_t Mesh::findNearestElement(Point &location) {
  return this->m_impl->findNearestElement(location);
}

/**
 * @param[in] location location to search
 * @return nearest element index
 */
size_t Mesh::findNearestElement(double x, double y) {
  return this->m_impl->findNearestElement(x, y);
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param[in] x location to search
 * @param[in] y location to search
 * @return index of nearest element, large integer if not found
 */
size_t Mesh::findElement(std::pair<double, double> location) {
  return this->m_impl->findElement(location);
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param[in] location location to search
 * @return index of nearest element, large integer if not found
 */
size_t Mesh::findElement(double x, double y) {
  return this->m_impl->findElement(x, y);
}

/**
 * @brief Mesh::findElement
 * @param[in] x location to search
 * @param[in] y location to search
 * @param[out] weights barycentric weights for interpolation [triangles only]
 * @return index of nearest element, large integer if not found
 */
size_t Mesh::findElement(double x, double y, std::vector<double> &weights) {
  return this->m_impl->findElement(x, y, weights);
}

/**
 * @brief Returns a pointer to the requested node in the internal node vector
 * @param[in] index location of the node in the vector
 * @return Node pointer
 */
Adcirc::Geometry::Node *Mesh::node(size_t index) {
  return this->m_impl->node(index);
}

/**
 * @brief Returns a pointer to the requested element in the internal element
 * vector
 * @param[in] index location of the node in the vector
 * @return Element pointer
 */
Adcirc::Geometry::Element *Mesh::element(size_t index) {
  return this->m_impl->element(index);
}

/**
 * @brief Returns a pointer to an open boundary by index
 * @param[in] index index in the open boundary array
 * @return Boundary pointer
 */
Adcirc::Geometry::Boundary *Mesh::openBoundary(size_t index) {
  return this->m_impl->openBoundary(index);
}

/**
 * @brief Returns a pointer to a land boundary by index
 * @param[in] index index in the land boundary array
 * @return Boundary pointer
 */
Adcirc::Geometry::Boundary *Mesh::landBoundary(size_t index) {
  return this->m_impl->landBoundary(index);
}

/**
 * @brief Returns a pointer to the requested node by its ID
 * @param[in] id Nodal ID to return a reference to
 * @return Node pointer
 */
Adcirc::Geometry::Node *Mesh::nodeById(size_t id) {
  return this->m_impl->nodeById(id);
}

/**
 * @brief Returns an pointer to the requested element by its ID
 * @param[in] id Elemental ID to return a reference to
 * @return Element pointer
 */
Adcirc::Geometry::Element *Mesh::elementById(size_t id) {
  return this->m_impl->elementById(id);
}

/**
 * @brief Returns the position in the array by node id
 * @param[in] id nodal id
 * @return array position
 */
size_t Mesh::nodeIndexById(size_t id) {
  return this->m_impl->nodeIndexById(id);
}

/**
 * @brief Returns the position in the array by element id
 * @param[in] id element id
 * @return array position
 */
size_t Mesh::elementIndexById(size_t id) {
  return this->m_impl->elementIndexById(id);
}

/**
 * @brief Resizes the vectors within the mesh
 * @param[in] numNodes Number of nodes
 * @param[in] numElements Number of elements
 * @param[in] numOpenBoundaries Number of open boundaries
 * @param[in] numLandBoundaries Number of land boundaries
 */
void Mesh::resizeMesh(size_t numNodes, size_t numElements,
                      size_t numOpenBoundaries, size_t numLandBoundaries) {
  this->m_impl->resizeMesh(numNodes, numElements, numOpenBoundaries,
                           numLandBoundaries);
}

/**
 * @brief Adds a node at the specified index in the node vector
 * @param[in] index location where the node should be added
 * @param[in] node Reference to an Node object
 */
void Mesh::addNode(size_t index, const Node &node) {
  return this->m_impl->addNode(index, node);
}
void Mesh::addNode(size_t index, const Node *node) {
  return this->m_impl->addNode(index, node);
}

/**
 * @brief Deletes an Node object at the specified index and shifts the
 * remaining nodes forward in the vector
 * @param[in] index location where the node should be deleted from
 */
void Mesh::deleteNode(size_t index) { return this->m_impl->deleteNode(index); }

/**
 * @brief Adds an Element at the specified position in the element vector
 * @param[in] index location where the element should be added
 * @param[in] element reference to the Element to add
 */
void Mesh::addElement(size_t index, const Element &element) {
  this->m_impl->addElement(index, element);
}

/**
 * @brief Deletes an Element object at the specified index and shifts the
 * remaining elements forward in the vector
 * @param[in] index location where the element should be deleted from
 */
void Mesh::deleteElement(size_t index) { this->m_impl->deleteElement(index); }

/**
 * @brief Adds a land boundary to the mesh
 * @param[in] index position in boundary array
 * @param[in] bnd Adcirc::Geometry::Boundary object
 */
void Mesh::addLandBoundary(size_t index, const Boundary &bnd) {
  this->m_impl->addLandBoundary(index, bnd);
}

/**
 * @brief Removes the land boundary at the specified position
 * @param[in] index position of boundary to remove
 */
void Mesh::deleteLandBoundary(size_t index) {
  this->m_impl->deleteLandBoundary(index);
}

/**
 * @brief Adds an open boundary to the mesh
 * @param[in] index position in boundary array
 * @param[in] bnd Adcirc::Geometry::Boundary object
 */
void Mesh::addOpenBoundary(size_t index, const Boundary &bnd) {
  this->m_impl->addOpenBoundary(index, bnd);
}

/**
 * @brief Removes the open boundary at the specified position
 * @param[in] index position of boundary to remove
 */
void Mesh::deleteOpenBoundary(size_t index) {
  this->m_impl->deleteOpenBoundary(index);
}

/**
 * @brief Returns a refrence to the nodal search kd-tree
 * @return kd-tree object with mesh nodes as serch locations
 */
Adcirc::Kdtree *Mesh::nodalSearchTree() const {
  return this->m_impl->nodalSearchTree();
}

/**
 * @brief Returns a reference to the elemental search kd-tree
 * @return kd-tree object with element centers as search locations
 */
Adcirc::Kdtree *Mesh::elementalSearchTree() const {
  return this->m_impl->elementalSearchTree();
}

/**
 * @brief Computes average size of the element edges connected to each node
 * @return vector containing size at each node
 */
std::vector<double> Mesh::computeMeshSize() {
  return this->m_impl->computeMeshSize();
}

/**
 * @brief Builds a list of elements connected to each node
 */
void Mesh::buildElementTable() { this->m_impl->buildElementTable(); }

/**
 * @brief Returns the number of elements surrounding a specified node
 * @param[in] n address of node
 * @return number of elements
 */
size_t Mesh::numElementsAroundNode(Adcirc::Geometry::Node *n) {
  return this->m_impl->numElementsAroundNode(n);
}

/**
 * @brief Returns the number of elements surrounding a specified node
 * @param[in] n index of node
 * @return number of elements
 */
size_t Mesh::numElementsAroundNode(size_t nodeIndex) {
  return this->m_impl->numElementsAroundNode(nodeIndex);
}

/**
 * @brief Returns the nth element in the list of elements surrounding a node
 * @param[in] n node pointer
 * @param[in] listIndex index of the element to return
 * @return element pointer
 */
Adcirc::Geometry::Element *Mesh::elementTable(Adcirc::Geometry::Node *n,
                                              size_t listIndex) {
  return this->m_impl->elementTable(n, listIndex);
}

/**
 * @brief Returns the nth element in the list of elements surrounding a node
 * @param[in] n node index
 * @param[in] listIndex index of the element to return
 * @return element pointer
 */
Adcirc::Geometry::Element *Mesh::elementTable(size_t nodeIndex,
                                              size_t listIndex) {
  return this->m_impl->elementTable(nodeIndex, listIndex);
}

/**
 * @brief Returns a vector of pointers to the elements surrouding a node
 * @param[in] n node pointer
 * @return vector of element references
 */
std::vector<Adcirc::Geometry::Element *> Mesh::elementsAroundNode(
    Adcirc::Geometry::Node *n) {
  return this->m_impl->elementsAroundNode(n);
}

/**
 * @brief Return a vector containing all nodes on the mesh boundary
 *
 * Note that the nodes returned are not nodes with boundary conditions, but
 * nodes that are on the mesh boundary
 *
 * @return vector of all nodes on boundary
 */
std::vector<Adcirc::Geometry::Node *> Mesh::boundaryNodes() {
  return this->m_impl->boundaryNodes();
}

/**
 * @brief Returns the hash of the mesh which serves as a unique identifier
 * @return hash as string
 */
std::string Mesh::hash(bool force) { return this->m_impl->hash(force); }

/**
 * @brief Gets the hash type that will be computed
 * @return hash type
 */
Adcirc::Cryptography::HashType Mesh::hashType() const {
  return this->m_impl->hashType();
}

/**
 * @brief Sets the hash type to be used
 * @param[in] hashType hash type to use
 */
void Mesh::setHashType(const Adcirc::Cryptography::HashType &hashType) {
  this->m_impl->setHashType(hashType);
}

/**
 * @brief Returns the vector of nodes in the mesh
 * @return pointer to vector of nodes
 */
std::vector<Adcirc::Geometry::Node> *Mesh::nodes() {
  return this->m_impl->nodes();
}

/**
 * @brief Returns the vector of elements in the mesh
 * @return pointer to vector of elements
 */
std::vector<Adcirc::Geometry::Element> *Mesh::elements() {
  return this->m_impl->elements();
}

/**
 * @brief Returns the vector of open boundaries in the mesh
 * @return pointer to vector of open boundaries
 */
std::vector<Adcirc::Geometry::Boundary> *Mesh::openBoundaries() {
  return this->m_impl->openBoundaries();
}

/**
 * @brief Returns the vector of land boundaries in the mesh
 * @return pointer to vector of land boundaries
 */
std::vector<Adcirc::Geometry::Boundary> *Mesh::landBoundaries() {
  return this->m_impl->landBoundaries();
}

bool Mesh::containsNode(Adcirc::Geometry::Node &n, size_t &index) {
  return this->m_impl->containsNode(n, index);
}

bool Mesh::containsNode(Adcirc::Geometry::Node *n, size_t &index) {
  return this->m_impl->containsNode(n, index);
}

bool Mesh::containsElement(Adcirc::Geometry::Element &e, size_t &index) {
  return this->m_impl->containsElement(e, index);
}

bool Mesh::containsElement(Adcirc::Geometry::Element *e, size_t &index) {
  return this->m_impl->containsElement(e, index);
}
