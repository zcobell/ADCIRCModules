#include "adcircmesh.h"
#include "qkdtree2.h"
#include "qproj4.h"
#include "shapefil.h"
#include <QString>
#include <QStringList>

#define CHECK_FILEREAD_RETURN(ok)                                              \
  if (!ok)                                                                     \
    return Adcirc::FileReadError;

AdcircMesh::AdcircMesh(QObject *parent) : QObject(parent) {
  this->setFilename(QString());
  this->defineProjection(4326, true);
}

AdcircMesh::AdcircMesh(QString filename, QObject *parent) : QObject(parent) {
  this->setFilename(filename);
  this->defineProjection(4326, true);
}

AdcircMesh::AdcircMesh(std::string filename, QObject *parent) {
  this->setFilename(QString::fromStdString(filename));
  this->defineProjection(4326, true);
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
}

QString AdcircMesh::filename() const { return this->m_filename; }

void AdcircMesh::setFilename(const QString &filename) {
  this->m_filename = filename;
}

QString AdcircMesh::meshHeaderString() const {
  return this->m_meshHeaderString;
}

void AdcircMesh::setMeshHeaderString(const QString &meshHeaderString) {
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

  bool ok;
  int ierr;
  int tempInt;
  QString tempLine;
  QStringList tempList;

  QFile fid(this->filename());
  if (!fid.open(QIODevice::ReadOnly))
    return Adcirc::FileOpenError;

  //...Reading the header
  this->setMeshHeaderString(fid.readLine().simplified());

  //...Reading the mesh size
  tempLine = fid.readLine();
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  tempLine = tempList.value(0);
  tempInt = tempLine.toInt(&ok);
  if (ok)
    this->setNumElements(tempInt);
  else
    return Adcirc::FileReadError;

  tempLine = tempList.value(1);
  tempInt = tempLine.toInt(&ok);
  if (ok)
    this->setNumNodes(tempInt);
  else
    return Adcirc::FileReadError;

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

int AdcircMesh::_readNodes(QFile &fid) {

  bool ok;
  int id;
  double x, y, z;
  QString tempLine;
  QStringList tempList;

  this->m_nodes.resize(this->numNodes());

  for (int i = 0; i < this->numNodes(); i++) {
    tempLine = fid.readLine();
    tempList = tempLine.split(" ", QString::SkipEmptyParts);

    id = tempList.value(0).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    x = tempList.value(1).toDouble(&ok);
    CHECK_FILEREAD_RETURN(ok);

    y = tempList.value(2).toDouble(&ok);
    CHECK_FILEREAD_RETURN(ok);

    z = tempList.value(3).toDouble(&ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_nodes[i] = new AdcircNode(id, x, y, z);
    this->m_nodeLookup[id] = i;
  }

  return Adcirc::NoError;
}

int AdcircMesh::_readElements(QFile &fid) {

  bool ok;
  int id, e1, e2, e3;
  QString tempLine;
  QStringList tempList;

  this->m_elements.resize(this->numElements());

  for (int i = 0; i < this->numElements(); i++) {
    tempLine = fid.readLine();
    tempList = tempLine.split(" ", QString::SkipEmptyParts);
    id = tempList.value(0).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    e1 = tempList.value(1).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    e2 = tempList.value(2).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    e3 = tempList.value(3).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_elements[i] =
        new AdcircElement(id, this->m_nodes[this->m_nodeLookup[e1]],
                          this->m_nodes[this->m_nodeLookup[e2]],
                          this->m_nodes[this->m_nodeLookup[e3]]);
  }

  return Adcirc::NoError;
}

int AdcircMesh::_readOpenBoundaries(QFile &fid) {
  QString tempLine;
  QStringList tempList;
  int length, nid;
  bool ok;

  tempLine = fid.readLine();
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  this->setNumOpenBoundaries(tempList.value(0).toInt(&ok));
  CHECK_FILEREAD_RETURN(ok);

  this->m_openBoundaries.resize(this->numOpenBoundaries());

  tempLine = fid.readLine().simplified().split(' ').value(0);
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  this->setTotalOpenBoundaryNodes(tempList.value(0).toInt(&ok));
  CHECK_FILEREAD_RETURN(ok);

  for (int i = 0; i < this->numOpenBoundaries(); i++) {
    tempLine = fid.readLine();
    tempList = tempLine.split(" ", QString::SkipEmptyParts);
    length = tempList.value(0).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_openBoundaries[i] = new AdcircBoundary(-1, length);

    for (int j = 0; j < this->m_openBoundaries[i]->length(); j++) {
      tempLine = fid.readLine();
      tempList = tempLine.split(" ", QString::SkipEmptyParts);
      nid = tempList.value(0).toInt(&ok);
      CHECK_FILEREAD_RETURN(ok);
      this->m_openBoundaries[i]->setNode1(
          j, this->m_nodes[this->m_nodeLookup[nid]]);
    }
  }
  return Adcirc::NoError;
}

int AdcircMesh::_readLandBoundaries(QFile &fid) {

  QString tempLine;
  QStringList tempList;
  int length, nid, code;
  bool ok;

  tempLine = fid.readLine();
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  this->setNumLandBoundaries(tempList.value(0).toInt(&ok));
  CHECK_FILEREAD_RETURN(ok);

  this->m_landBoundaries.resize(this->numLandBoundaries());

  tempLine = fid.readLine();
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  this->setTotalLandBoundaryNodes(tempList.value(0).toInt(&ok));
  CHECK_FILEREAD_RETURN(ok);

  for (int i = 0; i < this->numLandBoundaries(); i++) {
    tempLine = fid.readLine();
    tempList = tempLine.split(" ", QString::SkipEmptyParts);
    length = tempList.value(0).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);
    code = tempList.value(1).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_landBoundaries[i] = new AdcircBoundary(code, length);

    for (int j = 0; j < this->m_landBoundaries[i]->length(); j++) {
      tempLine = fid.readLine();
      tempList = tempLine.split(" ", QString::SkipEmptyParts);
      nid = tempList.value(0).toInt(&ok);
      CHECK_FILEREAD_RETURN(ok);
      this->m_landBoundaries[i]->setNode1(
          j, this->m_nodes[this->m_nodeLookup[nid]]);
      if (code == 3 || code == 13 || code == 23) {
        this->m_landBoundaries[i]->setCrestElevation(
            j, tempList.value(1).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      } else if (code == 4 || code == 24) {
        nid = tempList.value(1).toInt(&ok);
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setNode2(
            j, this->m_nodes[this->m_nodeLookup[nid]]);
        this->m_landBoundaries[i]->setCrestElevation(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, tempList.value(3).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(4).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      } else if (code == 5 || code == 25) {
        nid = tempList.value(1).toInt(&ok);
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setNode2(
            j, this->m_nodes[this->m_nodeLookup[nid]]);
        this->m_landBoundaries[i]->setCrestElevation(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, tempList.value(3).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(4).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setPipeHeight(
            j, tempList.value(5).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setPipeCoefficient(
            j, tempList.value(6).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->m_landBoundaries[i]->setPipeDiameter(
            j, tempList.value(7).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      }
    }
  }
  return Adcirc::NoError;
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

void AdcircMesh::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
  return;
}

int AdcircMesh::projection() { return this->m_epsg; }

bool AdcircMesh::isLatLon() { return this->m_isLatLon; }

int AdcircMesh::reproject(int epsg) {

  QProj4 proj;
  QVector<QPointF> inPoint, outPoint;
  inPoint.resize(this->numNodes());
  outPoint.resize(this->numNodes());

  for (int i = 0; i < this->numNodes(); i++) {
    inPoint[i] = QPointF(this->node(i)->x(), this->node(i)->y());
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

int AdcircMesh::toShapefile(QString outputFile) {

  SHPHandle shpid;
  DBFHandle dbfid;
  SHPObject *shpobj;
  int i, shp_index, nodeid;
  qreal latitude, longitude, elevation;

  shpid = SHPCreate(outputFile.toStdString().c_str(), SHPT_POINT);
  dbfid = DBFCreate(outputFile.toStdString().c_str());

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
  QVector<qreal> x, y;

  x.resize(this->numNodes());
  y.resize(this->numNodes());

  for (i = 0; i < this->numNodes(); i++) {
    x[i] = this->node(i)->x();
    y[i] = this->node(i)->y();
  }

  if (this->m_nodalSearchTree->isInitialized())
    delete this->m_nodalSearchTree;

  this->m_nodalSearchTree = new QKdtree2(this);
  ierr = this->m_nodalSearchTree->build(x, y);
  if (ierr != QKdtree2::NoError) {
    return Adcirc::KdtreeError;
  }

  return Adcirc::NoError;
}

int AdcircMesh::buildElementalSearchTree() {
  QVector<qreal> x, y;
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

  if (this->m_elementalSearchTree->isInitialized())
    delete this->m_elementalSearchTree;

  this->m_elementalSearchTree = new QKdtree2(this);
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
    this->m_nodes.removeAt(index);
    this->setNumNodes(this->m_nodes.length());
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
    this->m_elements.removeAt(index);
    this->setNumNodes(this->m_nodes.length());
  }
  return;
}
