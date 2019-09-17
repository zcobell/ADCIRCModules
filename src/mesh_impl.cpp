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
#include "mesh_impl.h"
#include <algorithm>
#include <set>
#include <string>
#include <tuple>
#include "boost/format.hpp"
#include "default_values.h"
#include "elementtable.h"
#include "ezproj.h"
#include "fileio.h"
#include "filetypes.h"
#include "hash.h"
#include "kdtree.h"
#include "logging.h"
#include "mesh.h"
#include "netcdf.h"
#include "shapefil.h"
#include "stringconversion.h"

using namespace Adcirc;
using namespace Adcirc::Geometry;

Mesh::~Mesh() = default;

/**
 * @brief Default Constructor
 */
MeshImpl::MeshImpl()
    : m_filename("none"),
      m_epsg(-1),
      m_hash(nullptr),
      m_hashType(AdcircDefaultHash) {
  this->_init();
}

/**
 * @brief Default constructor with filename parameter
 * @param filename name of the mesh to read
 */
MeshImpl::MeshImpl(const std::string &filename)
    : m_filename(filename),
      m_epsg(-1),
      m_hash(nullptr),
      m_hashType(AdcircDefaultHash) {
  this->_init();
}

/**
 * @brief Initialization routine called by all constructors
 */
void MeshImpl::_init() {
  if (this->m_epsg == -1) this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
  this->m_nodeOrderingLogical = true;
  this->m_elementOrderingLogical = true;
  this->m_nodes.clear();
  this->m_elements.clear();
  this->m_openBoundaries.clear();
  this->m_landBoundaries.clear();
  this->m_elementalSearchTree = std::unique_ptr<Kdtree>(new Kdtree());
  this->m_nodalSearchTree = std::unique_ptr<Kdtree>(new Kdtree());
}

/**
 * @brief Mesh::~Mesh Destructor
 */
MeshImpl::~MeshImpl() {
  this->m_nodes.clear();
  this->m_elements.clear();
  this->m_openBoundaries.clear();
  this->m_landBoundaries.clear();
  this->m_nodeLookup.clear();
  this->m_elementLookup.clear();
  if (this->m_hash != nullptr) delete[] this->m_hash;
}

/**
 * @brief Filename of the mesh to be read
 * @return Return the name of the mesh to be read
 */
std::string MeshImpl::filename() const { return this->m_filename; }

/**
 * @brief Sets the name of the mesh to be read
 * @param filename Name of the mesh
 */
void MeshImpl::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

/**
 * @brief Returns the mesh header from the processed mesh
 * @return mesh header
 */
std::string MeshImpl::meshHeaderString() const {
  return this->m_meshHeaderString;
}

/**
 * @brief Sets the header for the mesh
 * @param meshHeaderString header
 */
void MeshImpl::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_meshHeaderString = meshHeaderString;
}

/**
 * @brief Returns the number of nodes currently in the mesh
 * @return number of nodes
 */
size_t MeshImpl::numNodes() const { return this->m_nodes.size(); }

/**
 * @brief Sets the number of nodes in the mesh
 * @param numNodes number of nodes
 */
void MeshImpl::setNumNodes(size_t numNodes) { this->m_nodes.resize(numNodes); }

/**
 * @brief Returns the number of elements in the mesh
 * @return number of elements
 */
size_t MeshImpl::numElements() const { return this->m_elements.size(); }

/**
 * @brief Sets the number of elements in the mesh
 * @param numElements Number of elements
 */
void MeshImpl::setNumElements(size_t numElements) {
  this->m_elements.resize(numElements);
}

/**
 * @brief Returns the number of open boundaries in the model
 * @return number of open boundaries
 */
size_t MeshImpl::numOpenBoundaries() const {
  return this->m_openBoundaries.size();
}

/**
 * @brief Sets the number of open boundaries in the model
 * @param numOpenBoundaries Number of open boundaries
 */
void MeshImpl::setNumOpenBoundaries(size_t numOpenBoundaries) {
  this->m_openBoundaries.resize(numOpenBoundaries);
}

/**
 * @brief Returns the number of land boundaries in the model
 * @return number of land boundaries
 */
size_t MeshImpl::numLandBoundaries() const {
  return this->m_landBoundaries.size();
}

/**
 * @brief Sets the number of land boundaries in the model
 * @param numLandBoundaries Number of land boundaries
 */
void MeshImpl::setNumLandBoundaries(size_t numLandBoundaries) {
  this->m_landBoundaries.resize(numLandBoundaries);
}

/**
 * @brief Reads a specified mesh format
 * @param[optional] format MeshFormat enum describing the format of the mesh
 *
 * Reads the unstructured mesh into a mesh object. If no format is
 * specified, then it will be guessed from the file extension
 */
void MeshImpl::read(MeshFormat format) {
  if (this->m_filename == std::string()) {
    adcircmodules_throw_exception("No filename has been specified.");
  }
  if (!FileIO::Generic::fileExists(this->m_filename)) {
    adcircmodules_throw_exception("File does not exist.");
  }

  MeshFormat fmt;
  if (format == MeshUnknown) {
    fmt = this->getMeshFormat(this->m_filename);
  } else {
    fmt = format;
  }

  //...Wipes the old data if it was there
  this->_init();

  switch (fmt) {
    case MeshAdcirc:
      this->readAdcircMeshAscii();
      break;
    case MeshAdcircNetcdf:
      this->readAdcircMeshNetcdf();
      break;
    case Mesh2DM:
      this->read2dmMesh();
      break;
    case MeshDFlow:
      this->readDflowMesh();
      break;
    default:
      adcircmodules_throw_exception("Invalid mesh format selected.");
      break;
  }
  return;
}

void MeshImpl::readAdcircMeshNetcdf() {
  int ncid;
  int ierr = nc_open(this->m_filename.c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR)
    adcircmodules_throw_exception("Could not open mesh file as netCDF");

  int dimid_node, dimid_elem, dimid_nvertex;
  ierr += nc_inq_dimid(ncid, "node", &dimid_node);
  ierr += nc_inq_dimid(ncid, "nele", &dimid_elem);
  ierr += nc_inq_dimid(ncid, "nvertex", &dimid_nvertex);
  if (ierr != 0)
    adcircmodules_throw_exception("Could not read the mesh dimensions");

  size_t n_max_vertex;
  size_t nn = this->numNodes();
  size_t ne = this->numElements();
  ierr += nc_inq_dimlen(ncid, dimid_node, &nn);
  ierr += nc_inq_dimlen(ncid, dimid_elem, &ne);
  ierr += nc_inq_dimlen(ncid, dimid_nvertex, &n_max_vertex);
  if (ierr != 0) adcircmodules_throw_exception("Could not read the mesh sizes");

  int varid_x, varid_y, varid_z, varid_elem;
  ierr += nc_inq_varid(ncid, "x", &varid_x);
  ierr += nc_inq_varid(ncid, "y", &varid_y);
  ierr += nc_inq_varid(ncid, "depth", &varid_z);
  ierr += nc_inq_varid(ncid, "element", &varid_elem);
  if (ierr != 0) adcircmodules_throw_exception("Could not read the varids");

  double *x = new double[nn];
  double *y = new double[nn];
  double *z = new double[nn];

  ierr += nc_get_var_double(ncid, varid_x, x);
  ierr += nc_get_var_double(ncid, varid_y, y);
  ierr += nc_get_var_double(ncid, varid_z, z);
  if (ierr != 0) {
    delete[] x;
    delete[] y;
    delete[] z;
    adcircmodules_throw_exception("Could not read nodal data");
  }

  this->m_nodes.reserve(nn);
  for (size_t i = 0; i < nn; ++i) {
    this->m_nodes.push_back(Node(i + 1, x[i], y[i], z[i]));
  }

  this->m_nodeOrderingLogical = true;
  this->m_elementOrderingLogical = true;

  delete[] x;
  delete[] y;
  delete[] z;

  int *n1 = new int[ne];
  int *n2 = new int[ne];
  int *n3 = new int[ne];
  int *n4 = new int[ne];
  size_t start[2], count[2];
  start[0] = 0;
  start[1] = 0;
  count[0] = ne;
  count[1] = 1;

  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n1);
  start[1] = 1;
  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n2);
  start[1] = 2;
  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n3);
  if (n_max_vertex == 4) {
    start[1] = 3;
    ierr += nc_get_vara_int(ncid, varid_elem, start, count, n4);
  } else {
    for (size_t i = 0; i < ne; ++i) {
      n4[i] = NC_FILL_INT;
    }
  }

  if (ierr != 0) {
    delete[] n1;
    delete[] n2;
    delete[] n3;
    delete[] n4;
    adcircmodules_throw_exception("Could not read the element data");
  }

  this->m_elements.reserve(ne);
  for (size_t i = 0; i < ne; ++i) {
    if (n4[i] == NC_FILL_INT) {
      this->m_elements.push_back(Element(i + 1, &this->m_nodes[n1[i] - 1],
                                         &this->m_nodes[n2[i] - 1],
                                         &this->m_nodes[n3[i] - 1]));
    } else {
      this->m_elements.push_back(
          Element(i + 1, &this->m_nodes[n1[i] - 1], &this->m_nodes[n2[i] - 1],
                  &this->m_nodes[n3[i] - 1], &this->m_nodes[n4[i] - 1]));
    }
  }

  delete[] n1;
  delete[] n2;
  delete[] n3;
  delete[] n4;

  return;
}

/**
 * @brief Reads an ASCII formatted ADCIRC mesh.
 *
 * Note this will be extended in the future to netCDF formatted meshes
 */
void MeshImpl::readAdcircMeshAscii() {
  std::fstream fid(this->filename());

  this->readAdcircMeshHeader(fid);
  this->readAdcircNodes(fid);
  this->readAdcircElements(fid);
  this->readAdcircOpenBoundaries(fid);
  this->readAdcircLandBoundaries(fid);

  fid.close();

  return;
}

/**
 * @brief Reads an Aquaveo generic mesh format (2dm)
 *
 */
void MeshImpl::read2dmMesh() {
  std::vector<std::string> nodes, elements;
  this->read2dmData(nodes, elements);
  this->read2dmNodes(nodes);
  this->read2dmElements(elements);
  return;
}

/**
 * @brief Read a DFlow-FM unstructured mesh file
 */
void MeshImpl::readDflowMesh() {
  int ncid;
  int ierr = nc_open(this->m_filename.c_str(), NC_NOWRITE, &ncid);

  this->m_meshHeaderString = "DFlowFM-NetNC";

  int dimid_nnode, dimid_nelem, dimid_nmaxnode;
  ierr += nc_inq_dimid(ncid, "nNetNode", &dimid_nnode);
  ierr += nc_inq_dimid(ncid, "nNetElem", &dimid_nelem);
  ierr += nc_inq_dimid(ncid, "nNetElemMaxNode", &dimid_nmaxnode);
  if (ierr != 0) {
    nc_close(ncid);
    adcircmodules_throw_exception("Error opening DFlow mesh file");
  }

  size_t nmaxnode, nn, ne;
  ierr += nc_inq_dimlen(ncid, dimid_nnode, &nn);
  ierr += nc_inq_dimlen(ncid, dimid_nelem, &ne);
  ierr += nc_inq_dimlen(ncid, dimid_nmaxnode, &nmaxnode);
  if (ierr != 0) {
    nc_close(ncid);
    adcircmodules_throw_exception("Error reading mesh dimensions");
  }
  if (nmaxnode < 3 || nmaxnode > 4) {
    adcircmodules_throw_exception("Mesh must only contain triangles and quads");
  }

  int varid_x, varid_y, varid_z, varid_elem;
  ierr += nc_inq_varid(ncid, "NetNode_x", &varid_x);
  ierr += nc_inq_varid(ncid, "NetNode_y", &varid_y);
  ierr += nc_inq_varid(ncid, "NetNode_z", &varid_z);
  ierr += nc_inq_varid(ncid, "NetElemNode", &varid_elem);
  if (ierr != 0) {
    adcircmodules_throw_exception("Error reading variable id's");
  }

  int isFill, elemFillValue;
  ierr += nc_inq_var_fill(ncid, varid_elem, &isFill, &elemFillValue);
  if (ierr != 0) {
    adcircmodules_throw_exception("Error reading element fill values");
  }

  if (isFill == 1) elemFillValue = NC_FILL_INT;

  double *xcoor = new double[nn];
  double *ycoor = new double[nn];
  double *zcoor = new double[nn];
  int *elem = new int[ne * nmaxnode];

  ierr += nc_get_var_double(ncid, varid_x, xcoor);
  ierr += nc_get_var_double(ncid, varid_y, ycoor);
  ierr += nc_get_var_double(ncid, varid_z, zcoor);
  ierr += nc_get_var_int(ncid, varid_elem, elem);
  nc_close(ncid);

  if (ierr != 0) {
    delete[] xcoor;
    delete[] ycoor;
    delete[] zcoor;
    delete[] elem;
    adcircmodules_throw_exception("Error reading arrays from netcdf file.");
  }

  this->m_nodes.resize(nn);
  for (size_t i = 0; i < nn; ++i) {
    this->m_nodes[i] = Node(i + 1, xcoor[i], ycoor[i], zcoor[i]);
  }

  delete[] xcoor;
  delete[] ycoor;
  delete[] zcoor;

  for (size_t i = 0; i < ne; ++i) {
    std::vector<size_t> n(nmaxnode);
    size_t nfill = 0;
    for (size_t j = 0; j < nmaxnode; ++j) {
      n[j] = elem[i * nmaxnode + j];
      if (n[j] == elemFillValue || n[j] == NC_FILL_INT || n[j] == NC_FILL_INT64)
        nfill++;
    }

    size_t nnodeelem = nmaxnode - nfill;
    if (nnodeelem != 3 && nnodeelem != 4) {
      delete[] elem;
      adcircmodules_throw_exception("Invalid element type detected");
    }

    this->m_elements.resize(ne);
    if (nnodeelem == 3) {
      Element etemp(i + 1, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
                    &this->m_nodes[n[2] - 1]);
      this->m_elements[i] = etemp;
    } else {
      Element etemp(i + 1, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
                    &this->m_nodes[n[2] - 1], &this->m_nodes[n[3] - 1]);
      this->m_elements[i] = etemp;
    }
    this->m_elements[i].sortVerticiesAboutCenter();
  }

  delete[] elem;

  return;
}

/**
 * @brief Reads the data from the 2dm file into the appropriate vector
 * @param[out] nodes node cards vector
 * @param[out] elements element cards vector
 */
void MeshImpl::read2dmData(std::vector<std::string> &nodes,
                           std::vector<std::string> &elements) {
  std::fstream fid(this->filename());

  std::string junk;
  std::getline(fid, junk);
  std::getline(fid, junk);
  junk = junk.substr(8, junk.size());
  junk.erase(std::remove(junk.begin(), junk.end(), '\"'), junk.end());
  this->m_meshHeaderString = StringConversion::sanitizeString(junk);
  if (this->m_meshHeaderString == std::string()) {
    this->m_meshHeaderString = "Mesh";
  }

  std::string templine;
  while (std::getline(fid, templine)) {
    std::vector<std::string> templist;
    FileIO::Generic::splitString(templine, templist);
    if (templist[0] == "ND") {
      nodes.push_back(templine);
    } else if (templist[0] == "E4Q" || templist[0] == "E3T") {
      elements.push_back(templine);
    }
  }
  fid.close();
  return;
}

/**
 * @brief Builds a lookup table in the case that the nodes are not numbered in
 * order
 */
void MeshImpl::buildNodeLookupTable() {
  this->m_nodeLookup.reserve(this->numNodes());
  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->m_nodeLookup[this->m_nodes[i].id()] = i;
  }
}

/**
 * @brief Parses the node data into data structures
 * @param nodes vector of node data from 2dm file
 */
void MeshImpl::read2dmNodes(std::vector<std::string> &nodes) {
  this->m_nodes.reserve(nodes.size());
  this->m_nodeOrderingLogical = true;
  for (auto &n : nodes) {
    size_t id;
    double x, y, z;
    FileIO::SMSIO::splitString2dmNodeFormat(n, id, x, y, z);
    this->m_nodes.push_back(Node(id, x, y, z));
    if (this->m_nodes.size() != id) {
      this->m_nodeOrderingLogical = false;
    }
  }

  if (!this->m_nodeOrderingLogical) {
    this->buildNodeLookupTable();
  }
}

/**
 * @brief Parses the element data into data structures
 * @param elements vector of element data from the 2dm file
 */
void MeshImpl::read2dmElements(std::vector<std::string> &elements) {
  this->m_elements.reserve(elements.size());
  for (auto &e : elements) {
    size_t id;
    std::vector<size_t> n;
    if (FileIO::SMSIO::splitString2dmElementFormat(e, id, n)) {
      if (n.size() == 3) {
        if (this->m_nodeOrderingLogical) {
          this->m_elements.push_back(Element(id, &this->m_nodes[n[0] - 1],
                                             &this->m_nodes[n[1] - 1],
                                             &this->m_nodes[n[2] - 1]));
        } else {
          this->m_elements.push_back(
              Element(id, &this->m_nodes[this->m_nodeLookup[n[0]]],
                      &this->m_nodes[this->m_nodeLookup[n[1]]],
                      &this->m_nodes[this->m_nodeLookup[n[2]]]));
        }
      } else if (n.size() == 4) {
        if (this->m_nodeOrderingLogical) {
          this->m_elements.push_back(
              Element(id, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
                      &this->m_nodes[n[2] - 1], &this->m_nodes[n[3] - 1]));
        } else {
          this->m_elements.push_back(
              Element(id, &this->m_nodes[this->m_nodeLookup[n[0]]],
                      &this->m_nodes[this->m_nodeLookup[n[1]]],
                      &this->m_nodes[this->m_nodeLookup[n[2]]],
                      &this->m_nodes[this->m_nodeLookup[n[3]]]));
        }
      } else {
        adcircmodules_throw_exception("Too many nodes (" +
                                      std::to_string(n.size()) +
                                      ") detected in element.");
      }
    } else {
      adcircmodules_throw_exception("Error reading 2dm element");
    }
  }
  return;
}

/**
 * @brief Determine the mesh format based upon file extension
 * @return MeshFormat enum
 */
Adcirc::Geometry::MeshFormat MeshImpl::getMeshFormat(
    const std::string &filename) {
  std::string extension = FileIO::Generic::getFileExtension(filename);
  if (extension == ".14" || extension == ".grd") {
    return MeshAdcirc;
  } else if (extension == ".2dm") {
    return Mesh2DM;
  } else if (filename.find("_net.nc") != filename.npos) {
    return MeshDFlow;
  } else if (extension == ".nc") {
    return MeshAdcircNetcdf;
  } else {
    return MeshUnknown;
  }
}

/**
 * @brief Reads the mesh title line and node/element dimensional data
 * @param fid std::fstream reference for the currently opened mesh
 */
void MeshImpl::readAdcircMeshHeader(std::fstream &fid) {
  bool ok;
  size_t tempInt;
  std::string tempLine;
  std::vector<std::string> tempList;

  //...Read mesh header
  std::getline(fid, tempLine);

  //...Set the mesh file header
  this->setMeshHeaderString(StringConversion::sanitizeString(tempLine));

  //..Get the size of the mesh
  std::getline(fid, tempLine);
  FileIO::Generic::splitString(tempLine, tempList);
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
void MeshImpl::readAdcircNodes(std::fstream &fid) {
  this->m_nodes.resize(this->numNodes());

  size_t i = 0;
  for (auto &n : this->m_nodes) {
    size_t id;
    double x, y, z;
    std::string tempLine;
    std::getline(fid, tempLine);
    if (!FileIO::AdcircIO::splitStringNodeFormat(tempLine, id, x, y, z)) {
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
    this->buildNodeLookupTable();
  }

  return;
}

/**
 * @brief Reads the elements section of the ASCII mesh
 * @param fid std::fstream reference for the currently opened mesh
 */
void MeshImpl::readAdcircElements(std::fstream &fid) {
  size_t id;
  std::string tempLine;

  this->m_elements.resize(this->numElements());

  if (this->m_nodeOrderingLogical) {
    for (auto &e : this->m_elements) {
      std::getline(fid, tempLine);
      std::vector<size_t> n;
      n.reserve(4);
      if (!FileIO::AdcircIO::splitStringElemFormat(tempLine, id, n)) {
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
      if (!FileIO::AdcircIO::splitStringElemFormat(tempLine, id, n)) {
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
    this->m_elementLookup.reserve(this->numElements());
    for (size_t i = 0; i < this->numElements(); ++i) {
      this->m_elementLookup[this->m_elements[i].id()] = i;
    }
  }

  return;
}

/**
 * @brief Reads the open boundaries section of the ASCII formatted mesh file
 * @param fid std::fstream reference for the currently opened mesh
 */
void MeshImpl::readAdcircOpenBoundaries(std::fstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;

  std::getline(fid, tempLine);
  FileIO::Generic::splitString(tempLine, tempList);

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
    FileIO::Generic::splitString(tempLine, tempList);

    size_t length = StringConversion::stringToSizet(tempList[0], ok);
    if (!ok) {
      fid.close();
      adcircmodules_throw_exception("Error reading boundaries");
    }

    b.setBoundary(-1, length);

    for (size_t j = 0; j < b.length(); ++j) {
      std::getline(fid, tempLine);
      size_t nid;
      if (!FileIO::AdcircIO::splitStringBoundary0Format(tempLine, nid)) {
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
void MeshImpl::readAdcircLandBoundaries(std::fstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;

  size_t n1, n2;
  double supercritical, subcritical, crest, pipeheight, pipediam, pipecoef;
  bool ok;

  std::getline(fid, tempLine);

  FileIO::Generic::splitString(tempLine, tempList);

  this->setNumLandBoundaries(StringConversion::stringToSizet(tempList[0], ok));
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading boundaries");
  }
  this->m_landBoundaries.resize(this->numLandBoundaries());

  std::getline(fid, tempLine);

  for (auto &b : this->m_landBoundaries) {
    std::getline(fid, tempLine);
    FileIO::Generic::splitString(tempLine, tempList);

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
        if (!FileIO::AdcircIO::splitStringBoundary23Format(tempLine, n1, crest,
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
        if (!FileIO::AdcircIO::splitStringBoundary24Format(
                tempLine, n1, n2, crest, subcritical, supercritical)) {
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
        if (!FileIO::AdcircIO::splitStringBoundary25Format(
                tempLine, n1, n2, crest, subcritical, supercritical, pipeheight,
                pipecoef, pipediam)) {
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
        if (!FileIO::AdcircIO::splitStringBoundary0Format(tempLine, n1)) {
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
Kdtree *MeshImpl::elementalSearchTree() const {
  return m_elementalSearchTree.get();
}

/**
 * @brief Returns a refrence to the nodal search kd-tree
 * @return kd-tree object with mesh nodes as serch locations
 */
Kdtree *MeshImpl::nodalSearchTree() const { return m_nodalSearchTree.get(); }

/**
 * @brief Returns the number of land boundary nodes in the mesh
 * @return Number of nodes that fall on a land boundary
 */
size_t MeshImpl::totalLandBoundaryNodes() {
  size_t total = 0;
  for (auto &b : this->m_landBoundaries) {
    total += b.length();
  }
  return total;
}

/**
 * @brief Sets the z-values of the mesh to the values found within a vector
 * @param z values that will be set to the mesh nodes z attribute
 */
void MeshImpl::setZ(std::vector<double> &z) {
  assert(z.size() == this->numNodes());
  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->m_nodes[i].setZ(z[i]);
  }
  return;
}

/**
 * @brief Returns the number of open boundary nodes in the mesh
 * @return Number of open boundary nodes
 */
size_t MeshImpl::totalOpenBoundaryNodes() {
  size_t total = 0;
  for (auto &b : this->m_openBoundaries) {
    total += b.length();
  }
  return total;
}

/**
 * @brief Returns a pointer to the requested node in the internal node vector
 * @param index location of the node in the vector
 * @return Node pointer
 */
Node *MeshImpl::node(size_t index) {
  if (index < this->numNodes()) {
    return &this->m_nodes[index];
  } else {
    adcircmodules_throw_exception("Mesh: Node index " + std::to_string(index) +
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
Element *MeshImpl::element(size_t index) {
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
Node *MeshImpl::nodeById(size_t id) {
  if (this->m_nodeOrderingLogical) {
    if (id > 0 && id <= this->numNodes()) {
      return &this->m_nodes[id - 1];
    } else {
      adcircmodules_throw_exception("Mesh: Node id " + std::to_string(id) +
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
Element *MeshImpl::elementById(size_t id) {
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
Boundary *MeshImpl::openBoundary(size_t index) {
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
Boundary *MeshImpl::landBoundary(size_t index) {
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
void MeshImpl::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
  return;
}

/**
 * @brief Returns the EPSG code for the current mesh projection
 * @return EPSG code
 */
int MeshImpl::projection() { return this->m_epsg; }

/**
 * @brief Returns true if the mesh is in a geographic projection
 * @return boolean value for mesh projection type
 */
bool MeshImpl::isLatLon() { return this->m_isLatLon; }

/**
 * @brief Reprojects a mesh into the specified projection
 * @param epsg EPSG coordinate system to convert the mesh into
 */
void MeshImpl::reproject(int epsg) {
  Ezproj proj;
  bool isLatLon;
  std::vector<Point> inPoint, outPoint;
  inPoint.reserve(this->numNodes());
  outPoint.resize(this->numNodes());

  for (const auto &n : this->m_nodes) {
    inPoint.push_back(Point(n.x(), n.y()));
  }

  int ierr =
      proj.transform(this->projection(), epsg, inPoint, outPoint, isLatLon);

  if (ierr != Ezproj::NoError) {
    adcircmodules_throw_exception("Mesh: Proj4 library error");
  }

  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->node(i)->setX(outPoint[i].first);
    this->node(i)->setY(outPoint[i].second);
  }

  this->defineProjection(epsg, isLatLon);

  return;
}

/**
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void MeshImpl::toNodeShapefile(const std::string &outputFile) {
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
 * @brief Generates a table containing the node-to-node links that form elements
 * @return vector of unique node pairs
 */
std::vector<std::pair<Node *, Node *>> MeshImpl::generateLinkTable() {
  std::vector<std::pair<Node *, Node *>> legs;
  legs.reserve(this->numElements() * 4);
  for (auto &e : this->m_elements) {
    Element e_s = e;
    e_s.sortVerticiesAboutCenter();
    for (size_t j = 0; j < e_s.n(); ++j) {
      std::pair<Node *, Node *> p1 = e_s.elementLeg(j);
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
  return legs;
}

/**
 * @brief Writes the mesh connectivity into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void MeshImpl::toConnectivityShapefile(const std::string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_ARC);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());

  DBFAddField(dbfid, "node1", FTInteger, 16, 0);
  DBFAddField(dbfid, "node2", FTInteger, 16, 0);
  DBFAddField(dbfid, "znode1", FTDouble, 16, 4);
  DBFAddField(dbfid, "znode2", FTDouble, 16, 4);

  std::vector<std::pair<Node *, Node *>> legs = this->generateLinkTable();

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
void MeshImpl::toElementShapefile(const std::string &outputFile) {
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
    double longitude[4], latitude[4];
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
      nodez[3] = adcircmodules_default_value<double>();
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
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return;
}

void MeshImpl::toBoundaryShapefile(const std::string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_POINTZ);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());
  DBFAddField(dbfid, "bndId", FTInteger, 16, 0);
  DBFAddField(dbfid, "bndIdx", FTInteger, 16, 0);
  DBFAddField(dbfid, "bndCode", FTInteger, 16, 0);
  DBFAddField(dbfid, "node1", FTInteger, 16, 0);
  DBFAddField(dbfid, "node2", FTInteger, 16, 0);
  DBFAddField(dbfid, "bathy", FTDouble, 16, 4);
  DBFAddField(dbfid, "bc_elev", FTDouble, 16, 4);
  DBFAddField(dbfid, "supercritical", FTDouble, 16, 4);
  DBFAddField(dbfid, "subcritical", FTDouble, 16, 4);
  DBFAddField(dbfid, "pipediam", FTDouble, 16, 4);
  DBFAddField(dbfid, "pipeheight", FTDouble, 16, 4);
  DBFAddField(dbfid, "pipecoef", FTDouble, 16, 4);

  int idx = 0;
  int bcid = 0;
  for (auto &b : this->m_openBoundaries) {
    int code = b.boundaryCode();
    for (size_t i = 0; i < b.length(); ++i) {
      int node1 = static_cast<int>(b.node1(i)->id());
      int node2 = -9999;
      double lon = b.node1(i)->x();
      double lat = b.node1(i)->y();
      double bathy = -b.node1(i)->z();
      double elev = -9999.0;
      double sub = -9999.0;
      double sup = -9999.0;
      double pipeht = -9999.0;
      double pipecoef = -9999.0;
      double pipediam = -9999.0;
      SHPObject *shpobj =
          SHPCreateSimpleObject(SHPT_POINTZ, 1, &lon, &lat, &bathy);
      int shp_index = SHPWriteObject(shpid, -1, shpobj);
      SHPDestroyObject(shpobj);

      DBFWriteIntegerAttribute(dbfid, shp_index, 0, bcid);
      DBFWriteIntegerAttribute(dbfid, shp_index, 1, idx);
      DBFWriteIntegerAttribute(dbfid, shp_index, 2, code);
      DBFWriteIntegerAttribute(dbfid, shp_index, 3, node1);
      DBFWriteIntegerAttribute(dbfid, shp_index, 4, node2);
      DBFWriteDoubleAttribute(dbfid, shp_index, 5, bathy);
      DBFWriteDoubleAttribute(dbfid, shp_index, 6, elev);
      DBFWriteDoubleAttribute(dbfid, shp_index, 7, sup);
      DBFWriteDoubleAttribute(dbfid, shp_index, 8, sub);
      DBFWriteDoubleAttribute(dbfid, shp_index, 9, pipediam);
      DBFWriteDoubleAttribute(dbfid, shp_index, 10, pipeht);
      DBFWriteDoubleAttribute(dbfid, shp_index, 11, pipecoef);
      idx++;
    }
    bcid++;
  }

  for (auto &b : this->m_landBoundaries) {
    int code = b.boundaryCode();
    for (size_t i = 0; i < b.length(); ++i) {
      int node1 = static_cast<int>(b.node1(i)->id());
      int node2 = -9999;
      double lon = b.node1(i)->x();
      double lat = b.node1(i)->y();
      double bathy = -b.node1(i)->z();
      double elev = -9999.0;
      double sub = -9999.0;
      double sup = -9999.0;
      double pipeht = -9999.0;
      double pipecoef = -9999.0;
      double pipediam = -9999.0;
      if (b.isInternalWeir()) {
        lon = (lon + b.node2(i)->x()) / 2.0;
        lat = (lat + b.node2(i)->y()) / 2.0;
        bathy = (bathy + b.node2(i)->z()) / 2.0;
        node2 = static_cast<int>(b.node2(i)->id());
        elev = b.crestElevation(i);
        sup = b.supercriticalWeirCoefficient(i);
        sub = b.subcriticalWeirCoefficient(i);
      } else if (b.isExternalWeir()) {
        elev = b.crestElevation(i);
        sup = b.supercriticalWeirCoefficient(i);
      }
      if (b.isInternalWeirWithPipes()) {
        pipeht = b.pipeHeight(i);
        pipediam = b.pipeDiameter(i);
        pipecoef = b.pipeCoefficient(i);
      }
      SHPObject *shpobj =
          SHPCreateSimpleObject(SHPT_POINTZ, 1, &lon, &lat, &bathy);
      int shp_index = SHPWriteObject(shpid, -1, shpobj);
      SHPDestroyObject(shpobj);

      DBFWriteIntegerAttribute(dbfid, shp_index, 0, bcid);
      DBFWriteIntegerAttribute(dbfid, shp_index, 1, idx);
      DBFWriteIntegerAttribute(dbfid, shp_index, 2, code);
      DBFWriteIntegerAttribute(dbfid, shp_index, 3, node1);
      DBFWriteIntegerAttribute(dbfid, shp_index, 4, node2);
      DBFWriteDoubleAttribute(dbfid, shp_index, 5, bathy);
      DBFWriteDoubleAttribute(dbfid, shp_index, 6, elev);
      DBFWriteDoubleAttribute(dbfid, shp_index, 7, sup);
      DBFWriteDoubleAttribute(dbfid, shp_index, 8, sub);
      DBFWriteDoubleAttribute(dbfid, shp_index, 9, pipediam);
      DBFWriteDoubleAttribute(dbfid, shp_index, 10, pipeht);
      DBFWriteDoubleAttribute(dbfid, shp_index, 11, pipecoef);
      idx++;
    }
    bcid++;
  }

  DBFClose(dbfid);
  SHPClose(shpid);

  return;
}

/**
 * @brief Builds a kd-tree object with the mesh nodes as the search locations
 */
void MeshImpl::buildNodalSearchTree() {
  int ierr;
  std::vector<double> x, y;

  x.reserve(this->numNodes());
  y.reserve(this->numNodes());

  for (const auto &n : this->m_nodes) {
    x.push_back(n.x());
    y.push_back(n.y());
  }

  if (this->m_nodalSearchTree != nullptr) {
    if (this->m_nodalSearchTree->initialized()) {
      this->m_nodalSearchTree.reset(nullptr);
    }
  }

  ierr = this->m_nodalSearchTree->build(x, y);
  if (ierr != Kdtree::NoError) {
    adcircmodules_throw_exception("Mesh: KDTree2 library error");
  }

  return;
}

/**
 * @brief Builds a kd-tree object with the element centers as the search
 * locations
 */
void MeshImpl::buildElementalSearchTree() {
  std::vector<double> x, y;

  x.reserve(this->numElements());
  y.reserve(this->numElements());

  for (auto &e : this->m_elements) {
    double tempX = 0.0;
    double tempY = 0.0;
    e.getElementCenter(tempX, tempY);
    x.push_back(tempX);
    y.push_back(tempY);
  }

  if (this->m_elementalSearchTree != nullptr) {
    if (this->m_elementalSearchTree->initialized()) {
      this->m_elementalSearchTree.reset(nullptr);
    }
  }

  int ierr = this->m_elementalSearchTree->build(x, y);
  if (ierr != Kdtree::NoError) {
    adcircmodules_throw_exception("Mesh: KDTree2 library error");
  }

  return;
}

/**
 * @brief Deletes the nodal search tree
 */
void MeshImpl::deleteNodalSearchTree() {
  if (this->nodalSearchTreeInitialized()) {
    this->m_nodalSearchTree.reset();
  }
  return;
}

/**
 * @brief Deletes the elemental search tree
 */
void MeshImpl::deleteElementalSearchTree() {
  if (this->elementalSearchTreeInitialized()) {
    this->m_elementalSearchTree.reset();
  }
  return;
}

/**
 * @brief Returns a boolean value determining if the nodal search tree has
 * been initialized
 * @return true if the search tree is initialized
 */
bool MeshImpl::nodalSearchTreeInitialized() {
  return this->m_nodalSearchTree->initialized();
}

/**
 * @brief Returns a boolean value determining if the elemental search tree has
 * been initialized
 * @return true of the search tree is initialized
 */
bool MeshImpl::elementalSearchTreeInitialized() {
  return this->m_elementalSearchTree->initialized();
}

/**
 * @brief Resizes the vectors within the mesh
 * @param numNodes Number of nodes
 * @param numElements Number of elements
 * @param numOpenBoundaries Number of open boundaries
 * @param numLandBoundaries Number of land boundaries
 */
void MeshImpl::resizeMesh(size_t numNodes, size_t numElements,
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
void MeshImpl::addNode(size_t index, const Node &node) {
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
void MeshImpl::deleteNode(size_t index) {
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
void MeshImpl::addElement(size_t index, const Element &element) {
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
void MeshImpl::deleteElement(size_t index) {
  if (index < this->numElements()) {
    this->m_elements.erase(this->m_elements.begin() + index);
    this->setNumElements(this->m_elements.size());
  } else {
    adcircmodules_throw_exception("Mesh: Element index > number of nodes");
  }
  return;
}

void MeshImpl::addLandBoundary(size_t index, const Boundary &bnd) {
  if (index < this->numLandBoundaries()) {
    this->m_landBoundaries[index] = bnd;
  } else {
    adcircmodules_throw_exception(
        "Mesh: Land boundary index > number of boundaries");
  }
}

void MeshImpl::deleteLandBoundary(size_t index) {
  if (index < this->numLandBoundaries()) {
    this->m_landBoundaries.erase(this->m_landBoundaries.begin() + index);
  } else {
    adcircmodules_throw_exception(
        "Mesh: Land boundary index > number of boundaries");
  }
}

void MeshImpl::addOpenBoundary(size_t index, const Boundary &bnd) {
  if (index < this->numOpenBoundaries()) {
    this->m_openBoundaries[index] = bnd;
  } else {
    adcircmodules_throw_exception(
        "Mesh: Open boundary index > number of boundaries");
  }
}

void MeshImpl::deleteOpenBoundary(size_t index) {
  if (index < this->numOpenBoundaries()) {
    this->m_openBoundaries.erase(this->m_openBoundaries.begin() + index);
  } else {
    adcircmodules_throw_exception(
        "Mesh: Open boundary index > number of boundaries");
  }
}

/**
 * @brief Writes an Mesh object to disk in ASCII format
 * @param filename name of the output file to write
 * @param format format specified for the output file
 *
 * If no output specifier is supplied, format is guessed from file extension.
 */
void MeshImpl::write(const std::string &outputFile, MeshFormat format) {
  MeshFormat fmt;
  if (format == MeshUnknown) {
    fmt = this->getMeshFormat(outputFile);
  } else {
    fmt = format;
  }

  switch (fmt) {
    case MeshAdcirc:
      this->writeAdcircMesh(outputFile);
      break;
    case Mesh2DM:
      this->write2dmMesh(outputFile);
      break;
    case MeshDFlow:
      this->writeDflowMesh(outputFile);
      break;
    default:
      adcircmodules_throw_exception("No valid mesh format specified.");
      break;
  }
}

/**
 * @brief Writes an Mesh object to disk in ADCIRC ASCII format
 * @param filename name of the output file to write
 */
void MeshImpl::writeAdcircMesh(const std::string &filename) {
  std::ofstream outputFile;

  outputFile.open(filename);

  //...Write the header
  outputFile << this->meshHeaderString() << "\n";
  std::string tempString = boost::str(boost::format("%11i %11i") %
                                      this->numElements() % this->numNodes());
  outputFile << tempString << "\n";

  //...Write the mesh nodes
  for (auto n : this->m_nodes) {
    outputFile << n.toAdcircString(this->isLatLon()) << "\n";
  }

  //...Write the mesh elements
  for (auto &e : this->m_elements) {
    outputFile << e.toAdcircString() << "\n";
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
 * @brief Writes an Mesh object to disk in Aquaveo 2dm ASCII format
 * @param filename name of the output file to write
 */
void MeshImpl::write2dmMesh(const std::string &filename) {
  std::ofstream outputFile;
  outputFile.open(filename);

  //...Write the header
  outputFile << "MESH2D\n";
  outputFile << "MESHNAME \"" << this->m_meshHeaderString << "\"\n";

  //...Elements
  for (auto &e : this->m_elements) {
    outputFile << e.to2dmString() << "\n";
  }

  //...Nodes
  for (auto &n : this->m_nodes) {
    outputFile << n.to2dmString(this->isLatLon()) << "\n";
  }

  outputFile.close();
  return;
}

/**
 * @brief Get the maximum number of nodes per element
 * @return max nodes per element
 */
size_t MeshImpl::getMaxNodesPerElement() {
  size_t n = 0;
  for (auto &e : this->m_elements) {
    if (e.n() > n) n = e.n();
  }
  return n;
}

/**
 * @brief Writes the mesh to the DFlow-FM format
 * @param filename name of the output file (*_net.nc)
 */
void MeshImpl::writeDflowMesh(const std::string &filename) {
  std::vector<std::pair<Node *, Node *>> links = this->generateLinkTable();
  size_t nlinks = links.size();
  size_t maxelemnode = this->getMaxNodesPerElement();

  double *xarray = new double[this->numNodes()];
  double *yarray = new double[this->numNodes()];
  double *zarray = new double[this->numNodes()];
  int *linkArray = new int[nlinks * 2];
  int *linkTypeArray = new int[nlinks * 2];
  int *netElemNodearray = new int[this->numElements() * maxelemnode];

  for (size_t i = 0; i < this->numNodes(); ++i) {
    xarray[i] = this->node(i)->x();
    yarray[i] = this->node(i)->y();
    zarray[i] = this->node(i)->z();
  }

  size_t idx = 0;
  for (size_t i = 0; i < nlinks; ++i) {
    linkArray[idx] = links[i].first->id();
    idx++;
    linkArray[idx] = links[i].second->id();
    idx++;
    linkTypeArray[i] = 2;
  }

  idx = 0;
  for (size_t i = 0; i < this->numElements(); ++i) {
    if (this->element(i)->n() == 3) {
      netElemNodearray[idx] = this->element(i)->node(0)->id();
      idx++;
      netElemNodearray[idx] = this->element(i)->node(1)->id();
      idx++;
      netElemNodearray[idx] = this->element(i)->node(2)->id();
      idx++;
      if (maxelemnode == 4) {
        netElemNodearray[idx] = NC_FILL_INT;
        idx++;
      }
    } else {
      netElemNodearray[idx] = this->element(i)->node(0)->id();
      idx++;
      netElemNodearray[idx] = this->element(i)->node(1)->id();
      idx++;
      netElemNodearray[idx] = this->element(i)->node(2)->id();
      idx++;
      netElemNodearray[idx] = this->element(i)->node(3)->id();
      idx++;
    }
  }

  int ncid;
  int dimid_nnode, dimid_nlink, dimid_nelem, dimid_maxnode, dimid_nlinkpts;
  int ierr = nc_create(filename.c_str(), NC_CLASSIC_MODEL, &ncid);
  ierr = nc_def_dim(ncid, "nNetNode", this->numNodes(), &dimid_nnode);
  ierr = nc_def_dim(ncid, "nNetLink", nlinks, &dimid_nlink);
  ierr = nc_def_dim(ncid, "nNetElem", this->numElements(), &dimid_nelem);
  ierr = nc_def_dim(ncid, "nNetElemMaxNode", maxelemnode, &dimid_maxnode);
  ierr = nc_def_dim(ncid, "nNetLinkPts", 2, &dimid_nlinkpts);

  int *dim1d = new int[1];
  int *dim2d = new int[2];

  int varid_mesh2d, varid_netnodex, varid_netnodey, varid_netnodez;
  ierr = nc_def_var(ncid, "Mesh2D", NC_INT, 0, nullptr, &varid_mesh2d);
  dim1d[0] = dimid_nnode;
  ierr = nc_def_var(ncid, "NetNode_x", NC_DOUBLE, 1, dim1d, &varid_netnodex);
  ierr = nc_def_var(ncid, "NetNode_y", NC_DOUBLE, 1, dim1d, &varid_netnodey);
  ierr = nc_def_var(ncid, "NetNode_z", NC_DOUBLE, 1, dim1d, &varid_netnodez);

  dim1d[0] = dimid_nlink;
  dim2d[0] = dimid_nlink;
  dim2d[1] = dimid_nlinkpts;

  int varid_netlinktype, varid_netlink, varid_crs;
  ierr = nc_def_var(ncid, "NetLinkType", NC_INT, 1, dim1d, &varid_netlinktype);
  ierr = nc_def_var(ncid, "NetLink", NC_INT, 2, dim2d, &varid_netlink);
  ierr = nc_def_var(ncid, "crs", NC_INT, 0, nullptr, &varid_crs);

  dim2d[0] = dimid_nelem;
  dim2d[1] = dimid_maxnode;

  int varid_netelemnode;
  ierr = nc_def_var(ncid, "NetElemNode", NC_INT, 2, dim2d, &varid_netelemnode);

  //...Define attributes
  ierr = nc_put_att_text(ncid, varid_mesh2d, "cf_role", 13, "mesh_topology");
  dim1d[0] = 2;
  ierr = nc_put_att_int(ncid, varid_mesh2d, "topology_dimension", NC_INT, 1,
                        dim1d);
  ierr = nc_put_att_text(ncid, varid_mesh2d, "node_coordinates", 19,
                         "NetNode_x NetNode_y");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "node_dimension", 8, "nNetNode");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "face_node_connectivity", 11,
                         "NetElemNode");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "face_dimension", 8, "nNetElem");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "edge_node_connectivity", 7,
                         "NetLink");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "edge_dimension", 8, "nNetLink");

  if (this->isLatLon()) {
    ierr = nc_put_att_text(ncid, varid_netnodex, "axis", 5, "theta");
    ierr = nc_put_att_text(ncid, varid_netnodex, "long_name", 19,
                           "longitude of vertex");
    ierr = nc_put_att_text(ncid, varid_netnodex, "units", 12, "degrees_east");
    ierr =
        nc_put_att_text(ncid, varid_netnodex, "standard_name", 9, "longitude");

    ierr = nc_put_att_text(ncid, varid_netnodey, "axis", 3, "phi");
    ierr = nc_put_att_text(ncid, varid_netnodey, "long_name", 18,
                           "latitude of vertex");
    ierr = nc_put_att_text(ncid, varid_netnodey, "units", 13, "degrees_north");
    ierr =
        nc_put_att_text(ncid, varid_netnodey, "standard_name", 8, "latitude");
    dim1d[0] = 1;
    ierr = nc_put_att_int(ncid, NC_GLOBAL, "Spherical", NC_INT, 1, dim1d);

    dim1d[0] = this->m_epsg;
    ierr = nc_put_att_int(ncid, varid_crs, "EPSG", NC_INT, 1, dim1d);
  } else {
    ierr = nc_put_att_text(ncid, varid_netnodex, "axis", 1, "X");
    ierr = nc_put_att_text(ncid, varid_netnodex, "long_name", 32,
                           "x-coordinate in Cartesian system");
    ierr = nc_put_att_text(ncid, varid_netnodex, "units", 5, "metre");
    ierr = nc_put_att_text(ncid, varid_netnodex, "standard_name", 23,
                           "projection_x_coordinate");

    ierr = nc_put_att_text(ncid, varid_netnodey, "axis", 1, "Y");
    ierr = nc_put_att_text(ncid, varid_netnodey, "long_name", 32,
                           "x-coordinate in Cartesian system");
    ierr = nc_put_att_text(ncid, varid_netnodey, "units", 5, "metre");
    ierr = nc_put_att_text(ncid, varid_netnodey, "standard_name", 23,
                           "projection_y_coordinate");

    dim1d[0] = 0;
    ierr = nc_put_att_int(ncid, NC_GLOBAL, "Spherical", NC_INT, 1, dim1d);

    dim1d[0] = this->m_epsg;
    ierr = nc_put_att_int(ncid, varid_crs, "EPSG", NC_INT, 1, dim1d);
  }

  ierr = nc_put_att_text(ncid, varid_netnodez, "axis", 1, "Z");
  ierr = nc_put_att_text(ncid, varid_netnodez, "long_name", 32,
                         "z-coordinate in Cartesian system");
  ierr = nc_put_att_text(ncid, varid_netnodez, "units", 5, "metre");
  ierr = nc_put_att_text(ncid, varid_netnodez, "standard_name", 23,
                         "projection_z_coordinate");
  ierr = nc_put_att_text(ncid, varid_netnodez, "mesh", 6, "Mesh2D");
  ierr = nc_put_att_text(ncid, varid_netnodez, "location", 4, "node");

  dim1d[0] = 1;
  ierr = nc_put_att_int(ncid, varid_netlink, "start_index", NC_INT, 1, dim1d);
  ierr =
      nc_put_att_int(ncid, varid_netelemnode, "start_index", NC_INT, 1, dim1d);
  ierr = nc_put_att_text(ncid, NC_GLOBAL, "Conventions", 9, "UGRID-0.9");

  ierr = nc_enddef(ncid);

  ierr = nc_put_var_double(ncid, varid_netnodex, xarray);
  ierr = nc_put_var_double(ncid, varid_netnodey, yarray);
  ierr = nc_put_var_double(ncid, varid_netnodez, zarray);
  ierr = nc_put_var_int(ncid, varid_netlink, linkArray);
  ierr = nc_put_var_int(ncid, varid_netlinktype, linkTypeArray);
  ierr = nc_put_var_int(ncid, varid_netelemnode, netElemNodearray);
  ierr = nc_close(ncid);

  delete[] xarray;
  delete[] yarray;
  delete[] zarray;
  delete[] dim1d;
  delete[] dim2d;
  delete[] linkArray;
  delete[] linkTypeArray;
  delete[] netElemNodearray;

  return;
}

/**
 * @brief Allows the user to know if the code has determined that the node
 * ordering is logcical (i.e. sequential) or not
 * @return true if node ordering is sequential
 */
bool MeshImpl::nodeOrderingIsLogical() { return this->m_nodeOrderingLogical; }

/**
 * @brief Allows the user to know if the code has determined that the element
 * ordering is logcical (i.e. sequential) or not
 * @return true if element ordering is sequential
 */
bool MeshImpl::elementOrderingIsLogical() {
  return this->m_elementOrderingLogical;
}

/**
 * @brief Returns the position in the array by node id
 * @param id nodal id
 * @return array position
 */
size_t MeshImpl::nodeIndexById(size_t id) {
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
size_t MeshImpl::elementIndexById(size_t id) {
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
std::vector<double> MeshImpl::x() {
  std::vector<double> x;
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
std::vector<double> MeshImpl::y() {
  std::vector<double> y;
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
std::vector<double> MeshImpl::z() {
  std::vector<double> z;
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
std::vector<std::vector<double>> MeshImpl::xyz() {
  std::vector<std::vector<double>> xyz;
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
std::vector<std::vector<size_t>> MeshImpl::connectivity() {
  std::vector<std::vector<size_t>> conn;
  conn.reserve(this->numElements());
  for (auto &e : this->m_elements) {
    std::vector<size_t> v;
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
void MeshImpl::cpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    Point i(n.x(), n.y());
    Point o;
    Ezproj::cpp(lambda, phi, i, o);
    n.setX(o.first);
    n.setY(o.second);
  }
  return;
}

/**
 * @brief Convertes mesh back from the carte parallelogrammatique projection
 */
void MeshImpl::inverseCpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    Point i(n.x(), n.y());
    Point o;
    Ezproj::inverseCpp(lambda, phi, i, o);
    n.setX(o.first);
    n.setY(o.second);
  }
  return;
}

/**
 * @brief Finds the nearest mesh node to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest node index
 */
size_t MeshImpl::findNearestNode(double x, double y) {
  if (!this->nodalSearchTreeInitialized()) {
    this->buildNodalSearchTree();
  }
  return this->m_nodalSearchTree->findNearest(x, y);
}

/**
 * @param location location to search
 * @return nearest node index
 */
size_t MeshImpl::findNearestNode(Point &location) {
  return this->findNearestNode(location.first, location.second);
}

/**
 * @brief Finds the nearest mesh element to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest element index
 */
size_t MeshImpl::findNearestElement(double x, double y) {
  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }
  return this->findNearestElement(x, y);
}

/**
 * @param location location to search
 * @return nearest element index
 */
size_t MeshImpl::findNearestElement(Point &location) {
  return this->findNearestElement(location.first, location.second);
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param x location to search
 * @param y location to search
 * @return index of nearest element, large integer if not found
 */
size_t MeshImpl::findElement(double x, double y) {
  std::vector<double> wt;
  return this->findElement(x, y, wt);
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param x location to search
 * @param y location to search
 * @param weights interpolation weights for this x, y in the found element
 * @return index of nearest element, large integer if not found
 */
size_t MeshImpl::findElement(double x, double y, std::vector<double> &weights) {
  constexpr int searchDepth = 20;

  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }

  std::vector<size_t> indicies =
      this->elementalSearchTree()->findXNearest(x, y, searchDepth);
  size_t en = adcircmodules_default_value<size_t>();

  Point location(x, y);
  for (auto i : indicies) {
    bool found = this->element(i)->isInside(location);
    if (found) {
      en = i;
      break;
    }
  }

  if (en == adcircmodules_default_value<size_t>()) {
    std::fill(weights.begin(), weights.end(), 0.0);
    return en;
  }

  weights = this->element(en)->interpolationWeights(x, y);
  return en;
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param location location to search
 * @return index of nearest element, large integer if not found
 */
size_t MeshImpl::findElement(Point &location) {
  return this->findElement(location.first, location.second);
}

/**
 * @brief Computes average size of the element edges connected to each node
 * @return vector containing size at each node
 */
std::vector<double> MeshImpl::computeMeshSize() {
  if (!this->m_elementTable.initialized()) {
    this->m_elementTable.setMesh(this);
    this->m_elementTable.build();
  }

  std::vector<double> meshsize(this->numNodes());

  for (size_t i = 0; i < this->numNodes(); ++i) {
    std::vector<Element *> l = this->m_elementTable.elementList(this->node(i));
    double a = 0.0;
    for (size_t j = 0; j < l.size(); ++j) {
      a += l[j]->elementSize(false);
    }
    if (l.size() > 0)
      meshsize[i] = a / l.size();
    else
      meshsize[i] = 0.0;

    if (meshsize[i] < 0.0) {
      adcircmodules_throw_exception("Error computing mesh size table.");
    }
  }
  return meshsize;
}

/**
 * @brief comparison function for sorting the tuples of element edges
 * @param a first
 * @param b second
 * @return  true of a < b
 */
bool sortEdgeTuple(const std::tuple<Node *, Node *, Element *> &a,
                   const std::tuple<Node *, Node *, Element *> &b) {
  Node *n11, *n12;
  Node *n21, *n22;
  Element *e1, *e2;
  std::tie(n11, n12, e1) = a;
  std::tie(n21, n22, e2) = b;
  if (n11->id() < n21->id()) return true;
  if (n12->id() < n22->id()) return true;
  return false;
}

/**
 * @brief Determines if two tuples of element edges refer to the same edge
 * @param a first
 * @param b second
 * @return true of a == b
 */
bool edgeTupleEqual(const std::tuple<Node *, Node *, Element *> &a,
                    const std::tuple<Node *, Node *, Element *> &b) {
  Node *n11, *n12;
  Node *n21, *n22;
  Element *e1, *e2;
  std::tie(n11, n12, e1) = a;
  std::tie(n21, n22, e2) = b;
  if (n11->id() == n21->id() && n12->id() == n22->id()) return true;
  return false;
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
std::vector<std::vector<double>> MeshImpl::orthogonality() {
  std::vector<std::vector<double>> o;
  o.reserve(this->numElements() * 2);

  //...Build the node pairs with duplicates
  std::vector<std::tuple<Node *, Node *, Element *>> edg;
  edg.reserve(this->numElements() * 4);
  for (auto &e : this->m_elements) {
    e.sortVerticiesAboutCenter();
    for (size_t i = 0; i < e.n(); i++) {
      std::pair<Node *, Node *> p = e.elementLeg(i);
      Node *a = p.first;
      Node *b = p.second;
      if (a->id() < b->id()) {
        edg.push_back(std::tuple<Node *, Node *, Element *>(a, b, &e));
      } else {
        edg.push_back(std::tuple<Node *, Node *, Element *>(b, a, &e));
      }
    }
  }

  //...Sort the node pairs so we can iterate
  std::sort(edg.begin(), edg.end(), sortEdgeTuple);

  //...Iterate over the edges which now are back to back
  //   and calculate the orthogonality
  for (size_t i = 0; i < edg.size() - 1; ++i) {
    if (edgeTupleEqual(edg[i], edg[i + 1])) {
      Node *n11, *n12;
      Node *n21, *n22;
      Element *e1, *e2;
      std::tie(n11, n12, e1) = edg[i];
      std::tie(n21, n22, e2) = edg[i + 1];
      double xc1, xc2, yc1, yc2;
      e1->getElementCenter(xc1, yc1);
      e2->getElementCenter(xc2, yc2);
      double outx = (n11->x() + n12->x()) / 2.0;
      double outy = (n11->y() + n12->y()) / 2.0;
      double dx1 = n12->x() - n11->x();
      double dy1 = n12->y() - n11->y();
      double dx2 = xc2 - xc1;
      double dy2 = yc2 - yc1;
      double r1 = dx1 * dx1 + dy1 * dy1;
      double r2 = dx2 * dx2 + dy2 * dy2;
      double ortho = (dx1 * dx2 + dy1 * dy2) / std::sqrt(r1 * r2);
      std::vector<double> v = {outx, outy,
                               std::abs(std::max(std::min(ortho, 1.0), -1.0))};
      o.push_back(v);
    }
  }

  return o;
}

/**
 * @brief Builds a list of elements connected to each node
 */
void MeshImpl::buildElementTable() {
  this->m_elementTable.setMesh(this);
  this->m_elementTable.build();
  return;
}

/**
 * @brief Returns the number of elements surrounding a specified node
 * @param n address of node
 * @return number of elements
 */
size_t MeshImpl::numElementsAroundNode(Adcirc::Geometry::Node *n) {
  return this->m_elementTable.numElementsAroundNode(n);
}

/**
 * @brief Returns the number of elements surrounding a specified node
 * @param n index of node
 * @return number of elements
 */
size_t MeshImpl::numElementsAroundNode(size_t nodeIndex) {
  return this->m_elementTable.numElementsAroundNode(nodeIndex);
}

/**
 * @brief Returns the nth element in the list of elements surrounding a node
 * @param n node pointer
 * @param listIndex index of the element to return
 * @return element pointer
 */
Adcirc::Geometry::Element *MeshImpl::elementTable(Adcirc::Geometry::Node *n,
                                                  size_t listIndex) {
  return this->m_elementTable.elementTable(n, listIndex);
}

/**
 * @brief Returns the nth element in the list of elements surrounding a node
 * @param n node index
 * @param listIndex index of the element to return
 * @return element pointer
 */
Adcirc::Geometry::Element *MeshImpl::elementTable(size_t nodeIndex,
                                                  size_t listIndex) {
  return this->m_elementTable.elementTable(nodeIndex, listIndex);
}

/**
 * @brief Returns a vector of pointers to the elements surrouding a node
 * @param n node pointer
 * @return vector of element references
 */
std::vector<Adcirc::Geometry::Element *> MeshImpl::elementsAroundNode(
    Adcirc::Geometry::Node *n) {
  return this->m_elementTable.elementList(n);
}

std::vector<Adcirc::Geometry::Node *> MeshImpl::boundaryNodes() {
  std::vector<std::pair<Node *, Node *>> links = this->generateLinkTable();
  std::vector<size_t> count(links.size());
  std::fill(count.begin(), count.end(), 0);
  adcmap<std::pair<Node *, Node *>, size_t> lookup;

  lookup.reserve(links.size());
  for (size_t i = 0; i < links.size(); ++i) {
    lookup[links[i]] = i;
  }

  for (auto &e : this->m_elements) {
    for (size_t j = 0; j < e.n(); ++j) {
      std::pair<Node *, Node *> p1 = e.elementLeg(j);
      std::pair<Node *, Node *> p2;
      if (p1.first->id() > p1.second->id()) {
        p2 = std::pair<Node *, Node *>(p1.second, p1.first);
      } else {
        p2 = std::move(p1);
      }

      count[lookup[p2]] += static_cast<size_t>(1);
    }
  }

  std::vector<Adcirc::Geometry::Node *> bdyVec;
  bdyVec.reserve(count.size() * 2);
  for (size_t i = 0; i < links.size(); ++i) {
    if (count[i] == 1) {
      bdyVec.push_back(links[i].first);
      bdyVec.push_back(links[i].second);
    }
  }

  std::set<Node *> bnd(bdyVec.begin(), bdyVec.end());
  bdyVec.clear();
  bdyVec.assign(bnd.begin(), bnd.end());
  bnd.clear();

  return bdyVec;
}

std::string MeshImpl::hash(bool force) {
  if (this->m_hash == nullptr || force) this->generateHash(force);
  return std::string(this->m_hash);
}

void MeshImpl::generateHash(bool force) {
  Hash h(this->m_hashType);
  for (auto &n : this->m_nodes) {
    h.addData(n.hash(this->m_hashType, force));
  }

  for (auto &e : this->m_elements) {
    h.addData(e.hash(this->m_hashType, force));
  }

  for (auto &b : this->m_openBoundaries) {
    h.addData(b.hash(this->m_hashType, force));
  }

  for (auto &b : this->m_landBoundaries) {
    h.addData(b.hash(this->m_hashType, force));
  }
  this->m_hash = h.getHash();
}

HashType MeshImpl::hashType() const { return this->m_hashType; }

void MeshImpl::setHashType(const HashType &hashType) {
  this->m_hashType = hashType;
}
