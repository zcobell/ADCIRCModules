#include "mesh.h"
#include "meshimpl.h"

using namespace Adcirc::Geometry;

Mesh::Mesh() : m_impl(new MeshImpl) {}

Mesh::Mesh(const std::string &filename) : m_impl(new MeshImpl(filename)) {}

Mesh::~Mesh() { delete this->m_impl; }

std::vector<double> Mesh::x() { return std::move(this->m_impl->x()); }

std::vector<double> Mesh::y() { return std::move(this->m_impl->y()); }

std::vector<double> Mesh::z() { return std::move(this->m_impl->z()); }

std::vector<std::vector<double>> Mesh::xyz() {
  return std::move(this->m_impl->xyz());
}

std::vector<std::vector<size_t>> Mesh::connectivity() {
  return std::move(this->m_impl->connectivity());
}

std::vector<std::vector<double>> Mesh::orthogonality() {
  return std::move(this->m_impl->orthogonality());
}

void Mesh::read(Adcirc::Geometry::MeshFormat format) {
  this->m_impl->read(format);
}

void Mesh::write(const std::string &outputFile,
                 Adcirc::Geometry::MeshFormat format) {
  this->m_impl->write(outputFile, format);
}

std::string Mesh::filename() const { return std::move(this->m_impl->filename()); }

void Mesh::setFilename(const std::string &filename) {
  this->m_impl->setFilename(filename);
}

std::string Mesh::meshHeaderString() const {
  return std::move(this->m_impl->meshHeaderString());
}

void Mesh::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_impl->setMeshHeaderString(meshHeaderString);
}

size_t Mesh::numNodes() const { return this->m_impl->numNodes(); }

void Mesh::setNumNodes(size_t numNodes) { this->m_impl->setNumNodes(numNodes); }

size_t Mesh::numElements() const { return this->m_impl->numElements(); }

void Mesh::setNumElements(size_t numElements) {
  this->m_impl->setNumElements(numElements);
}

size_t Mesh::numOpenBoundaries() const {
  return this->m_impl->numOpenBoundaries();
}

void Mesh::setNumOpenBoundaries(size_t numOpenBoundaries) {
  this->m_impl->setNumOpenBoundaries(numOpenBoundaries);
}

size_t Mesh::numLandBoundaries() const {
  return this->m_impl->numLandBoundaries();
}

void Mesh::setNumLandBoundaries(size_t numLandBoundaries) {
  this->m_impl->setNumLandBoundaries(numLandBoundaries);
}

size_t Mesh::totalOpenBoundaryNodes() {
  return this->m_impl->totalOpenBoundaryNodes();
}

size_t Mesh::totalLandBoundaryNodes() {
  return this->m_impl->totalLandBoundaryNodes();
}

void Mesh::setZ(std::vector<double> z) { this->m_impl->setZ(z); }

int Mesh::projection() { return this->m_impl->projection(); }

void Mesh::defineProjection(int epsg, bool isLatLon) {
  this->m_impl->defineProjection(epsg, isLatLon);
}

void Mesh::reproject(int epsg) { this->m_impl->reproject(epsg); }

bool Mesh::isLatLon() { return this->m_impl->isLatLon(); }

void Mesh::cpp(double lambda, double phi) { this->m_impl->cpp(lambda, phi); }

void Mesh::inverseCpp(double lambda, double phi) {
  this->m_impl->inverseCpp(lambda, phi);
}

void Mesh::toNodeShapefile(const std::string &outputFile) {
  this->m_impl->toNodeShapefile(outputFile);
}

void Mesh::toConnectivityShapefile(const std::string &outputFile) {
  this->m_impl->toConnectivityShapefile(outputFile);
}

void Mesh::toElementShapefile(const std::string &outputFile) {
  this->m_impl->toElementShapefile(outputFile);
}

void Mesh::buildNodalSearchTree() { this->m_impl->buildNodalSearchTree(); }

void Mesh::buildElementalSearchTree() {
  this->m_impl->buildElementalSearchTree();
}

void Mesh::deleteNodalSearchTree() { this->m_impl->deleteNodalSearchTree(); }

void Mesh::deleteElementalSearchTree() {
  this->m_impl->deleteElementalSearchTree();
}

bool Mesh::nodalSearchTreeInitialized() {
  return this->m_impl->nodalSearchTreeInitialized();
}

bool Mesh::elementalSearchTreeInitialized() {
  return this->m_impl->elementalSearchTreeInitialized();
}

bool Mesh::nodeOrderingIsLogical() {
  return this->m_impl->nodeOrderingIsLogical();
}

bool Mesh::elementOrderingIsLogical() {
  return this->m_impl->elementOrderingIsLogical();
}

size_t Mesh::findNearestNode(Point location) {
  return this->m_impl->findNearestNode(location);
}

size_t Mesh::findNearestNode(double x, double y) {
  return this->m_impl->findNearestNode(x, y);
}

size_t Mesh::findNearestElement(Point location) {
  return this->m_impl->findNearestElement(location);
}

size_t Mesh::findNearestElement(double x, double y) {
  return this->m_impl->findNearestElement(x, y);
}

size_t Mesh::findElement(Point location) {
  return this->m_impl->findElement(location);
}

size_t Mesh::findElement(double x, double y) {
  return this->m_impl->findElement(x, y);
}

Adcirc::Geometry::Node *Mesh::node(size_t index) {
  return this->m_impl->node(index);
}

Adcirc::Geometry::Element *Mesh::element(size_t index) {
  return this->m_impl->element(index);
}

Adcirc::Geometry::Boundary *Mesh::openBoundary(size_t index) {
  return this->m_impl->openBoundary(index);
}

Adcirc::Geometry::Boundary *Mesh::landBoundary(size_t index) {
  return this->m_impl->landBoundary(index);
}

Adcirc::Geometry::Node *Mesh::nodeById(size_t id) {
  return this->m_impl->nodeById(id);
}

Adcirc::Geometry::Element *Mesh::elementById(size_t id) {
  return this->m_impl->elementById(id);
}

size_t Mesh::nodeIndexById(size_t id) {
  return this->m_impl->nodeIndexById(id);
}

size_t Mesh::elementIndexById(size_t id) {
  return this->m_impl->elementIndexById(id);
}

void Mesh::resizeMesh(size_t numNodes, size_t numElements,
                      size_t numOpenBoundaries, size_t numLandBoundaries) {
  this->m_impl->resizeMesh(numNodes, numElements, numOpenBoundaries,
                           numLandBoundaries);
}

void Mesh::addNode(size_t index, Adcirc::Geometry::Node &node) {
  return this->m_impl->addNode(index, node);
}

void Mesh::deleteNode(size_t index) { return this->m_impl->deleteNode(index); }

void Mesh::addElement(size_t index, Adcirc::Geometry::Element &element) {
  this->m_impl->addElement(index, element);
}

void Mesh::deleteElement(size_t index) { this->m_impl->deleteElement(index); }

QKdtree2 *Mesh::nodalSearchTree() const {
  return this->m_impl->nodalSearchTree();
}

QKdtree2 *Mesh::elementalSearchTree() const {
  return this->m_impl->elementalSearchTree();
}

std::vector<double> Mesh::computeMeshSize() {
  return std::move(this->m_impl->computeMeshSize());
}

void Mesh::buildElementTable() { this->m_impl->buildElementTable(); }

size_t Mesh::numElementsAroundNode(Adcirc::Geometry::Node *n) {
  return this->m_impl->numElementsAroundNode(n);
}

size_t Mesh::numElementsAroundNode(size_t nodeIndex) {
  return this->m_impl->numElementsAroundNode(nodeIndex);
}

Adcirc::Geometry::Element *Mesh::elementTable(Adcirc::Geometry::Node *n,
                                              size_t listIndex) {
  return this->m_impl->elementTable(n, listIndex);
}

Adcirc::Geometry::Element *Mesh::elementTable(size_t nodeIndex,
                                              size_t listIndex) {
  return this->m_impl->elementTable(nodeIndex, listIndex);
}
std::vector<Adcirc::Geometry::Element *> Mesh::elementsAroundNode(
    Adcirc::Geometry::Node *n) {
  return std::move(this->m_impl->elementsAroundNode(n));
}
