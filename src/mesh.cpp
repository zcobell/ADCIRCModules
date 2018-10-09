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
#include "mesh.h"
#include <algorithm>
#include <list>
#include <set>
#include <string>
#include "boost/format.hpp"
#include "error.h"
#include "io.h"
#include "kdtree2lib.h"
#include "projection.h"
#include "shapefil.h"
#include "stringconversion.h"

using namespace std;
using namespace Adcirc;
using namespace Adcirc::Geometry;

/**
 * @brief Default Constructor
 */
Mesh::Mesh() : m_filename("none") { this->_init(); }

/**
 * @brief Default constructor with filename parameter
 * @param filename name of the mesh to read
 */
Mesh::Mesh(const std::string &filename) : m_filename(filename) {
  this->_init();
}

/**
 * @brief Initialization routine called by all constructors
 */
void Mesh::_init() {
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
 * @brief Mesh::~Mesh Destructor
 */
Mesh::~Mesh() {
  this->m_nodes.clear();
  this->m_elements.clear();
  this->m_openBoundaries.clear();
  this->m_landBoundaries.clear();
  this->m_nodeLookup.clear();
  this->m_elementLookup.clear();
}

/**
 * @brief Filename of the mesh to be read
 * @return Return the name of the mesh to be read
 */
string Mesh::filename() const { return this->m_filename; }

/**
 * @brief Sets the name of the mesh to be read
 * @param filename Name of the mesh
 */
void Mesh::setFilename(const string &filename) { this->m_filename = filename; }

/**
 * @brief Returns the mesh header from the processed mesh
 * @return mesh header
 */
string Mesh::meshHeaderString() const { return this->m_meshHeaderString; }

/**
 * @brief Sets the header for the mesh
 * @param meshHeaderString header
 */
void Mesh::setMeshHeaderString(const string &meshHeaderString) {
  this->m_meshHeaderString = meshHeaderString;
}

/**
 * @brief Returns the number of nodes currently in the mesh
 * @return number of nodes
 */
size_t Mesh::numNodes() const { return this->m_numNodes; }

/**
 * @brief Sets the number of nodes in the mesh. Note this does not resize the
 * mesh
 * @param numNodes number of nodes
 */
void Mesh::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

/**
 * @brief Returns the number of elements in the mesh
 * @return number of elements
 */
size_t Mesh::numElements() const { return this->m_numElements; }

/**
 * @brief Sets the number of elements in the mesh. Note: This does not resize
 * the mesh
 * @param numElements Number of elements
 */
void Mesh::setNumElements(size_t numElements) {
  this->m_numElements = numElements;
}

/**
 * @brief Returns the number of open boundaries in the model
 * @return number of open boundaries
 */
size_t Mesh::numOpenBoundaries() const { return this->m_numOpenBoundaries; }

/**
 * @brief Sets the number of open boundaries in the model. Note this does not
 * resize the boundary array
 * @param numOpenBoundaries Number of open boundaries
 */
void Mesh::setNumOpenBoundaries(size_t numOpenBoundaries) {
  this->m_numOpenBoundaries = numOpenBoundaries;
}

/**
 * @brief Returns the number of land boundaries in the model
 * @return number of land boundaries
 */
size_t Mesh::numLandBoundaries() const { return this->m_numLandBoundaries; }

/**
 * @brief Sets the number of land boundaries in the model. Note this does not
 * resize the boundary array
 * @param numLandBoundaries Number of land boundaries
 */
void Mesh::setNumLandBoundaries(size_t numLandBoundaries) {
  this->m_numLandBoundaries = numLandBoundaries;
}

/**
 * @brief Reads an ASCII formatted ADCIRC mesh. Note this will be extended in
 * the future to netCDF formatted meshes
 */
void Mesh::read() {
  std::fstream fid(this->filename());

  this->_readMeshHeader(fid);
  this->_readNodes(fid);
  this->_readElements(fid);
  this->_readOpenBoundaries(fid);
  this->_readLandBoundaries(fid);

  fid.close();

  return;
}

/**
 * @brief Reads the mesh title line and node/element dimensional data
 * @param fid std::fstream reference for the currently opened mesh
 */
void Mesh::_readMeshHeader(std::fstream &fid) {
  bool ok;
  size_t tempInt;
  string tempLine;
  vector<string> tempList;

  //...Read mesh header
  std::getline(fid, tempLine);

  //...Set the mesh file header
  this->setMeshHeaderString(tempLine);

  //..Get the size of the mesh
  std::getline(fid, tempLine);
  IO::splitString(tempLine, tempList);
  tempLine = tempList[0];
  tempInt = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading mesh header");
  }
  this->setNumElements(tempInt);

  tempLine = tempList[1];
  tempInt = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading mesh header");
  }
  this->setNumNodes(tempInt);

  return;
}

/**
 * @brief Reads the node section of the ASCII formatted mesh
 * @param fid std::fstream reference for the currently opened mesh
 */
void Mesh::_readNodes(std::fstream &fid) {
  this->m_nodes.resize(this->numNodes());

  size_t i = 0;
  for (auto &n : this->m_nodes) {
    size_t id;
    double x, y, z;
    string tempLine;
    std::getline(fid, tempLine);
    if (!IO::splitStringNodeFormat(tempLine, id, x, y, z)) {
      fid.close();
      adcircmodules_throw_exception("Error reading nodes");
    }

    if (i != id - 1) {
      this->m_nodeOrderingLogical = false;
    }

    n = Node(id, x, y, z);
    i++;
  }

  if (!this->m_nodeOrderingLogical) {
    this->m_nodeLookup.reserve(this->numNodes());

    for (size_t i = 0; i < this->m_numNodes; ++i) {
      this->m_nodeLookup[this->m_nodes[i].id()] = i;
    }
  }

  return;
}

/**
 * @brief Reads the elements section of the ASCII mesh
 * @param fid std::fstream reference for the currently opened mesh
 */
void Mesh::_readElements(std::fstream &fid) {
  size_t id;
  string tempLine;

  this->m_elements.resize(this->numElements());

  if (this->m_nodeOrderingLogical) {
    for (auto &e : this->m_elements) {
      std::getline(fid, tempLine);
      std::vector<size_t> n;
      n.reserve(4);
      if (!IO::splitStringElemFormat(tempLine, id, n)) {
        fid.close();
        adcircmodules_throw_exception("Error reading elements");
      }
      if (n.size() == 3) {
        e.setElement(id, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
                     &this->m_nodes[n[2] - 1]);
      } else if (n.size() == 4) {
        e.setElement(id, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
                     &this->m_nodes[n[2] - 1], &this->m_nodes[n[3] - 1]);
      }
    }
  } else {
    size_t i = 0;
    for (auto &e : this->m_elements) {
      std::getline(fid, tempLine);
      std::vector<size_t> n;
      n.reserve(4);
      if (!IO::splitStringElemFormat(tempLine, id, n)) {
        fid.close();
        adcircmodules_throw_exception("Error reading nodes");
      }

      if (i != id - 1) {
        this->m_elementOrderingLogical = false;
      }
      if (n.size() == 3) {
        e.setElement(id, &this->m_nodes[this->m_nodeLookup[n[0]]],
                     &this->m_nodes[this->m_nodeLookup[n[1]]],
                     &this->m_nodes[this->m_nodeLookup[n[2]]]);
      } else if (n.size() == 4) {
        e.setElement(id, &this->m_nodes[this->m_nodeLookup[n[0]]],
                     &this->m_nodes[this->m_nodeLookup[n[1]]],
                     &this->m_nodes[this->m_nodeLookup[n[2]]],
                     &this->m_nodes[this->m_nodeLookup[n[3]]]);
      }
      i++;
    }
  }

  if (!this->m_elementOrderingLogical) {
    this->m_elementLookup.reserve(this->m_numElements);
    for (size_t i = 0; i < this->m_numElements; ++i) {
      this->m_elementLookup[this->m_elements[i].id()] = i;
    }
  }

  return;
}

/**
 * @brief Reads the open boundaries section of the ASCII formatted mesh file
 * @param fid std::fstream reference for the currently opened mesh
 */
void Mesh::_readOpenBoundaries(std::fstream &fid) {
  string tempLine;
  vector<string> tempList;

  std::getline(fid, tempLine);
  IO::splitString(tempLine, tempList);

  bool ok;
  this->setNumOpenBoundaries(StringConversion::stringToSizet(tempList[0], ok));
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading number of open boundaries");
  }

  this->m_openBoundaries.resize(this->numOpenBoundaries());

  std::getline(fid, tempLine);

  for (auto &b : this->m_openBoundaries) {
    std::getline(fid, tempLine);
    IO::splitString(tempLine, tempList);

    size_t length = StringConversion::stringToSizet(tempList[0], ok);
    if (!ok) {
      fid.close();
      adcircmodules_throw_exception("Error reading boundaries");
    }

    b.setBoundary(-1, length);

    for (size_t j = 0; j < b.length(); ++j) {
      std::getline(fid, tempLine);
      size_t nid;
      if (!IO::splitStringBoundary0Format(tempLine, nid)) {
        fid.close();
        adcircmodules_throw_exception("Error reading boundaries");
      }

      if (this->m_nodeOrderingLogical) {
        b.setNode1(j, &this->m_nodes[nid - 1]);
      } else {
        b.setNode1(j, &this->m_nodes[this->m_nodeLookup[nid]]);
      }
    }
  }
  return;
}

/**
 * @brief Reads the land boundaries section of the ASCII formatted mesh file
 * @param fid std::fstream reference for the currently opened mesh
 */
void Mesh::_readLandBoundaries(std::fstream &fid) {
  string tempLine;
  vector<string> tempList;

  size_t n1, n2;
  double supercritical, subcritical, crest, pipeheight, pipediam, pipecoef;
  bool ok;

  std::getline(fid, tempLine);

  IO::splitString(tempLine, tempList);

  this->setNumLandBoundaries(StringConversion::stringToSizet(tempList[0], ok));
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading boundaries");
  }
  this->m_landBoundaries.resize(this->numLandBoundaries());

  std::getline(fid, tempLine);

  for (auto &b : this->m_landBoundaries) {
    std::getline(fid, tempLine);
    IO::splitString(tempLine, tempList);

    size_t length = StringConversion::stringToSizet(tempList[0], ok);
    if (!ok) {
      fid.close();
      adcircmodules_throw_exception("Error reading boundaries");
    }
    int code = StringConversion::stringToInt(tempList[1], ok);
    if (!ok) {
      fid.close();
      adcircmodules_throw_exception("Error reading boundaries");
    }

    b.setBoundary(code, length);

    for (size_t j = 0; j < b.length(); ++j) {
      std::getline(fid, tempLine);

      if (code == 3 || code == 13 || code == 23) {
        if (!IO::splitStringBoundary23Format(tempLine, n1, crest,
                                             supercritical)) {
          fid.close();
          adcircmodules_throw_exception("Error reading boundaries");
        }
        if (this->m_nodeOrderingLogical) {
          b.setNode1(j, &this->m_nodes[n1 - 1]);
        } else {
          b.setNode1(j, &this->m_nodes[this->m_nodeLookup[n1]]);
        }

        b.setCrestElevation(j, crest);
        b.setSupercriticalWeirCoefficient(j, supercritical);

      } else if (code == 4 || code == 24) {
        if (!IO::splitStringBoundary24Format(tempLine, n1, n2, crest,
                                             subcritical, supercritical)) {
          fid.close();
          adcircmodules_throw_exception("Error reading boundaries");
        }

        if (this->m_nodeOrderingLogical) {
          b.setNode1(j, &this->m_nodes[n1 - 1]);
          b.setNode2(j, &this->m_nodes[n2 - 1]);
        } else {
          b.setNode1(j, &this->m_nodes[this->m_nodeLookup[n1]]);
          b.setNode2(j, &this->m_nodes[this->m_nodeLookup[n2]]);
        }

        b.setCrestElevation(j, crest);
        b.setSubcriticalWeirCoefficient(j, subcritical);
        b.setSupercriticalWeirCoefficient(j, supercritical);

      } else if (code == 5 || code == 25) {
        if (!IO::splitStringBoundary25Format(tempLine, n1, n2, crest,
                                             subcritical, supercritical,
                                             pipeheight, pipecoef, pipediam)) {
          fid.close();
          adcircmodules_throw_exception("Error reading boundaries");
        }

        if (this->m_nodeOrderingLogical) {
          b.setNode1(j, &this->m_nodes[n1 - 1]);
          b.setNode2(j, &this->m_nodes[n2 - 1]);
        } else {
          b.setNode1(j, &this->m_nodes[this->m_nodeLookup[n1]]);
          b.setNode2(j, &this->m_nodes[this->m_nodeLookup[n2]]);
        }

        b.setCrestElevation(j, crest);
        b.setSubcriticalWeirCoefficient(j, subcritical);
        b.setSupercriticalWeirCoefficient(j, supercritical);
        b.setPipeHeight(j, pipeheight);
        b.setPipeCoefficient(j, pipecoef);
        b.setPipeDiameter(j, pipediam);
      } else {
        if (!IO::splitStringBoundary0Format(tempLine, n1)) {
          fid.close();
          adcircmodules_throw_exception("Error reading boundaries");
        }
        if (this->m_nodeOrderingLogical) {
          b.setNode1(j, &this->m_nodes[n1 - 1]);
        } else {
          b.setNode1(j, &this->m_nodes[this->m_nodeLookup[n1]]);
        }
      }
    }
  }
  return;
}

/**
 * @brief Returns a reference to the elemental search kd-tree
 * @return kd-tree object with element centers as search locations
 */
QKdtree2 *Mesh::elementalSearchTree() const {
  return m_elementalSearchTree.get();
}

/**
 * @brief Returns a refrence to the nodal search kd-tree
 * @return kd-tree object with mesh nodes as serch locations
 */
QKdtree2 *Mesh::nodalSearchTree() const { return m_nodalSearchTree.get(); }

/**
 * @brief Returns the number of land boundary nodes in the mesh
 * @return Number of nodes that fall on a land boundary
 */
size_t Mesh::totalLandBoundaryNodes() {
  this->m_totalLandBoundaryNodes = 0;
  for (auto &m_landBoundarie : this->m_landBoundaries) {
    this->m_totalLandBoundaryNodes += m_landBoundarie.length();
  }
  return this->m_totalLandBoundaryNodes;
}

/**
 * @brief Sets the z-values of the mesh to the values found within a vector
 * @param z values that will be set to the mesh nodes z attribute
 */
void Mesh::setZ(std::vector<double> z) {
  assert(z.size() == this->m_numNodes);
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    this->m_nodes[i].setZ(z[i]);
  }
  return;
}

/**
 * @brief Returns the number of open boundary nodes in the mesh
 * @return Number of open boundary nodes
 */
size_t Mesh::totalOpenBoundaryNodes() {
  this->m_totalOpenBoundaryNodes = 0;
  for (auto &m_openBoundarie : this->m_openBoundaries) {
    this->m_totalOpenBoundaryNodes += m_openBoundarie.length();
  }
  return this->m_totalOpenBoundaryNodes;
}

/**
 * @brief Returns a pointer to the requested node in the internal node vector
 * @param index location of the node in the vector
 * @return Node pointer
 */
Node *Mesh::node(size_t index) {
  if (index < this->numNodes()) {
    return &this->m_nodes[index];
  } else {
    adcircmodules_throw_exception("Mesh: Node index " + to_string(index) +
                                  " out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns a pointer to the requested element in the internal element
 * vector
 * @param index location of the node in the vector
 * @return Element pointer
 */
Element *Mesh::element(size_t index) {
  if (index < this->numElements()) {
    return &this->m_elements[index];
  } else {
    adcircmodules_throw_exception("Mesh: Element index out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns a pointer to the requested node by its ID
 * @param id Nodal ID to return a reference to
 * @return Node pointer
 */
Node *Mesh::nodeById(size_t id) {
  if (this->m_nodeOrderingLogical) {
    if (id > 0 && id <= this->numNodes()) {
      return &this->m_nodes[id - 1];
    } else {
      adcircmodules_throw_exception("Mesh: Node id " + to_string(id) +
                                    " not found");
      return nullptr;
    }
  } else {
    return &this->m_nodes[this->m_nodeLookup[id]];
  }
}

/**
 * @brief Returns an pointer to the requested element by its ID
 * @param id Elemental ID to return a reference to
 * @return Element pointer
 */
Element *Mesh::elementById(size_t id) {
  if (this->m_elementOrderingLogical) {
    if (id > 0 && id <= this->numElements()) {
      return &this->m_elements[id - 1];
    } else {
      adcircmodules_throw_exception("Mesh: Element id not found");
      return nullptr;
    }
  } else {
    return &this->m_elements[this->m_elementLookup[id]];
  }
}

/**
 * @brief Returns a pointer to an open boundary by index
 * @param index index in the open boundary array
 * @return Boundary pointer
 */
Boundary *Mesh::openBoundary(size_t index) {
  if (index < this->numOpenBoundaries()) {
    return &this->m_openBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Open boundary index out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns a pointer to a land boundary by index
 * @param index index in the land boundary array
 * @return Boundary pointer
 */
Boundary *Mesh::landBoundary(size_t index) {
  if (index < this->numLandBoundaries()) {
    return &this->m_landBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Land boundary index out of bounds");
    return nullptr;
  }
}

/**
 * @brief Sets the mesh projection using an EPSG code. Note this does not
 * reproject the mesh.
 * @param epsg EPSG code for the mesh
 * @param isLatLon defines if the current projection is a lat/lon projection.
 * This helps define the significant digits when writing the mesh file
 */
void Mesh::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
  return;
}

/**
 * @brief Returns the EPSG code for the current mesh projection
 * @return EPSG code
 */
int Mesh::projection() { return this->m_epsg; }

/**
 * @brief Returns true if the mesh is in a geographic projection
 * @return boolean value for mesh projection type
 */
bool Mesh::isLatLon() { return this->m_isLatLon; }

/**
 * @brief Reprojects a mesh into the specified projection
 * @param epsg EPSG coordinate system to convert the mesh into
 */
void Mesh::reproject(int epsg) {
  Projection proj;
  bool isLatLon;
  vector<Point> inPoint, outPoint;
  inPoint.reserve(this->numNodes());
  outPoint.resize(this->numNodes());

  for (const auto &n : this->m_nodes) {
    inPoint.push_back(Point(n.x(), n.y()));
  }

  int ierr =
      proj.transform(this->projection(), epsg, inPoint, outPoint, isLatLon);

  if (ierr != Projection::NoError) {
    adcircmodules_throw_exception("Mesh: Proj4 library error");
  }

  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->node(i)->setX(outPoint[i].x());
    this->node(i)->setY(outPoint[i].y());
  }

  this->defineProjection(epsg, isLatLon);

  return;
}

/**
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toNodeShapefile(const string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_POINT);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());

  DBFAddField(dbfid, "nodeid", FTInteger, 16, 0);
  DBFAddField(dbfid, "longitude", FTDouble, 16, 8);
  DBFAddField(dbfid, "latitude", FTDouble, 16, 8);
  DBFAddField(dbfid, "elevation", FTDouble, 16, 4);

  for (auto &n : this->m_nodes) {
    int nodeid = static_cast<int>(n.id());
    double longitude = n.x();
    double latitude = n.y();
    double elevation = n.z();

    SHPObject *shpobj =
        SHPCreateSimpleObject(SHPT_POINT, 1, &longitude, &latitude, &elevation);
    int shp_index = SHPWriteObject(shpid, -1, shpobj);
    SHPDestroyObject(shpobj);

    DBFWriteIntegerAttribute(dbfid, shp_index, 0, nodeid);
    DBFWriteDoubleAttribute(dbfid, shp_index, 1, longitude);
    DBFWriteDoubleAttribute(dbfid, shp_index, 2, latitude);
    DBFWriteDoubleAttribute(dbfid, shp_index, 3, elevation);
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return;
}

/**
 * @brief Writes the mesh connectivity into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toConnectivityShapefile(const string &outputFile) {
  std::vector<std::pair<Node *, Node *>> legs;
  legs.reserve(this->m_numElements * 4);

  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_ARC);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());

  DBFAddField(dbfid, "node1", FTInteger, 16, 0);
  DBFAddField(dbfid, "node2", FTInteger, 16, 0);
  DBFAddField(dbfid, "znode1", FTDouble, 16, 4);
  DBFAddField(dbfid, "znode2", FTDouble, 16, 4);

  for (auto &e : this->m_elements) {
    for (int j = 0; j < e.n(); ++j) {
      std::pair<Node *, Node *> p1 = e.elementLeg(j);
      std::pair<Node *, Node *> p2;
      if (p1.first->id() > p1.second->id()) {
        p2 = std::pair<Node *, Node *>(p1.second, p1.first);
      } else {
        p2 = std::move(p1);
      }
      legs.push_back(p2);
    }
  }

  std::set<std::pair<Node *, Node *>> s(legs.begin(), legs.end());
  legs.clear();
  legs.assign(s.begin(), s.end());
  s.clear();

  for (auto &l : legs) {
    double latitude[2], longitude[2], elevation[2];
    int nodeid[2];

    nodeid[0] = l.first->id();
    nodeid[1] = l.second->id();
    longitude[0] = l.first->x();
    longitude[1] = l.second->x();
    latitude[0] = l.first->y();
    latitude[1] = l.second->y();
    elevation[0] = l.first->z();
    elevation[1] = l.second->z();

    SHPObject *shpobj =
        SHPCreateSimpleObject(SHPT_ARC, 2, longitude, latitude, elevation);
    int shp_index = SHPWriteObject(shpid, -1, shpobj);
    SHPDestroyObject(shpobj);

    DBFWriteIntegerAttribute(dbfid, shp_index, 0, nodeid[0]);
    DBFWriteIntegerAttribute(dbfid, shp_index, 1, nodeid[1]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 2, elevation[0]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 3, elevation[1]);
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return;
}

/**
 * @brief Writes the mesh elements as polygons into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void Mesh::toElementShapefile(const string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_POLYGONZ);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());
  DBFAddField(dbfid, "elementid", FTInteger, 16, 0);
  DBFAddField(dbfid, "node1", FTInteger, 16, 0);
  DBFAddField(dbfid, "node2", FTInteger, 16, 0);
  DBFAddField(dbfid, "node3", FTInteger, 16, 0);
  DBFAddField(dbfid, "node4", FTInteger, 16, 0);
  DBFAddField(dbfid, "znode1", FTDouble, 16, 4);
  DBFAddField(dbfid, "znode2", FTDouble, 16, 4);
  DBFAddField(dbfid, "znode3", FTDouble, 16, 4);
  DBFAddField(dbfid, "znode4", FTDouble, 16, 4);
  DBFAddField(dbfid, "zmean", FTDouble, 16, 4);

  for (auto &e : this->m_elements) {
    double zmean;
    double *latitude = new double(e.n());
    double *longitude = new double(e.n());
    double nodeid[4], nodez[4];
    int id = static_cast<int>(e.id());

    zmean = 0.0;
    for (size_t i = 0; i < e.n(); ++i) {
      longitude[i] = e.node(i)->x();
      latitude[i] = e.node(i)->y();
      nodez[i] = e.node(i)->z();
      nodeid[i] = e.node(i)->id();
      zmean += nodez[i];
    }
    zmean = zmean / e.n();

    if (e.n() == 3) {
      nodez[3] = -9999.0;
      nodeid[3] = -1;
    }

    SHPObject *shpobj =
        SHPCreateSimpleObject(SHPT_POLYGONZ, e.n(), longitude, latitude, nodez);
    int shp_index = SHPWriteObject(shpid, -1, shpobj);
    SHPDestroyObject(shpobj);

    DBFWriteIntegerAttribute(dbfid, shp_index, 0, id);
    DBFWriteIntegerAttribute(dbfid, shp_index, 1, nodeid[0]);
    DBFWriteIntegerAttribute(dbfid, shp_index, 2, nodeid[1]);
    DBFWriteIntegerAttribute(dbfid, shp_index, 3, nodeid[2]);
    DBFWriteIntegerAttribute(dbfid, shp_index, 4, nodeid[3]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 5, nodez[0]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 6, nodez[1]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 7, nodez[2]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 8, nodez[3]);
    DBFWriteDoubleAttribute(dbfid, shp_index, 9, zmean);

    delete latitude;
    delete longitude;
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return;
}

/**
 * @brief Builds a kd-tree object with the mesh nodes as the search locations
 */
void Mesh::buildNodalSearchTree() {
  int ierr;
  vector<double> x, y;

  x.reserve(this->numNodes());
  y.reserve(this->numNodes());

  for (const auto &n : this->m_nodes) {
    x.push_back(n.x());
    y.push_back(n.y());
  }

  if (this->m_nodalSearchTree != nullptr) {
    if (this->m_nodalSearchTree->isInitialized()) {
      this->m_nodalSearchTree.reset(nullptr);
    }
  }

  this->m_nodalSearchTree = unique_ptr<QKdtree2>(new QKdtree2());
  ierr = this->m_nodalSearchTree->build(x, y);
  if (ierr != QKdtree2::NoError) {
    adcircmodules_throw_exception("Mesh: KDTree2 library error");
  }

  return;
}

/**
 * @brief Builds a kd-tree object with the element centers as the search
 * locations
 */
void Mesh::buildElementalSearchTree() {
  vector<double> x, y;

  x.reserve(this->numElements());
  y.reserve(this->numElements());

  for (auto &e : this->m_elements) {
    double tempX = 0.0;
    double tempY = 0.0;
    for (int j = 0; j < e.n(); ++j) {
      tempX = tempX + e.node(j)->x();
      tempY = tempY + e.node(j)->y();
    }
    x.push_back(tempX / e.n());
    y.push_back(tempY / e.n());
  }

  if (this->m_elementalSearchTree != nullptr) {
    if (this->m_elementalSearchTree->isInitialized()) {
      this->m_elementalSearchTree.reset(nullptr);
    }
  }

  this->m_elementalSearchTree = unique_ptr<QKdtree2>(new QKdtree2());
  int ierr = this->m_elementalSearchTree->build(x, y);
  if (ierr != QKdtree2::NoError) {
    adcircmodules_throw_exception("Mesh: KDTree2 library error");
  }

  return;
}

/**
 * @brief Deletes the nodal search tree
 */
void Mesh::deleteNodalSearchTree() {
  if (this->nodalSearchTreeInitialized()) {
    this->m_nodalSearchTree.reset();
  }
  return;
}

/**
 * @brief Deletes the elemental search tree
 */
void Mesh::deleteElementalSearchTree() {
  if (this->elementalSearchTreeInitialized()) {
    this->m_elementalSearchTree.reset();
  }
  return;
}

/**
 * @brief Returns a boolean value determining if the nodal search tree has been
 * initialized
 * @return true if the search tree is initialized
 */
bool Mesh::nodalSearchTreeInitialized() {
  return this->m_nodalSearchTree->isInitialized();
}

/**
 * @brief Returns a boolean value determining if the elemental search tree has
 * been initialized
 * @return true of the search tree is initialized
 */
bool Mesh::elementalSearchTreeInitialized() {
  return this->m_elementalSearchTree->isInitialized();
}

/**
 * @brief Resizes the vectors within the mesh
 * @param numNodes Number of nodes
 * @param numElements Number of elements
 * @param numOpenBoundaries Number of open boundaries
 * @param numLandBoundaries Number of land boundaries
 */
void Mesh::resizeMesh(size_t numNodes, size_t numElements,
                      size_t numOpenBoundaries, size_t numLandBoundaries) {
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
 * @brief Adds a node at the specified index in the node vector
 * @param index location where the node should be added
 * @param node Reference to an Node object
 */
void Mesh::addNode(size_t index, Node &node) {
  if (index < this->numNodes()) {
    this->m_nodes[index] = node;
  } else {
    adcircmodules_throw_exception("Mesh: Node index > number of nodes");
  }

  return;
}

/**
 * @brief Deletes an Node object at the specified index and shifts the
 * remaining nodes forward in the vector
 * @param index location where the node should be deleted from
 */
void Mesh::deleteNode(size_t index) {
  if (index < this->numNodes()) {
    this->m_nodes.erase(this->m_nodes.begin() + index);
    this->setNumNodes(this->m_nodes.size());
  } else {
    adcircmodules_throw_exception("Mesh: Node index > number of nodes");
  }
  return;
}

/**
 * @brief Adds an Element at the specified position in the element vector
 * @param index location where the element should be added
 * @param element reference to the Element to add
 */
void Mesh::addElement(size_t index, Element &element) {
  if (index < this->numElements()) {
    this->m_elements[index] = element;
  } else {
    adcircmodules_throw_exception("Mesh: Element index > number of nodes");
  }
  return;
}

/**
 * @brief Deletes an Element object at the specified index and shifts the
 * remaining elements forward in the vector
 * @param index location where the element should be deleted from
 */
void Mesh::deleteElement(size_t index) {
  if (index < this->numElements()) {
    this->m_elements.erase(this->m_elements.begin() + index);
    this->setNumElements(this->m_elements.size());
  } else {
    adcircmodules_throw_exception("Mesh: Element index > number of nodes");
  }
  return;
}

/**
 * @brief Writes an Mesh object to disk in ASCII format
 * @param filename name of the output file to write
 */
void Mesh::write(const string &filename) {
  string tempString;
  std::ofstream outputFile;

  outputFile.open(filename);

  //...Write the header
  outputFile << this->meshHeaderString() << "\n";
  tempString = boost::str(boost::format("%11i %11i") % this->numElements() %
                          this->numNodes());
  outputFile << tempString << "\n";

  //...Write the mesh nodes
  for (auto n : this->m_nodes) {
    outputFile << n.toString(this->isLatLon()) << "\n";
  }

  //...Write the mesh elements
  for (auto &e : this->m_elements) {
    outputFile << e.toString() << "\n";
  }

  //...Write the open boundary header
  outputFile << this->numOpenBoundaries() << "\n";
  outputFile << this->totalOpenBoundaryNodes() << "\n";

  //...Write the open boundaries
  for (auto &b : this->m_openBoundaries) {
    for (const auto &j : b.toStringList()) {
      outputFile << j << "\n";
    }
  }

  //...Write the land boundary header
  outputFile << this->numLandBoundaries() << "\n";
  outputFile << this->totalLandBoundaryNodes() << "\n";

  //...Write the land boundaries
  for (auto &b : this->m_landBoundaries) {
    for (const auto &j : b.toStringList()) {
      outputFile << j << "\n";
    }
  }

  //...Close the file
  outputFile.close();

  return;
}

/**
 * @brief Allows the user to know if the code has determined that the node
 * ordering is logcical (i.e. sequential) or not
 * @return true if node ordering is sequential
 */
bool Mesh::nodeOrderingIsLogical() { return this->m_nodeOrderingLogical; }

/**
 * @brief Allows the user to know if the code has determined that the element
 * ordering is logcical (i.e. sequential) or not
 * @return true if element ordering is sequential
 */
bool Mesh::elementOrderingIsLogical() { return this->m_elementOrderingLogical; }

/**
 * @brief Returns the position in the array by node id
 * @param id nodal id
 * @return array position
 */
size_t Mesh::nodeIndexById(size_t id) {
  if (this->m_nodeOrderingLogical) {
    return id - 1;
  } else {
    return this->m_nodeLookup[id];
  }
}

/**
 * @brief Returns the position in the array by element id
 * @param id element id
 * @return array position
 */
size_t Mesh::elementIndexById(size_t id) {
  if (this->m_elementOrderingLogical) {
    return id;
  } else {
    return this->m_elementLookup[id];
  }
}

/**
 * @brief Returns a vector of the x-coordinates
 * @return vector of x coordinates
 *
 * Implemented mostly for the python interface
 */
vector<double> Mesh::x() {
  vector<double> x;
  x.reserve(this->numNodes());
  for (const auto &n : this->m_nodes) {
    x.push_back(n.x());
  }
  return x;
}

/**
 * @brief Returns a vector of the y-coordinates
 * @return vector of y coordinates
 *
 * Implemented mostly for the python interface
 */
vector<double> Mesh::y() {
  vector<double> y;
  y.reserve(this->numNodes());
  for (const auto &n : this->m_nodes) {
    y.push_back(n.y());
  }
  return y;
}

/**
 * @brief Returns a vector of the z-coordinates
 * @return vector of z coordinates
 *
 * Implemented mostly for the python interface
 */
vector<double> Mesh::z() {
  vector<double> z;
  z.reserve(this->numNodes());
  for (const auto &n : this->m_nodes) {
    z.push_back(n.z());
  }
  return z;
}

/**
 * @brief Returns a 2d-vector of the xyz-coordinates
 * @return 2d-vector of xyz coordinates
 *
 * Implemented mostly for the python interface
 */
vector<vector<double>> Mesh::xyz() {
  vector<vector<double>> xyz;
  xyz.resize(3);
  xyz[0] = this->x();
  xyz[1] = this->y();
  xyz[2] = this->z();
  return xyz;
}

/**
 * @brief Returns a 2d-vector of the mesh connectivity
 * @return 2d-vector of mesh connectivity
 *
 * Implemented mostly for the python interface
 */
vector<vector<size_t>> Mesh::connectivity() {
  vector<vector<size_t>> conn;
  conn.reserve(this->numElements());
  for (auto &e : this->m_elements) {
    vector<size_t> v;
    v.resize(e.n());
    for (int i = 0; i < e.n(); ++i) {
      v[i] = e.node(i)->id();
    }
    conn.push_back(v);
  }
  return conn;
}

/**
 * @brief Convertes mesh to the carte parallelogrammatique projection
 *
 * This is the projection used within adcirc internally
 */
void Mesh::cpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    Point i = Point(n.x(), n.y());
    Point o;
    Projection::cpp(lambda, phi, i, o);
    n.setX(o.x());
    n.setY(o.y());
  }
  return;
}

/**
 * @brief Convertes mesh back from the carte parallelogrammatique projection
 */
void Mesh::inverseCpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    Point i = Point(n.x(), n.y());
    Point o;
    Projection::inverseCpp(lambda, phi, i, o);
    n.setX(o.x());
    n.setY(o.y());
  }
  return;
}

/**
 * @brief Finds the nearest mesh node to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest node index
 */
size_t Mesh::findNearestNode(double x, double y) {
  return this->findNearestNode(Point(x, y));
}

/**
 * @param location location to search
 * @return nearest node index
 */
size_t Mesh::findNearestNode(Point location) {
  if (!this->nodalSearchTreeInitialized()) {
    this->buildNodalSearchTree();
  }
  return this->nodalSearchTree()->findNearest(location);
}

/**
 * @brief Finds the nearest mesh element to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest element index
 */
size_t Mesh::findNearestElement(double x, double y) {
  return this->findNearestElement(Point(x, y));
}

/**
 * @param location location to search
 * @return nearest element index
 */
size_t Mesh::findNearestElement(Point location) {
  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }
  return this->elementalSearchTree()->findNearest(location);
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param x location to search
 * @param y location to search
 * @return index of nearest element, -1 if not found
 */
size_t Mesh::findElement(double x, double y) {
  return this->findElement(Point(x, y));
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param location location to search
 * @return index of nearest element, -1 if not found
 */
size_t Mesh::findElement(Point location) {
  const int searchDepth = 20;

  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }

  vector<size_t> indicies;
  indicies = this->elementalSearchTree()->findXNearest(location, searchDepth);

  for (auto i : indicies) {
    bool found = this->element(i)->isInside(location);
    if (found) {
      return i;
    }
  }
  return -1;
}
