#include "adcircmesh.h"
#include "boost/format.hpp"
#include "qkdtree2.h"
#include "qproj4.h"
#include "shapefil.h"
#include "split.h"
#include <fstream>
#include <string>

#define CHECK_FILEREAD_RETURN(ok)                                              \
  if (!ok)                                                                     \
    return Adcirc::FileReadError;

AdcircMesh::AdcircMesh() {
  this->setFilename(std::string());
  this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
}

AdcircMesh::AdcircMesh(std::string filename) {
  this->setFilename(filename);
  this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
}

AdcircMesh::AdcircMesh(const char *filename) {
  this->setFilename(filename);
  this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
}

AdcircMesh::~AdcircMesh() {
  for (int i = 0; i < this->numNodes(); i++)
    if (this->m_nodes[i] != nullptr)
      delete this->m_nodes[i];
  this->m_nodes.clear();

  for (int i = 0; i < this->numElements(); i++)
    if (this->m_elements[i] != nullptr)
      delete this->m_elements[i];
  this->m_elements.clear();

  for (int i = 0; i < this->numOpenBoundaries(); i++)
    if (this->m_openBoundaries[i] != nullptr)
      delete this->m_openBoundaries[i];
  this->m_openBoundaries.clear();

  for (int i = 0; i < this->numLandBoundaries(); i++)
    if (this->m_landBoundaries[i] != nullptr)
      delete this->m_landBoundaries[i];
  this->m_landBoundaries.clear();

  if (this->m_nodalSearchTree != nullptr)
    delete this->m_nodalSearchTree;

  if (this->m_elementalSearchTree != nullptr)
    delete this->m_elementalSearchTree;
}

std::string AdcircMesh::filename() const { return this->m_filename; }

void AdcircMesh::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

void AdcircMesh::setFilename(const char *filename) {
  this->m_filename = std::string(filename);
}

std::string AdcircMesh::meshHeaderString() const {
  return this->m_meshHeaderString;
}

void AdcircMesh::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_meshHeaderString = meshHeaderString;
}

int AdcircMesh::numNodes() const { return this->m_numNodes; }

void AdcircMesh::setNumNodes(int numNodes) { this->m_numNodes = numNodes; }

int AdcircMesh::numElements() const { return this->m_numElements; }

void AdcircMesh::setNumElements(int numElements) {
  this->m_numElements = numElements;
}

int AdcircMesh::numOpenBoundaries() const { return this->m_numOpenBoundaries; }

void AdcircMesh::setNumOpenBoundaries(int numOpenBoundaries) {
  this->m_numOpenBoundaries = numOpenBoundaries;
}

int AdcircMesh::numLandBoundaries() const { return this->m_numLandBoundaries; }

void AdcircMesh::setNumLandBoundaries(int numLandBoundaries) {
  this->m_numLandBoundaries = numLandBoundaries;
}

int AdcircMesh::read() {

  int ierr;
  int tempInt;
  std::string tempLine;
  std::vector<std::string> tempList;

  std::fstream fid(this->filename());

  //...Reading the header
  std::getline(fid, tempLine);
  this->setMeshHeaderString(tempLine);

  //...Reading the mesh size
  std::getline(fid, tempLine);
  tempList = split::stringSplitToVector(tempLine, ' ');
  tempLine = tempList[0];
  tempInt = stoi(tempLine);
  this->setNumElements(tempInt);

  tempLine = tempList[1];
  tempInt = stoi(tempLine);
  this->setNumNodes(tempInt);

  //...Read the node table
  ierr = this->_readNodes(fid);
  if (ierr != Adcirc::NoError) {
    fid.close();
    return ierr;
  }

  //...Read the element table
  ierr = this->_readElements(fid);
  if (ierr != Adcirc::NoError) {
    fid.close();
    return ierr;
  }

  //...Read the open boundaries
  ierr = this->_readOpenBoundaries(fid);
  if (ierr != Adcirc::NoError) {
    fid.close();
    return ierr;
  }

  //...Read the land boundaries
  ierr = this->_readLandBoundaries(fid);
  if (ierr != Adcirc::NoError) {
    fid.close();
    return ierr;
  }

  fid.close();

  return Adcirc::NoError;
}

int AdcircMesh::_readNodes(std::fstream &fid) {

  int id;
  double x, y, z;
  std::string tempLine;
  std::vector<std::string> tempList;

  this->m_nodes.resize(this->numNodes());

  for (int i = 0; i < this->numNodes(); i++) {
    std::getline(fid, tempLine);
    tempList = split::stringSplitToVector(tempLine, ' ');

    id = stoi(tempList[0]);
    x = stod(tempList[1]);
    y = stod(tempList[2]);
    z = stod(tempList[3]);

    this->m_nodes[i] = new AdcircNode(id, x, y, z);
    this->m_nodeLookup[id] = i;
  }

  return Adcirc::NoError;
}

int AdcircMesh::_readElements(std::fstream &fid) {

  bool ok;
  int id, e1, e2, e3;
  std::string tempLine;
  std::vector<std::string> tempList;

  this->m_elements.resize(this->numElements());

  for (int i = 0; i < this->numElements(); i++) {
    std::getline(fid, tempLine);
    tempList = split::stringSplitToVector(tempLine, ' ');
    id = stoi(tempList[0]);
    e1 = stod(tempList[2]);
    e2 = stod(tempList[3]);
    e3 = stod(tempList[4]);

    this->m_elements[i] =
        new AdcircElement(id, this->m_nodes[this->m_nodeLookup[e1]],
                          this->m_nodes[this->m_nodeLookup[e2]],
                          this->m_nodes[this->m_nodeLookup[e3]]);
  }

  return Adcirc::NoError;
}

int AdcircMesh::_readOpenBoundaries(std::fstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;
  int length, nid;
  bool ok;

  std::getline(fid, tempLine);
  tempList = split::stringSplitToVector(tempLine, ' ');
  this->setNumOpenBoundaries(stoi(tempList[0]));

  this->m_openBoundaries.resize(this->numOpenBoundaries());

  std::getline(fid, tempLine);
  tempList = split::stringSplitToVector(tempLine, ' ');
  tempLine = tempList[0];
  this->setTotalOpenBoundaryNodes(stoi(tempLine));

  for (int i = 0; i < this->numOpenBoundaries(); i++) {
    std::getline(fid, tempLine);
    tempList = split::stringSplitToVector(tempLine, ' ');
    length = stoi(tempList[0]);

    this->m_openBoundaries[i] = new AdcircBoundary(-1, length);

    for (int j = 0; j < this->m_openBoundaries[i]->length(); j++) {
      std::getline(fid, tempLine);
      tempList = split::stringSplitToVector(tempLine, ' ');
      nid = stoi(tempList[0]);
      this->m_openBoundaries[i]->setNode1(
          j, this->m_nodes[this->m_nodeLookup[nid]]);
    }
  }
  return Adcirc::NoError;
}

int AdcircMesh::_readLandBoundaries(std::fstream &fid) {

  std::string tempLine;
  std::vector<std::string> tempList;
  int length, nid, code;
  bool ok;

  std::getline(fid, tempLine);
  tempList = split::stringSplitToVector(tempLine, ' ');
  this->setNumLandBoundaries(stoi(tempList[0]));

  this->m_landBoundaries.resize(this->numLandBoundaries());

  std::getline(fid, tempLine);
  tempList = split::stringSplitToVector(tempLine, ' ');
  this->setTotalLandBoundaryNodes(stoi(tempList[0]));

  for (int i = 0; i < this->numLandBoundaries(); i++) {
    std::getline(fid, tempLine);
    tempList = split::stringSplitToVector(tempLine, ' ');
    length = stoi(tempList[0]);
    code = stoi(tempList[1]);

    this->m_landBoundaries[i] = new AdcircBoundary(code, length);

    for (int j = 0; j < this->m_landBoundaries[i]->length(); j++) {
      std::getline(fid, tempLine);
      tempList = split::stringSplitToVector(tempLine, ' ');
      nid = stoi(tempList[0]);

      this->m_landBoundaries[i]->setNode1(
          j, this->m_nodes[this->m_nodeLookup[nid]]);
      if (code == 3 || code == 13 || code == 23) {

        this->m_landBoundaries[i]->setCrestElevation(j, stod(tempList[1]));

        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, stod(tempList[2]));

      } else if (code == 4 || code == 24) {
        nid = stoi(tempList[1]);

        this->m_landBoundaries[i]->setNode2(
            j, this->m_nodes[this->m_nodeLookup[nid]]);
        this->m_landBoundaries[i]->setCrestElevation(j, stod(tempList[2]));

        this->m_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, stod(tempList[3]));

        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, stod(tempList[4]));

      } else if (code == 5 || code == 25) {
        nid = stoi(tempList[1]);

        this->m_landBoundaries[i]->setNode2(
            j, this->m_nodes[this->m_nodeLookup[nid]]);

        this->m_landBoundaries[i]->setCrestElevation(j, stod(tempList[2]));

        this->m_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, stod(tempList[3]));

        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, stod(tempList[4]));

        this->m_landBoundaries[i]->setPipeHeight(j, stod(tempList[5]));

        this->m_landBoundaries[i]->setPipeCoefficient(j, stod(tempList[6]));

        this->m_landBoundaries[i]->setPipeDiameter(j, stod(tempList[7]));
      }
    }
  }
  return Adcirc::NoError;
}

QKdtree2 *AdcircMesh::elementalSearchTree() const
{
  return m_elementalSearchTree;
}

QKdtree2 *AdcircMesh::nodalSearchTree() const
{
  return m_nodalSearchTree;
}

int AdcircMesh::totalLandBoundaryNodes() const {
  return m_totalLandBoundaryNodes;
}

void AdcircMesh::setTotalLandBoundaryNodes(int totalLandBoundaryNodes) {
  m_totalLandBoundaryNodes = totalLandBoundaryNodes;
}

int AdcircMesh::totalOpenBoundaryNodes() const {
  return m_totalOpenBoundaryNodes;
}

void AdcircMesh::setTotalOpenBoundaryNodes(int totalOpenBoundaryNodes) {
  m_totalOpenBoundaryNodes = totalOpenBoundaryNodes;
}

AdcircNode *AdcircMesh::node(int index) {
  if (index >= 0 && index < this->numNodes())
    return this->m_nodes[index];
  else
    return nullptr;
}

AdcircElement *AdcircMesh::element(int index) {
  if (index >= 0 && index < this->numElements())
    return this->m_elements[index];
  else
    return nullptr;
}

AdcircBoundary *AdcircMesh::openBoundary(int index) {
  if (index >= 0 && index < this->numOpenBoundaries())
    return this->m_openBoundaries[index];
  else
    return nullptr;
}

AdcircBoundary *AdcircMesh::landBoundary(int index) {
  if (index >= 0 && index < this->numLandBoundaries())
    return this->m_landBoundaries[index];
  else
    return nullptr;
}

void AdcircMesh::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
  return;
}

int AdcircMesh::projection() { return this->m_epsg; }

bool AdcircMesh::isLatLon() { return this->m_isLatLon; }

int AdcircMesh::reproject(int epsg) {

  QProj4 proj;
  std::vector<Point> inPoint, outPoint;
  inPoint.resize(this->numNodes());
  outPoint.resize(this->numNodes());

  for (int i = 0; i < this->numNodes(); i++) {
    inPoint[i] = Point(this->node(i)->x(), this->node(i)->y());
  }

  int ierr = proj.transform(this->projection(), epsg, inPoint, outPoint,
                            this->m_isLatLon);

  if (ierr != QProj4::NoError)
    return Adcirc::Proj4Error;

  for (int i = 0; i < this->numNodes(); i++) {
    this->node(i)->setX(outPoint[i].x());
    this->node(i)->setY(outPoint[i].y());
  }

  return Adcirc::NoError;
}

int AdcircMesh::toShapefile(const char *outputFile) {
  std::string filename = std::string(outputFile);
  int ierr = this->toShapefile(filename);
  return ierr;
}

int AdcircMesh::toShapefile(std::string outputFile) {

  SHPHandle shpid;
  DBFHandle dbfid;
  SHPObject *shpobj;
  int i, shp_index, nodeid;
  double latitude, longitude, elevation;

  shpid = SHPCreate(outputFile.c_str(), SHPT_POINT);
  dbfid = DBFCreate(outputFile.c_str());

  DBFAddField(dbfid, "nodeid", FTInteger, 16, 0);
  DBFAddField(dbfid, "longitude", FTDouble, 16, 8);
  DBFAddField(dbfid, "latitude", FTDouble, 16, 8);
  DBFAddField(dbfid, "elevation", FTDouble, 16, 4);

  for (i = 0; i < this->numNodes(); i++) {

    nodeid = this->node(i)->id();
    longitude = this->node(i)->x();
    latitude = this->node(i)->y();
    elevation = this->node(i)->z();

    shpobj =
        SHPCreateSimpleObject(SHPT_POINT, 1, &longitude, &latitude, &elevation);
    shp_index = SHPWriteObject(shpid, -1, shpobj);
    SHPDestroyObject(shpobj);

    DBFWriteIntegerAttribute(dbfid, shp_index, 0, nodeid);
    DBFWriteDoubleAttribute(dbfid, shp_index, 1, longitude);
    DBFWriteDoubleAttribute(dbfid, shp_index, 2, latitude);
    DBFWriteDoubleAttribute(dbfid, shp_index, 3, elevation);
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return Adcirc::NoError;
}

int AdcircMesh::buildNodalSearchTree() {
  int i, ierr;
  std::vector<double> x, y;

  x.resize(this->numNodes());
  y.resize(this->numNodes());

  for (i = 0; i < this->numNodes(); i++) {
    x[i] = this->node(i)->x();
    y[i] = this->node(i)->y();
  }

  if (this->m_nodalSearchTree != nullptr)
    if (this->m_nodalSearchTree->isInitialized())
      delete this->m_nodalSearchTree;

  this->m_nodalSearchTree = new QKdtree2();
  ierr = this->m_nodalSearchTree->build(x, y);
  if (ierr != QKdtree2::NoError) {
    return Adcirc::KdtreeError;
  }

  return Adcirc::NoError;
}

int AdcircMesh::buildElementalSearchTree() {
  std::vector<double> x, y;
  double tempX, tempY;

  x.resize(this->numElements());
  y.resize(this->numElements());

  for (int i = 0; i < this->numElements(); i++) {
    tempX = 0.0;
    tempY = 0.0;
    for (int j = 0; j < this->element(i)->n(); j++) {
      tempX = tempX + this->element(i)->node(j)->x();
      tempY = tempY + this->element(i)->node(j)->y();
    }
    x[i] = tempX / this->element(i)->n();
    y[i] = tempY / this->element(i)->n();
  }

  if (this->m_elementalSearchTree != nullptr)
    if (this->m_elementalSearchTree->isInitialized())
      delete this->m_elementalSearchTree;

  this->m_elementalSearchTree = new QKdtree2();
  int ierr = this->m_elementalSearchTree->build(x, y);
  if (ierr != QKdtree2::NoError) {
    return Adcirc::KdtreeError;
  }

  return Adcirc::NoError;
}

bool AdcircMesh::nodalSearchTreeInitialized() {
  return this->m_nodalSearchTree->isInitialized();
}

bool AdcircMesh::elementalSearchTreeInitialized() {
  return this->m_elementalSearchTree->isInitialized();
}

void AdcircMesh::resizeMesh(int numNodes, int numElements,
                            int numOpenBoundaries, int numLandBoundaries) {
  if (numNodes != this->numNodes()) {
    if (numNodes < this->numNodes()) {
      for (int i = numNodes; i < this->numNodes(); i++) {
        if (this->m_nodes[i] != nullptr)
          delete this->m_nodes[i];
      }
    }
    this->m_nodes.resize(numNodes);
    this->setNumNodes(numNodes);
  }

  if (numElements != this->numElements()) {
    if (numElements < this->numElements()) {
      for (int i = numElements; i < this->numElements(); i++) {
        if (this->m_elements[i] != nullptr)
          delete this->m_elements[i];
      }
    }
    this->m_elements.resize(numElements);
    this->setNumElements(numElements);
  }

  if (numOpenBoundaries != this->numOpenBoundaries()) {
    if (numOpenBoundaries < this->numOpenBoundaries()) {
      for (int i = numOpenBoundaries; i < this->numOpenBoundaries(); i++) {
        if (this->m_openBoundaries[i] != nullptr)
          delete this->m_openBoundaries[i];
      }
    }
    this->m_openBoundaries.resize(numOpenBoundaries);
    this->setNumOpenBoundaries(numOpenBoundaries);
  }

  if (numLandBoundaries != this->numLandBoundaries()) {
    if (numLandBoundaries < this->numLandBoundaries()) {
      for (int i = numLandBoundaries; i < this->numLandBoundaries(); i++) {
        if (this->m_landBoundaries[i] != nullptr)
          delete this->m_landBoundaries[i];
      }
    }
    this->m_landBoundaries.resize(numLandBoundaries);
    this->setNumLandBoundaries(numLandBoundaries);
  }

  return;
}

void AdcircMesh::addNode(int index, AdcircNode *node) {
  if (index < this->numNodes()) {
    if (this->m_nodes[index] != nullptr)
      delete this->m_nodes[index];
    this->m_nodes[index] = node;
  }
  return;
}

void AdcircMesh::deleteNode(int index) {
  if (index < this->numNodes()) {
    if (this->m_nodes[index] != nullptr)
      delete this->m_nodes[index];
    this->m_nodes.erase(this->m_nodes.begin() + index);
    this->setNumNodes(this->m_nodes.size());
  }
  return;
}

void AdcircMesh::addElement(int index, AdcircElement *element) {
  if (index < this->numElements()) {
    if (this->m_elements[index] != nullptr)
      delete this->m_elements[index];
    this->m_elements[index] = element;
  }
  return;
}

void AdcircMesh::deleteElement(int index) {
  if (index < this->numElements()) {
    if (this->m_elements[index] != nullptr)
      delete this->m_elements[index];
    this->m_elements.erase(this->m_elements.begin() + index);
    this->setNumNodes(this->m_nodes.size());
  }
  return;
}

int AdcircMesh::write(const char *filename) {
  return this->write(std::string(filename));
}

int AdcircMesh::write(std::string filename) {
  int i, j;
  std::string tempString;
  std::vector<std::string> boundaryList;
  std::ofstream outputFile;

  outputFile.open(filename);

  //...Write the header
  outputFile << this->meshHeaderString() << "\n";
  tempString = boost::str(boost::format("%11i %11i") % this->numElements() %
                          this->numNodes());
  outputFile << tempString << "\n";

  //...Write the mesh nodes
  for (i = 0; i < this->numNodes(); i++)
    outputFile << this->node(i)->toString(this->isLatLon()) << "\n";

  //...Write the mesh elements
  for (i = 0; i < this->numElements(); i++)
    outputFile << this->element(i)->toString() << "\n";

  //...Write the open boundary header
  outputFile << this->numOpenBoundaries() << "\n";
  outputFile << this->totalOpenBoundaryNodes() << "\n";

  //...Write the open boundaries
  for (i = 0; i < this->numOpenBoundaries(); i++) {
    boundaryList = this->openBoundary(i)->toStringList();
    for (j = 0; j < boundaryList.size(); j++)
      outputFile << boundaryList[j] << "\n";
  }

  //...Write the land boundary header
  outputFile << this->numLandBoundaries() << "\n";
  outputFile << this->totalLandBoundaryNodes() << "\n";

  //...Write the land boundaries
  for (i = 0; i < this->numLandBoundaries(); i++) {
    boundaryList = this->landBoundary(i)->toStringList();
    for (j = 0; j < boundaryList.size(); j++)
      outputFile << boundaryList[j] << "\n";
  }

  //...Close the file
  outputFile.close();

  return Adcirc::NoError;
}
