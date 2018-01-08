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
#include "adcircmesh.h"
#include "boost/format.hpp"
#include "io.h"
#include "qkdtree2.h"
#include "qproj4.h"
#include "shapefil.h"
#include "stringconversion.h"
#include <string>

#define CHECK_FILEREAD_RETURN(ok)                                              \
  if (!ok)                                                                     \
    return FileIO::GenericFileReadError;

#define CHECK_FILEREAD_RETURN_AND_CLOSE(ok)                                    \
  if (!ok) {                                                                   \
    fid.close();                                                               \
    return FileIO::GenericFileReadError;                                       \
  }

#define CHECK_FILEREAD_RETURN_INT(ierr)                                        \
  if (ierr != FileIO::NoError)                                                 \
    return ierr;

#define CHECK_RETURN_AND_CLOSE(ierr)                                           \
  if (ierr != Adcirc::NoError) {                                               \
    fid.close();                                                               \
    return ierr;                                                               \
  }

/**
 * @name AdcircMesh::AdcircMesh
 * @brief Default Constructor
 */
AdcircMesh::AdcircMesh() { this->_init(); }

/**
 * @overload AdcircMesh::AdcircMesh
 * @brief Default constructor with filename parameter
 * @param filename name of the mesh to read
 */
AdcircMesh::AdcircMesh(std::string filename) {
  this->_init();
  this->setFilename(filename);
}

/**
 * @overload AdcircMesh::AdcircMesh
 * @brief Default constructor with filename as a char
 * (for python)
 * @param filename name of the mesh to read
 */
AdcircMesh::AdcircMesh(const char *filename) {
  this->_init();
  this->setFilename(filename);
}

/**
 * @name Adcircmesh::_init
 * @brief Initialization routine called by all constructors
 */
void AdcircMesh::_init() {
  this->setFilename(std::string());
  this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
  this->m_numNodes = 0;
  this->m_numElements = 0;
  this->m_numLandBoundaries = 0;
  this->m_numOpenBoundaries = 0;
  this->m_totalOpenBoundaryNodes = 0;
  this->m_totalLandBoundaryNodes = 0;
  this->m_nodeOrderingLogical = true;
  this->m_elementOrderingLogical = true;
}

/**
 * @brief AdcircMesh::~AdcircMesh Destructor
 */
AdcircMesh::~AdcircMesh() {

  this->m_nodes.clear();
  this->m_elements.clear();
  this->m_openBoundaries.clear();
  this->m_landBoundaries.clear();
  this->m_nodeLookup.clear();
  this->m_elementLookup.clear();

  if (this->m_nodalSearchTree != nullptr)
    delete this->m_nodalSearchTree;

  if (this->m_elementalSearchTree != nullptr)
    delete this->m_elementalSearchTree;
}

/**
 * @name AdcircMesh::filename
 * @brief Filename of the mesh to be read
 * @return Return the name of the mesh to be read
 */
std::string AdcircMesh::filename() const { return this->m_filename; }

/**
 * @name AdcircMesh::setFilename
 * @brief Sets the name of the mesh to be read
 * @param filename Name of the mesh
 */
void AdcircMesh::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

/**
 * @overload AdcircMesh::setFilename
 * @brief Sets the name of the mesh to be read
 * @param filename Name of the mesh
 */
void AdcircMesh::setFilename(const char *filename) {
  this->m_filename = std::string(filename);
}

/**
 * @name AdcircMesh::meshHeaderString
 * @brief Returns the mesh header from the processed mesh
 * @return mesh header
 */
std::string AdcircMesh::meshHeaderString() const {
  return this->m_meshHeaderString;
}

/**
 * @name AdcircMesh::setMeshHeaderString
 * @brief Sets the header for the mesh
 * @param mesh header
 */
void AdcircMesh::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_meshHeaderString = meshHeaderString;
}

/**
 * @overload AdcircMesh::setMeshHeaderString
 * @brief Sets the header for the mesh
 * @param mesh header
 */
void AdcircMesh::setMeshHeaderString(const char *meshHeaderString) {
  this->m_meshHeaderString = std::string(meshHeaderString);
}

/**
 * @name AdcircMesh::numNodes
 * @brief Returns the number of nodes currently in the mesh
 * @return number of nodes
 */
int AdcircMesh::numNodes() const { return this->m_numNodes; }

/**
 * @name Adcircmesh::setNumNodes
 * @brief Sets the number of nodes in the mesh. Note this does not resize the
 * mesh
 * @param numNodes number of nodes
 */
void AdcircMesh::setNumNodes(int numNodes) { this->m_numNodes = numNodes; }

/**
 * @name AdcircMesh::numElements
 * @brief Returns the number of elements in the mesh
 * @return number of elements
 */
int AdcircMesh::numElements() const { return this->m_numElements; }

/**
 * @name Adcircmesh::setNumElements
 * @brief Sets the number of elements in the mesh. Note: This does not resize
 * the mesh
 * @param numElements Number of elements
 */
void AdcircMesh::setNumElements(int numElements) {
  this->m_numElements = numElements;
}

/**
 * @name AdcircMesh::numOpenBonudaries
 * @brief Returns the number of open boundaries in the model
 * @return number of open boundaries
 */
int AdcircMesh::numOpenBoundaries() const { return this->m_numOpenBoundaries; }

/**
 * @name AdcircMesh::setNumOpenBoundaries
 * @brief Sets the number of open boundaries in the model. Note this does not
 * resize the boundary array
 * @param numOpenBoundaries Number of open boundaries
 */
void AdcircMesh::setNumOpenBoundaries(int numOpenBoundaries) {
  this->m_numOpenBoundaries = numOpenBoundaries;
}

/**
 * @name AdcircMesh::numLandBoundaries
 * @brief Returns the number of land boundaries in the model
 * @return number of land boundaries
 */
int AdcircMesh::numLandBoundaries() const { return this->m_numLandBoundaries; }

/**
 * @name AdcircMesh::setNumLandBoundaries
 * @brief Sets the number of land boundaries in the model. Note this does not
 * resize the boundary array
 * @param numLandBoundaries Number of land boundaries
 */
void AdcircMesh::setNumLandBoundaries(int numLandBoundaries) {
  this->m_numLandBoundaries = numLandBoundaries;
}

/**
 * @name Adcircmesh::read
 * @brief Reads an ASCII formatted ADCIRC mesh. Note this will be extended in
 * the future to netCDF formatted meshes
 * @return error code. If error code does not equal Adcirc::NoError, an error
 * occured
 */
int AdcircMesh::read() {

  int ierr;

  std::fstream fid(this->filename());

  this->_readMeshHeader(fid);

  //...Read the node table
  ierr = this->_readNodes(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  //...Read the element table
  ierr = this->_readElements(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  //...Read the open boundaries
  ierr = this->_readOpenBoundaries(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  //...Read the land boundaries
  ierr = this->_readLandBoundaries(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  fid.close();

  return Adcirc::NoError;
}

/**
 * @name Adcircmesh::_readMeshHeader
 * @brief Reads the mesh title line and node/element dimensional data
 * @param fid std::fstream reference for the currently opened mesh
 * @return error code
 */
int AdcircMesh::_readMeshHeader(std::fstream &fid) {

  bool ok;
  int tempInt;
  std::string tempLine;
  std::vector<std::string> tempList;

  //...Read mesh header
  std::getline(fid, tempLine);

  //...Set the mesh file header
  this->setMeshHeaderString(tempLine);

  //..Get the size of the mesh
  std::getline(fid, tempLine);
  IO::splitString(tempLine, tempList);
  tempLine = tempList[0];
  tempInt = StringConversion::stringToInt(tempLine, ok);
  CHECK_FILEREAD_RETURN_AND_CLOSE(ok);
  this->setNumElements(tempInt);

  tempLine = tempList[1];
  tempInt = StringConversion::stringToInt(tempLine, ok);
  CHECK_FILEREAD_RETURN_AND_CLOSE(ok);
  this->setNumNodes(tempInt);

  return Adcirc::NoError;
}

/**
 * @name AdcircMesh::_readNodes
 * @brief Reads the node section of the ASCII formatted mesh
 * @param fid std::fstream reference for the currently opened mesh
 * @return error code
 */
int AdcircMesh::_readNodes(std::fstream &fid) {
  int id;
  double x, y, z;
  std::string tempLine;

  this->m_nodes.resize(this->numNodes());

  for (int i = 0; i < this->numNodes(); i++) {
    std::getline(fid, tempLine);
    int ierr = IO::splitStringNodeFormat(tempLine, id, x, y, z);
    CHECK_FILEREAD_RETURN_INT(ierr);

    if (i != id - 1)
      this->m_nodeOrderingLogical = false;

    this->m_nodes[i] = AdcircNode(id, x, y, z);
  }

  if (!this->m_nodeOrderingLogical) {
    this->m_nodeLookup.reserve(this->numNodes());

    for (int i = 0; i < this->m_numNodes; i++)
      this->m_nodeLookup[this->m_nodes[i].id()] = i;
  }

  return Adcirc::NoError;
}

/**
 * @name AdcircMesh::_readElements
 * @brief Reads the elements section of the ASCII mesh
 * @param fid std::fstream reference for the currently opened mesh
 * @return error code
 */
int AdcircMesh::_readElements(std::fstream &fid) {

  int id, e1, e2, e3;
  std::string tempLine;

  this->m_elements.resize(this->numElements());

  if (this->m_nodeOrderingLogical) {
    for (int i = 0; i < this->numElements(); i++) {
      std::getline(fid, tempLine);
      int ierr = IO::splitStringElemFormat(tempLine, id, e1, e2, e3);
      CHECK_FILEREAD_RETURN_INT(ierr);

      this->m_elements[i].setElement(id, &this->m_nodes[e1 - 1],
                                     &this->m_nodes[e2 - 1],
                                     &this->m_nodes[e3 - 1]);
    }
  } else {
    for (int i = 0; i < this->numElements(); i++) {
      std::getline(fid, tempLine);
      int ierr = IO::splitStringElemFormat(tempLine, id, e1, e2, e3);
      CHECK_FILEREAD_RETURN_INT(ierr);

      if (i != id - 1)
        this->m_elementOrderingLogical = false;

      this->m_elements[i].setElement(id, &this->m_nodes[this->m_nodeLookup[e1]],
                                     &this->m_nodes[this->m_nodeLookup[e2]],
                                     &this->m_nodes[this->m_nodeLookup[e3]]);
    }
  }

  if (!this->m_elementOrderingLogical) {
    this->m_elementLookup.reserve(this->m_numElements);
    for (int i = 0; i < this->m_numElements; i++)
      this->m_elementLookup[this->m_elements[i].id()] = i;
  }

  return Adcirc::NoError;
}

/**
 * @name AdcircMesh::_readOpenBoundaries
 * @brief Reads the open boundaries section of the ASCII formatted mesh file
 * @param fid std::fstream reference for the currently opened mesh
 * @return error code
 */
int AdcircMesh::_readOpenBoundaries(std::fstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;
  int length, nid, ierr;
  bool ok;

  std::getline(fid, tempLine);
  IO::splitString(tempLine, tempList);

  this->setNumOpenBoundaries(StringConversion::stringToInt(tempList[0], ok));
  CHECK_FILEREAD_RETURN(ok);

  this->m_openBoundaries.resize(this->numOpenBoundaries());

  std::getline(fid, tempLine);
  // IO::splitString(tempLine, tempList);
  // tempLine = tempList[0];
  // this->setTotalOpenBoundaryNodes(StringConversion::stringToInt(tempLine,
  // ok));  CHECK_FILEREAD_RETURN(ok);

  for (int i = 0; i < this->numOpenBoundaries(); i++) {

    std::getline(fid, tempLine);
    IO::splitString(tempLine, tempList);

    length = StringConversion::stringToInt(tempList[0], ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_openBoundaries[i].setBoundary(-1, length);

    for (int j = 0; j < this->m_openBoundaries[i].length(); j++) {

      std::getline(fid, tempLine);
      ierr = IO::splitStringBoundary0Format(tempLine, nid);
      CHECK_FILEREAD_RETURN_INT(ierr);

      if (this->m_nodeOrderingLogical)
        this->m_openBoundaries[i].setNode1(j, &this->m_nodes[nid - 1]);
      else
        this->m_openBoundaries[i].setNode1(
            j, &this->m_nodes[this->m_nodeLookup[nid]]);
    }
  }
  return Adcirc::NoError;
}

/**
 * @name AdcircMesh::_readLandBoundaries
 * @brief Reads the land boundaries section of the ASCII formatted mesh file
 * @param fid std::fstream reference for the currently opened mesh
 * @return error code
 */
int AdcircMesh::_readLandBoundaries(std::fstream &fid) {

  std::string tempLine;
  std::vector<std::string> tempList;
  int length, n1, n2, code, ierr;
  double supercritical, subcritical, crest, pipeheight, pipediam, pipecoef;
  bool ok;

  std::getline(fid, tempLine);

  IO::splitString(tempLine, tempList);

  this->setNumLandBoundaries(StringConversion::stringToInt(tempList[0], ok));
  CHECK_FILEREAD_RETURN(ok);

  this->m_landBoundaries.resize(this->numLandBoundaries());

  std::getline(fid, tempLine);

  // IO::splitString(tempLine, tempList);
  // this->setTotalLandBoundaryNodes(
  //    StringConversion::stringToInt(tempList[0], ok));
  // CHECK_FILEREAD_RETURN(ok);

  for (int i = 0; i < this->numLandBoundaries(); i++) {

    std::getline(fid, tempLine);
    IO::splitString(tempLine, tempList);

    length = StringConversion::stringToInt(tempList[0], ok);
    CHECK_FILEREAD_RETURN(ok);
    code = StringConversion::stringToInt(tempList[1], ok);
    CHECK_FILEREAD_RETURN(ok);

    this->m_landBoundaries[i].setBoundary(code, length);

    for (int j = 0; j < this->m_landBoundaries[i].length(); j++) {

      std::getline(fid, tempLine);

      if (code == 3 || code == 13 || code == 23) {

        ierr =
            IO::splitStringBoundary23Format(tempLine, n1, crest, supercritical);
        CHECK_FILEREAD_RETURN_INT(ierr);
        if (this->m_nodeOrderingLogical) {
          this->m_landBoundaries[i].setNode1(j, &this->m_nodes[n1 - 1]);
        } else {
          this->m_landBoundaries[i].setNode1(
              j, &this->m_nodes[this->m_nodeLookup[n1]]);
        }

        this->m_landBoundaries[i].setCrestElevation(j, crest);
        this->m_landBoundaries[i].setSupercriticalWeirCoefficient(
            j, supercritical);

      } else if (code == 4 || code == 24) {
        ierr = IO::splitStringBoundary24Format(tempLine, n1, n2, crest,
                                               subcritical, supercritical);
        CHECK_FILEREAD_RETURN_INT(ierr);

        if (this->m_nodeOrderingLogical) {
          this->m_landBoundaries[i].setNode1(j, &this->m_nodes[n1 - 1]);
          this->m_landBoundaries[i].setNode2(j, &this->m_nodes[n2 - 1]);
        } else {
          this->m_landBoundaries[i].setNode1(
              j, &this->m_nodes[this->m_nodeLookup[n1]]);
          this->m_landBoundaries[i].setNode2(
              j, &this->m_nodes[this->m_nodeLookup[n2]]);
        }

        this->m_landBoundaries[i].setCrestElevation(j, crest);
        this->m_landBoundaries[i].setSubcriticalWeirCoefficient(j, subcritical);
        this->m_landBoundaries[i].setSupercriticalWeirCoefficient(
            j, supercritical);

      } else if (code == 5 || code == 25) {
        ierr = IO::splitStringBoundary25Format(tempLine, n1, n2, crest,
                                               subcritical, supercritical,
                                               pipeheight, pipecoef, pipediam);
        CHECK_FILEREAD_RETURN_INT(ierr);

        if (this->m_nodeOrderingLogical) {
          this->m_landBoundaries[i].setNode1(j, &this->m_nodes[n1 - 1]);
          this->m_landBoundaries[i].setNode2(j, &this->m_nodes[n2 - 1]);
        } else {
          this->m_landBoundaries[i].setNode1(
              j, &this->m_nodes[this->m_nodeLookup[n1]]);
          this->m_landBoundaries[i].setNode2(
              j, &this->m_nodes[this->m_nodeLookup[n2]]);
        }

        this->m_landBoundaries[i].setCrestElevation(j, crest);
        this->m_landBoundaries[i].setSubcriticalWeirCoefficient(j, subcritical);
        this->m_landBoundaries[i].setSupercriticalWeirCoefficient(
            j, supercritical);
        this->m_landBoundaries[i].setPipeHeight(j, pipeheight);
        this->m_landBoundaries[i].setPipeCoefficient(j, pipecoef);
        this->m_landBoundaries[i].setPipeDiameter(j, pipediam);
      } else {
        ierr = IO::splitStringBoundary0Format(tempLine, n1);
        CHECK_FILEREAD_RETURN_INT(ierr);
        if (this->m_nodeOrderingLogical)
          this->m_landBoundaries[i].setNode1(j, &this->m_nodes[n1 - 1]);
        else
          this->m_landBoundaries[i].setNode1(
              j, &this->m_nodes[this->m_nodeLookup[n1]]);
      }
    }
  }
  return Adcirc::NoError;
}

/**
 * @name AdcircMesh::elementalSearchTree
 * @brief Returns a reference to the elemental search kd-tree
 * @return kd-tree object with element centers as search locations
 */
QKdtree2 *AdcircMesh::elementalSearchTree() const {
  return m_elementalSearchTree;
}

/**
 * @name AdcircMesh::nodalSearchTree
 * @brief Returns a refrence to the nodal search kd-tree
 * @return kd-tree object with mesh nodes as serch locations
 */
QKdtree2 *AdcircMesh::nodalSearchTree() const { return m_nodalSearchTree; }

/**
 * @name AdcircMesh::totalLandBonudaryNodes
 * @brief Returns the number of land boundary nodes in the mesh
 * @return Number of nodes that fall on a land boundary
 */
int AdcircMesh::totalLandBoundaryNodes() {
  this->m_totalLandBoundaryNodes = 0;
  for (size_t i = 0; i < this->m_landBoundaries.size(); i++)
    this->m_totalLandBoundaryNodes += this->m_landBoundaries[i].length();
  return this->m_totalLandBoundaryNodes;
}

/**
 * @name AdcircMesh::totalOpenBoundaryNodes
 * @brief Returns the number of open boundary nodes in the mesh
 * @return Number of open boundary nodes
 */
int AdcircMesh::totalOpenBoundaryNodes() {
  this->m_totalOpenBoundaryNodes = 0;
  for (size_t i = 0; i < this->m_openBoundaries.size(); i++)
    this->m_totalOpenBoundaryNodes += this->m_openBoundaries[i].length();
  return this->m_totalOpenBoundaryNodes;
}

/**
 * @name AdcircMesh::node
 * @brief Returns a pointer to the requested node in the internal node vector
 * @param index location of the node in the vector
 * @return AdcircNode pointer
 */
AdcircNode *AdcircMesh::node(int index) {
  if (index >= 0 && index < this->numNodes())
    return &this->m_nodes[index];
  else
    return nullptr;
}

/**
 * @name AdcircMesh::element
 * @brief Returns a pointer to the requested element in the internal element
 * vector
 * @param index location of the node in the vector
 * @return AdcircElement pointer
 */
AdcircElement *AdcircMesh::element(int index) {
  if (index >= 0 && index < this->numElements())
    return &this->m_elements[index];
  else
    return nullptr;
}

/**
 * @name AdcircMesh::nodeById
 * @brief Returns a pointer to the requested node by its ID
 * @param id Nodal ID to return a reference to
 * @return AdcircNode pointer
 */
AdcircNode *AdcircMesh::nodeById(int id) {
  if (this->m_nodeOrderingLogical)
    if (id > 0 && id <= this->numNodes())
      return &this->m_nodes[id - 1];
    else
      return nullptr;
  else
    return &this->m_nodes[this->m_nodeLookup[id]];
}

/**
 * @name AdcircMesh::elementById
 * @brief Returns an pointer to the requested element by its ID
 * @param id Elemental ID to return a reference to
 * @return AdcircElement pointer
 */
AdcircElement *AdcircMesh::elementById(int id) {
  if (this->m_elementOrderingLogical)
    if (id > 0 && id <= this->numElements())
      return &this->m_elements[id - 1];
    else
      return nullptr;
  else
    return &this->m_elements[this->m_elementLookup[id]];
}

/**
 * @name AdcircMesh::openBoundary
 * @brief Returns a pointer to an open boundary by index
 * @param index index in the open boundary array
 * @return AdcircBoundary pointer
 */
AdcircBoundary *AdcircMesh::openBoundary(int index) {
  if (index >= 0 && index < this->numOpenBoundaries())
    return &this->m_openBoundaries[index];
  else
    return nullptr;
}

/**
 * @name AdcircMesh::landBoundary
 * @brief Returns a pointer to a land boundary by index
 * @param index index in the land boundary array
 * @return AdcircBoundary pointer
 */
AdcircBoundary *AdcircMesh::landBoundary(int index) {
  if (index >= 0 && index < this->numLandBoundaries())
    return &this->m_landBoundaries[index];
  else
    return nullptr;
}

/**
 * @name AdcircMesh::defineProjection
 * @brief Sets the mesh projection using an EPSG code. Note this does not
 * reproject the mesh.
 * @param epsg EPSG code for the mesh
 * @param isLatLon defines if the current projection is a lat/lon projection.
 * This helps define the significant digits when writing the mesh file
 */
void AdcircMesh::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
  return;
}

/**
 * @name AdcircMesh::projection
 * @brief Returns the EPSG code for the current mesh projection
 * @return EPSG code
 */
int AdcircMesh::projection() { return this->m_epsg; }

/**
 * @name AdcircMesh::isLatLon
 * @brief Returns true if the mesh is in a geographic projection
 * @return boolean value for mesh projection type
 */
bool AdcircMesh::isLatLon() { return this->m_isLatLon; }

/**
 * @name AdcircMesh::reproject
 * @brief Reprojects a mesh into the specified projection
 * @param epsg EPSG coordinate system to convert the mesh into
 * @return error code
 */
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

/**
 * @name AdcircMesh::toShapefile
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 * @return error code
 */
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

/**
 * @overload AdcircMesh::toShapefile
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 * @return error code
 */
int AdcircMesh::toShapefile(const char *outputFile) {
  std::string filename = std::string(outputFile);
  int ierr = this->toShapefile(filename);
  return ierr;
}

/**
 * @name AdcircMesh::buildNodalSearchTree
 * @brief Builds a kd-tree object with the mesh nodes as the search locations
 * @return error code
 */
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

/**
 * @name AdcircMesh::buildElementalSearchTree
 * @brief Builds a kd-tree object with the element centers as the search
 * locations
 * @return error code
 */
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

/**
 * @name AdcircMesh::nodalSearchTreeInitialized
 * @brief Returns a boolean value determining if the nodal search tree has been
 * initialized
 * @return true if the search tree is initialized
 */
bool AdcircMesh::nodalSearchTreeInitialized() {
  return this->m_nodalSearchTree->isInitialized();
}

/**
 * @name AdcircMesh::elementalSearchTreeInitialized
 * @brief Returns a boolean value determining if the elemental search tree has
 * been initialized
 * @return true of the search tree is initialized
 */
bool AdcircMesh::elementalSearchTreeInitialized() {
  return this->m_elementalSearchTree->isInitialized();
}

/**
 * @name AdcircMesh::resizeMesh
 * @brief Resizes the vectors within the mesh
 * @param numNodes Number of nodes
 * @param numElements Number of elements
 * @param numOpenBoundaries Number of open boundaries
 * @param numLandBoundaries Number of land boundaries
 */
void AdcircMesh::resizeMesh(int numNodes, int numElements,
                            int numOpenBoundaries, int numLandBoundaries) {
  if (numNodes != this->numNodes()) {
    this->m_nodes.resize(numNodes);
    this->setNumNodes(numNodes);
  }

  if (numElements != this->numElements()) {
    this->m_elements.resize(numElements);
    this->setNumElements(numElements);
  }

  if (numOpenBoundaries != this->numOpenBoundaries()) {
    this->m_openBoundaries.resize(numOpenBoundaries);
    this->setNumOpenBoundaries(numOpenBoundaries);
  }

  if (numLandBoundaries != this->numLandBoundaries()) {
    this->m_landBoundaries.resize(numLandBoundaries);
    this->setNumLandBoundaries(numLandBoundaries);
  }

  return;
}

/**
 * @name AdcircMesh::addNode
 * @brief Adds a node at the specified index in the node vector
 * @param index location where the node should be added
 * @param node Reference to an AdcircNode object
 */
void AdcircMesh::addNode(int index, AdcircNode &node) {
  unsigned long long index2 = static_cast<unsigned long long>(index);
  if (index < this->numNodes()) {
    this->m_nodes[index2] = node;
  }
  return;
}

/**
 * @name AdcircMesh::deleteNode
 * @brief Deletes an AdcircNode object at the specified index and shifts the
 * remaining nodes forward in the vector
 * @param index location where the node should be deleted from
 */
void AdcircMesh::deleteNode(int index) {
  if (index < this->numNodes()) {
    this->m_nodes.erase(this->m_nodes.begin() + index);
    this->setNumNodes(static_cast<int>(this->m_nodes.size()));
  }
  return;
}

/**
 * @name AdcircMesh::addElement
 * @brief Adds an AdcircElement at the specified position in the element vector
 * @param index location where the element should be added
 * @param element reference to the AdcircElement to add
 */
void AdcircMesh::addElement(int index, AdcircElement &element) {
  if (index < this->numElements()) {
    this->m_elements[index] = element;
  }
  return;
}

/**
 * @name AdcircMesh::deleteElement
 * @brief Deletes an AdcircElement object at the specified index and shifts the
 * remaining elements forward in the vector
 * @param index location where the element should be deleted from
 */
void AdcircMesh::deleteElement(int index) {
  if (index < this->numElements()) {
    this->m_elements.erase(this->m_elements.begin() +
                           static_cast<long long>(index));
    this->setNumElements(static_cast<int>(this->m_elements.size()));
  }
  return;
}

/**
 * @name AdcircMesh::write
 * @brief Writes an AdcircMesh object to disk in ASCII format
 * @param filename name of the output file to write
 * @return error code
 */
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

/**
 * @overload AdcircMesh::write
 * @brief Writes an AdcircMesh object to disk in ASCII format
 * @param filename name of the output file to write
 * @return error code
 */
int AdcircMesh::write(const char *filename) {
  return this->write(std::string(filename));
}

/**
 * @name AdcircMesh::nodeOrderingIsLogical
 * @brief Allows the user to know if the code has determined that the node
 * ordering is logcical (i.e. sequential) or not
 * @return true if node ordering is sequential
 */
bool AdcircMesh::nodeOrderingIsLogical() { return this->m_nodeOrderingLogical; }

/**
 * @name AdcircMesh::elementOrderingIsLogical
 * @brief Allows the user to know if the code has determined that the element
 * ordering is logcical (i.e. sequential) or not
 * @return true if element ordering is sequential
 */
bool AdcircMesh::elementOrderingIsLogical() {
  return this->m_elementOrderingLogical;
}

/**
 * @name AdcircMesh::nodeIndexById
 * @brief Returns the position in the array by node id
 * @param id nodal id
 * @return array position
 */
int AdcircMesh::nodeIndexById(int id){
  if(this->m_nodeOrderingLogical)
    return id;
  else
    return this->m_nodeLookup[id];
}

/**
 * @name AdcircMesh::elementIndexById
 * @brief Returns the position in the array by element id
 * @param id element id
 * @return array position
 */
int AdcircMesh::elementIndexById(int id){
  if(this->m_elementOrderingLogical)
    return id;
  else
    return this->m_elementLookup[id];
}
