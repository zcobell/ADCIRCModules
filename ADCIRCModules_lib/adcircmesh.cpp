#include "adcircmesh.h"
#include <QString>
#include <QStringList>

#define CHECK_FILEREAD_RETURN(ok)                                              \
  if (!ok)                                                                     \
    return Adcirc::FileReadError;

AdcircMesh::AdcircMesh(QObject *parent) : QObject(parent) {
  this->setFilename(QString());
}

AdcircMesh::AdcircMesh(QString filename, QObject *parent) : QObject(parent) {
  this->setFilename(filename);
}

AdcircMesh::~AdcircMesh() {
  for (int i = 0; i < this->numNodes(); i++)
    delete this->_nodes[i];
  this->_nodes.clear();

  for (int i = 0; i < this->numElements(); i++)
    delete this->_elements[i];
  this->_elements.clear();

  for (int i = 0; i < this->numOpenBoundaries(); i++)
    delete this->_openBoundaries[i];
  this->_openBoundaries.clear();

  for (int i = 0; i < this->numLandBoundaries(); i++)
    delete this->_landBoundaries[i];
  this->_landBoundaries.clear();
}

QString AdcircMesh::filename() const { return this->_filename; }

void AdcircMesh::setFilename(const QString &filename) {
  this->_filename = filename;
}

QString AdcircMesh::meshHeaderString() const { return this->_meshHeaderString; }

void AdcircMesh::setMeshHeaderString(const QString &meshHeaderString) {
  this->_meshHeaderString = meshHeaderString;
}

int AdcircMesh::numNodes() const { return _numNodes; }

void AdcircMesh::setNumNodes(int numNodes) { _numNodes = numNodes; }

int AdcircMesh::numElements() const { return _numElements; }

void AdcircMesh::setNumElements(int numElements) { _numElements = numElements; }

int AdcircMesh::numOpenBoundaries() const { return _numOpenBoundaries; }

void AdcircMesh::setNumOpenBoundaries(int numOpenBoundaries) {
  _numOpenBoundaries = numOpenBoundaries;
}

int AdcircMesh::numLandBoundaries() const { return _numLandBoundaries; }

void AdcircMesh::setNumLandBoundaries(int numLandBoundaries) {
  _numLandBoundaries = numLandBoundaries;
}

int AdcircMesh::read(QString filename) {
  this->setFilename(filename);
  int ierr = this->read();
  return ierr;
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

  this->_nodes.resize(this->numNodes());

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

    this->_nodes[i] = new AdcircNode(id, x, y, z);
    this->_nodeLookup[id] = i;
  }

  return Adcirc::NoError;
}

int AdcircMesh::_readElements(QFile &fid) {

  bool ok;
  int id, e1, e2, e3;
  QString tempLine;
  QStringList tempList;

  this->_elements.resize(this->numElements());

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

    this->_elements[i] =
        new AdcircElement(id, this->_nodes[this->_nodeLookup[e1]],
                          this->_nodes[this->_nodeLookup[e2]],
                          this->_nodes[this->_nodeLookup[e3]]);
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

  this->_openBoundaries.resize(this->numOpenBoundaries());

  tempLine = fid.readLine().simplified().split(' ').value(0);
  tempList = tempLine.split(" ", QString::SkipEmptyParts);
  this->setTotalOpenBoundaryNodes(tempList.value(0).toInt(&ok));
  CHECK_FILEREAD_RETURN(ok);

  for (int i = 0; i < this->numOpenBoundaries(); i++) {
    tempLine = fid.readLine();
    tempList = tempLine.split(" ", QString::SkipEmptyParts);
    length = tempList.value(0).toInt(&ok);
    CHECK_FILEREAD_RETURN(ok);

    this->_openBoundaries[i] = new AdcircBoundary(-1, length);

    for (int j = 0; j < this->_openBoundaries[i]->length(); j++) {
      tempLine = fid.readLine();
      tempList = tempLine.split(" ", QString::SkipEmptyParts);
      nid = tempList.value(0).toInt(&ok);
      CHECK_FILEREAD_RETURN(ok);
      this->_openBoundaries[i]->setNode1(j,
                                         this->_nodes[this->_nodeLookup[nid]]);
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

  this->_landBoundaries.resize(this->numLandBoundaries());

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

    this->_landBoundaries[i] = new AdcircBoundary(code, length);

    for (int j = 0; j < this->_landBoundaries[i]->length(); j++) {
      tempLine = fid.readLine();
      tempList = tempLine.split(" ", QString::SkipEmptyParts);
      nid = tempList.value(0).toInt(&ok);
      CHECK_FILEREAD_RETURN(ok);
      this->_landBoundaries[i]->setNode1(j,
                                         this->_nodes[this->_nodeLookup[nid]]);
      if (code == 3 || code == 13 || code == 23) {
        this->_landBoundaries[i]->setCrestElevation(
            j, tempList.value(1).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      } else if (code == 4 || code == 24) {
        nid = tempList.value(1).toInt(&ok);
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setNode2(
            j, this->_nodes[this->_nodeLookup[nid]]);
        this->_landBoundaries[i]->setCrestElevation(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, tempList.value(3).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(4).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      } else if (code == 5 || code == 25) {
        nid = tempList.value(1).toInt(&ok);
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setNode2(
            j, this->_nodes[this->_nodeLookup[nid]]);
        this->_landBoundaries[i]->setCrestElevation(
            j, tempList.value(2).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setSubcriticalWeirCoeffient(
            j, tempList.value(3).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setSupercriticalWeirCoefficient(
            j, tempList.value(4).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setPipeHeight(
            j, tempList.value(5).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setPipeCoefficient(
            j, tempList.value(6).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
        this->_landBoundaries[i]->setPipeDiameter(
            j, tempList.value(7).toDouble(&ok));
        CHECK_FILEREAD_RETURN(ok);
      }
    }
  }
  return Adcirc::NoError;
}

int AdcircMesh::totalLandBoundaryNodes() const {
  return _totalLandBoundaryNodes;
}

void AdcircMesh::setTotalLandBoundaryNodes(int totalLandBoundaryNodes) {
  _totalLandBoundaryNodes = totalLandBoundaryNodes;
}

int AdcircMesh::totalOpenBoundaryNodes() const {
  return _totalOpenBoundaryNodes;
}

void AdcircMesh::setTotalOpenBoundaryNodes(int totalOpenBoundaryNodes) {
  _totalOpenBoundaryNodes = totalOpenBoundaryNodes;
}
