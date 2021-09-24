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
#include "MeshPrivate.h"

#include <algorithm>
#include <set>
#include <string>
#include <tuple>
#include <utility>

#include "AdcHash.h"
#include "DefaultValues.h"
#include "ElementTable.h"
#include "FPCompare.h"
#include "FileIO.h"
#include "FileTypes.h"
#include "KDTree.h"
#include "Logging.h"
#include "Mesh.h"
#include "Projection.h"
#include "StringConversion.h"
#include "boost/format.hpp"
#include "boost/functional/hash.hpp"
#include "netcdf.h"
#include "shapefil.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef USE_GDAL
#include "cpl_conv.h"
#include "cpl_error.h"
#include "gdal_priv.h"
#include "ogr_spatialref.h"
#endif

using namespace Adcirc::Private;
using namespace Adcirc::Geometry;

Adcirc::Geometry::Mesh::~Mesh() = default;

/**
 * @brief Default Constructor
 */
MeshPrivate::MeshPrivate()
    : m_hashType(Adcirc::Cryptography::AdcircDefaultHash),
      m_filename("none"),
      m_epsg(-1),
      m_topology(std::make_unique<Adcirc::Geometry::Topology>(this)) {
  this->_init();
}

/**
 * @brief Default constructor with filename parameter
 * @param filename name of the mesh to read
 */
MeshPrivate::MeshPrivate(std::string filename)
    : m_hashType(Adcirc::Cryptography::AdcircDefaultHash),
      m_filename(std::move(filename)),
      m_epsg(-1),
      m_topology(std::make_unique<Adcirc::Geometry::Topology>(this)) {
  this->_init();
}

MeshPrivate &MeshPrivate::operator=(const MeshPrivate &m) {
  MeshPrivate::meshCopier(this, &m);
  return *this;
}

std::unique_ptr<MeshPrivate> MeshPrivate::clone() const {
  std::unique_ptr<MeshPrivate> m;
  MeshPrivate::meshCopier(m.get(), this);
  return m;
}

MeshPrivate::MeshPrivate(const MeshPrivate &m) {
  MeshPrivate::meshCopier(this, &m);
}

void MeshPrivate::meshCopier(MeshPrivate *a, const MeshPrivate *b) {
  a->setMeshHeaderString(b->meshHeaderString());
  a->resizeMesh(b->numNodes(), b->numElements(), b->numOpenBoundaries(),
                b->numLandBoundaries());
  a->setHashType(b->hashType());

  for (size_t i = 0; i < b->numNodes(); ++i) {
    a->addNode(i, b->nodeC(i));
  }
  for (size_t i = 0; i < b->numElements(); ++i) {
    a->addElement(i, b->elementC(i));
  }
  for (size_t i = 0; i < b->numOpenBoundaries(); ++i) {
    a->addOpenBoundary(i, b->openBoundaryC(i));
  }
  for (size_t i = 0; i < b->numLandBoundaries(); ++i) {
    a->addLandBoundary(i, b->landBoundaryC(i));
  }
}

/**
 * @brief Initialization routine called by all constructors
 */
void MeshPrivate::_init() {
  if (this->m_epsg == -1) this->defineProjection(4326, true);
  this->m_nodalSearchTree = nullptr;
  this->m_elementalSearchTree = nullptr;
  this->m_nodeOrderingLogical = true;
  this->m_elementOrderingLogical = true;
  this->m_hash.reset(nullptr);
  this->m_elementalSearchTree = std::make_unique<Kdtree>();
  this->m_nodalSearchTree = std::make_unique<Kdtree>();
}

/**
 * @brief Mesh::~Mesh Destructor
 */
MeshPrivate::~MeshPrivate() = default;

/**
 * @brief Filename of the mesh to be read
 * @return Return the name of the mesh to be read
 */
std::string MeshPrivate::filename() const { return this->m_filename; }

/**
 * @brief Sets the name of the mesh to be read
 * @param filename Name of the mesh
 */
void MeshPrivate::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

/**
 * @brief Returns the mesh header from the processed mesh
 * @return mesh header
 */
std::string MeshPrivate::meshHeaderString() const {
  return this->m_meshHeaderString;
}

/**
 * @brief Sets the header for the mesh
 * @param meshHeaderString header
 */
void MeshPrivate::setMeshHeaderString(const std::string &meshHeaderString) {
  this->m_meshHeaderString = meshHeaderString;
}

/**
 * @brief Returns the number of nodes currently in the mesh
 * @return number of nodes
 */
size_t MeshPrivate::numNodes() const { return this->m_nodes.size(); }

/**
 * @brief Sets the number of nodes in the mesh
 * @param numNodes number of nodes
 */
void MeshPrivate::setNumNodes(size_t numNodes) {
  this->m_nodes.resize(numNodes);
}

/**
 * @brief Returns the number of elements in the mesh
 * @return number of elements
 */
size_t MeshPrivate::numElements() const { return this->m_elements.size(); }

/**
 * @brief Sets the number of elements in the mesh
 * @param numElements Number of elements
 */
void MeshPrivate::setNumElements(size_t numElements) {
  this->m_elements.resize(numElements);
}

/**
 * @brief Returns the number of open boundaries in the model
 * @return number of open boundaries
 */
size_t MeshPrivate::numOpenBoundaries() const {
  return this->m_openBoundaries.size();
}

/**
 * @brief Sets the number of open boundaries in the model
 * @param numOpenBoundaries Number of open boundaries
 */
void MeshPrivate::setNumOpenBoundaries(size_t numOpenBoundaries) {
  this->m_openBoundaries.resize(numOpenBoundaries);
}

/**
 * @brief Returns the number of land boundaries in the model
 * @return number of land boundaries
 */
size_t MeshPrivate::numLandBoundaries() const {
  return this->m_landBoundaries.size();
}

/**
 * @brief Sets the number of land boundaries in the model
 * @param numLandBoundaries Number of land boundaries
 */
void MeshPrivate::setNumLandBoundaries(size_t numLandBoundaries) {
  this->m_landBoundaries.resize(numLandBoundaries);
}

/**
 * @brief Reads a specified mesh format
 * @param[optional] format MeshFormat enum describing the format of the mesh
 *
 * Reads the unstructured mesh into a mesh object. If no format is
 * specified, then it will be guessed from the file extension
 */
void MeshPrivate::read(MeshFormat format) {
  if (this->m_filename.empty()) {
    adcircmodules_throw_exception("No filename has been specified.");
  }
  if (!Adcirc::FileIO::Generic::fileExists(this->m_filename)) {
    adcircmodules_throw_exception("File does not exist.");
  }

  MeshFormat fmt;
  if (format == MeshUnknown) {
    fmt = Adcirc::Private::MeshPrivate::getMeshFormat(this->m_filename);
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
}

void MeshPrivate::readAdcircMeshNetcdf() {
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

  std::vector<double> x(nn);
  std::vector<double> y(nn);
  std::vector<double> z(nn);

  ierr += nc_get_var_double(ncid, varid_x, x.data());
  ierr += nc_get_var_double(ncid, varid_y, y.data());
  ierr += nc_get_var_double(ncid, varid_z, z.data());
  if (ierr != 0) {
    adcircmodules_throw_exception("Could not read nodal data");
  }

  this->m_nodes.reserve(nn);
  for (size_t i = 0; i < nn; ++i) {
    this->m_nodes.emplace_back(i + 1, x[i], y[i], z[i]);
  }

  this->m_nodeOrderingLogical = true;
  this->m_elementOrderingLogical = true;

  std::vector<int> n1(ne);
  std::vector<int> n2(ne);
  std::vector<int> n3(ne);
  std::vector<int> n4(ne);

  size_t start[] = {0, 0};
  size_t count[] = {ne, 1};

  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n1.data());
  start[1] = 1;
  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n2.data());
  start[1] = 2;
  ierr += nc_get_vara_int(ncid, varid_elem, start, count, n3.data());
  if (n_max_vertex == 4) {
    start[1] = 3;
    ierr += nc_get_vara_int(ncid, varid_elem, start, count, n4.data());
  } else {
    for (size_t i = 0; i < ne; ++i) {
      n4[i] = NC_FILL_INT;
    }
  }

  if (ierr != 0) {
    adcircmodules_throw_exception("Could not read the element data");
  }

  this->m_elements.reserve(ne);
  for (size_t i = 0; i < ne; ++i) {
    if (n4[i] == NC_FILL_INT) {
      this->m_elements.emplace_back(i + 1, &this->m_nodes[n1[i] - 1],
                                    &this->m_nodes[n2[i] - 1],
                                    &this->m_nodes[n3[i] - 1]);
    } else {
      this->m_elements.emplace_back(
          i + 1, &this->m_nodes[n1[i] - 1], &this->m_nodes[n2[i] - 1],
          &this->m_nodes[n3[i] - 1], &this->m_nodes[n4[i] - 1]);
    }
  }
}

/**
 * @brief Reads an ASCII formatted ADCIRC mesh.
 *
 * Note this will be extended in the future to netCDF formatted meshes
 */
void MeshPrivate::readAdcircMeshAscii() {
  std::ifstream fid(this->filename());

  this->readAdcircMeshHeader(fid);
  this->readAdcircNodes(fid);
  this->readAdcircElements(fid);
  this->readAdcircOpenBoundaries(fid);
  this->readAdcircLandBoundaries(fid);

  fid.close();
}

/**
 * @brief Reads an Aquaveo generic mesh format (2dm)
 *
 */
void MeshPrivate::read2dmMesh() {
  std::vector<std::string> nodes, elements;
  this->read2dmData(nodes, elements);
  this->read2dmNodes(nodes);
  this->read2dmElements(elements);
}

/**
 * @brief Read a DFlow-FM unstructured mesh file
 */
void MeshPrivate::readDflowMesh() {
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

  std::vector<double> xcoor(nn);
  std::vector<double> ycoor(nn);
  std::vector<double> zcoor(nn);
  std::vector<int> elem(ne * nmaxnode);

  ierr += nc_get_var_double(ncid, varid_x, xcoor.data());
  ierr += nc_get_var_double(ncid, varid_y, ycoor.data());
  ierr += nc_get_var_double(ncid, varid_z, zcoor.data());
  ierr += nc_get_var_int(ncid, varid_elem, elem.data());
  nc_close(ncid);

  if (ierr != 0) {
    adcircmodules_throw_exception("Error reading arrays from netcdf file.");
    return;
  }

  this->m_nodes.resize(nn);
  for (size_t i = 0; i < nn; ++i) {
    this->m_nodes[i] = Node(i + 1, xcoor[i], ycoor[i], zcoor[i]);
  }

  xcoor.clear();
  ycoor.clear();
  zcoor.clear();

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
      adcircmodules_throw_exception("Invalid element type detected");
      return;
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
    this->m_elements[i].sortVerticesAboutCenter();
  }
}

/**
 * @brief Reads the data from the 2dm file into the appropriate vector
 * @param[out] nodes node cards vector
 * @param[out] elements element cards vector
 */
void MeshPrivate::read2dmData(std::vector<std::string> &nodes,
                              std::vector<std::string> &elements) {
  std::ifstream fid(this->filename());

  std::string junk;
  std::getline(fid, junk);
  std::getline(fid, junk);
  junk = junk.substr(8, junk.size());
  junk.erase(std::remove(junk.begin(), junk.end(), '\"'), junk.end());
  this->m_meshHeaderString = StringConversion::sanitizeString(junk);
  if (this->m_meshHeaderString.empty()) {
    this->m_meshHeaderString = "Mesh";
  }

  std::string templine;
  while (std::getline(fid, templine)) {
    std::vector<std::string> templist;
    Adcirc::FileIO::Generic::splitString(templine, templist);
    if (templist[0] == "ND") {
      nodes.push_back(templine);
    } else if (templist[0] == "E4Q" || templist[0] == "E3T") {
      elements.push_back(templine);
    }
  }
  fid.close();
}

/**
 * @brief Builds a lookup table in the case that the nodes are not numbered in
 * order
 */
void MeshPrivate::buildNodeLookupTable() {
  this->m_nodeLookup.reserve(this->numNodes());
  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->m_nodeLookup[this->m_nodes[i].id()] = i;
  }
}

/**
 * @brief Parses the node data into data structures
 * @param nodes vector of node data from 2dm file
 */
void MeshPrivate::read2dmNodes(std::vector<std::string> &nodes) {
  this->m_nodes.reserve(nodes.size());
  this->m_nodeOrderingLogical = true;
  for (auto &n : nodes) {
    size_t id;
    double x, y, z;
    Adcirc::FileIO::SMSIO::splitString2dmNodeFormat(n, id, x, y, z);
    this->m_nodes.emplace_back(id, x, y, z);
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
void MeshPrivate::read2dmElements(std::vector<std::string> &elements) {
  this->m_elements.reserve(elements.size());
  for (auto &e : elements) {
    size_t id;
    std::vector<size_t> n;
    if (Adcirc::FileIO::SMSIO::splitString2dmElementFormat(e, id, n)) {
      if (n.size() == 3) {
        if (this->m_nodeOrderingLogical) {
          this->m_elements.emplace_back(id, &this->m_nodes[n[0] - 1],
                                        &this->m_nodes[n[1] - 1],
                                        &this->m_nodes[n[2] - 1]);
        } else {
          this->m_elements.emplace_back(
              id, &this->m_nodes[this->m_nodeLookup[n[0]]],
              &this->m_nodes[this->m_nodeLookup[n[1]]],
              &this->m_nodes[this->m_nodeLookup[n[2]]]);
        }
      } else if (n.size() == 4) {
        if (this->m_nodeOrderingLogical) {
          this->m_elements.emplace_back(
              id, &this->m_nodes[n[0] - 1], &this->m_nodes[n[1] - 1],
              &this->m_nodes[n[2] - 1], &this->m_nodes[n[3] - 1]);
        } else {
          this->m_elements.emplace_back(
              id, &this->m_nodes[this->m_nodeLookup[n[0]]],
              &this->m_nodes[this->m_nodeLookup[n[1]]],
              &this->m_nodes[this->m_nodeLookup[n[2]]],
              &this->m_nodes[this->m_nodeLookup[n[3]]]);
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
}

/**
 * @brief Determine the mesh format based upon file extension
 * @return MeshFormat enum
 */
Adcirc::Geometry::MeshFormat MeshPrivate::getMeshFormat(
    const std::string &filename) {
  std::string extension = Adcirc::FileIO::Generic::getFileExtension(filename);
  if (extension == ".14" || extension == ".grd") {
    return MeshAdcirc;
  } else if (extension == ".2dm") {
    return Mesh2DM;
  } else if (filename.find("_net.nc") != std::string::npos) {
    return MeshDFlow;
  } else if (extension == ".nc") {
    return MeshAdcircNetcdf;
  } else {
    return MeshUnknown;
  }
}

/**
 * @brief Reads the mesh title line and node/element dimensional data
 * @param fid std::ifstream reference for the currently opened mesh
 */
void MeshPrivate::readAdcircMeshHeader(std::ifstream &fid) {
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
  Adcirc::FileIO::Generic::splitString(tempLine, tempList);
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
}

/**
 * @brief Reads the node section of the ASCII formatted mesh
 * @param fid std::ifstream reference for the currently opened mesh
 */
void MeshPrivate::readAdcircNodes(std::ifstream &fid) {
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
}

/**
 * @brief Reads the elements section of the ASCII mesh
 * @param fid std::ifstream reference for the currently opened mesh
 */
void MeshPrivate::readAdcircElements(std::ifstream &fid) {
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
}

/**
 * @brief Reads the open boundaries section of the ASCII formatted mesh file
 * @param fid std::ifstream reference for the currently opened mesh
 */
void MeshPrivate::readAdcircOpenBoundaries(std::ifstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;

  std::getline(fid, tempLine);
  Adcirc::FileIO::Generic::splitString(tempLine, tempList);

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
    Adcirc::FileIO::Generic::splitString(tempLine, tempList);

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
}

/**
 * @brief Reads the land boundaries section of the ASCII formatted mesh file
 * @param fid std::ifstream reference for the currently opened mesh
 */
void MeshPrivate::readAdcircLandBoundaries(std::ifstream &fid) {
  std::string tempLine;
  std::vector<std::string> tempList;

  size_t n1, n2;
  double supercritical, subcritical, crest, pipeheight, pipediam, pipecoef;
  bool ok;

  std::getline(fid, tempLine);

  Adcirc::FileIO::Generic::splitString(tempLine, tempList);

  this->setNumLandBoundaries(StringConversion::stringToSizet(tempList[0], ok));
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading boundaries");
  }
  this->m_landBoundaries.resize(this->numLandBoundaries());

  std::getline(fid, tempLine);

  for (auto &b : this->m_landBoundaries) {
    std::getline(fid, tempLine);
    Adcirc::FileIO::Generic::splitString(tempLine, tempList);

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
}

/**
 * @brief Returns a reference to the elemental search kd-tree
 * @return kd-tree object with element centers as search locations
 */
Adcirc::Kdtree *MeshPrivate::elementalSearchTree() const {
  return m_elementalSearchTree.get();
}

/**
 * @brief Returns a refrence to the nodal search kd-tree
 * @return kd-tree object with mesh nodes as serch locations
 */
Adcirc::Kdtree *MeshPrivate::nodalSearchTree() const {
  return m_nodalSearchTree.get();
}

/**
 * @brief Returns the number of land boundary nodes in the mesh
 * @return Number of nodes that fall on a land boundary
 */
size_t MeshPrivate::totalLandBoundaryNodes() {
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
void MeshPrivate::setZ(std::vector<double> &z) {
  assert(z.size() == this->numNodes());
  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->m_nodes[i].setZ(z[i]);
  }
}

/**
 * @brief Returns the number of open boundary nodes in the mesh
 * @return Number of open boundary nodes
 */
size_t MeshPrivate::totalOpenBoundaryNodes() {
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
Node *MeshPrivate::node(size_t index) {
  if (index < this->numNodes()) {
    return &this->m_nodes[index];
  } else {
    adcircmodules_throw_exception("Mesh: Node index " + std::to_string(index) +
                                  " out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns the requested node from the internal node vector
 * @param index location of the node in the vector
 * @return Node
 */
Node MeshPrivate::nodeC(size_t index) const {
  if (index < this->numNodes()) {
    return this->m_nodes[index];
  } else {
    adcircmodules_throw_exception("Mesh: Node index " + std::to_string(index) +
                                  " out of bounds");
    return Node();
  }
}

/**
 * @brief Returns a pointer to the requested element in the internal element
 * vector
 * @param index location of the node in the vector
 * @return Element pointer
 */
Element *MeshPrivate::element(size_t index) {
  // if (index < this->numElements()) {
  return &this->m_elements[index];
  //} else {
  //  adcircmodules_throw_exception("Mesh: Element index out of bounds");
  //  return nullptr;
  //}
}

/**
 * @brief Returns the requested element in the internal element
 * vector
 * @param index location of the node in the vector
 * @return Element
 */
Element MeshPrivate::elementC(size_t index) const {
  if (index < this->numElements()) {
    return this->m_elements[index];
  } else {
    adcircmodules_throw_exception("Mesh: Element index out of bounds");
    return Element();
  }
}

/**
 * @brief Returns a pointer to the requested node by its ID
 * @param id Nodal ID to return a reference to
 * @return Node pointer
 */
Node *MeshPrivate::nodeById(size_t id) {
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
Element *MeshPrivate::elementById(size_t id) {
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
Boundary *MeshPrivate::openBoundary(size_t index) {
  if (index < this->numOpenBoundaries()) {
    return &this->m_openBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Open boundary index out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns the open boundary by index
 * @param index index in the open boundary array
 * @return Boundary
 */
Boundary MeshPrivate::openBoundaryC(size_t index) const {
  if (index < this->numOpenBoundaries()) {
    return this->m_openBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Open boundary index out of bounds");
    return Boundary();
  }
}

/**
 * @brief Returns a pointer to a land boundary by index
 * @param index index in the land boundary array
 * @return Boundary pointer
 */
Boundary *MeshPrivate::landBoundary(size_t index) {
  if (index < this->numLandBoundaries()) {
    return &this->m_landBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Land boundary index out of bounds");
    return nullptr;
  }
}

/**
 * @brief Returns and boundary by index
 * @param index index in the land boundary array
 * @return Boundary
 */
Boundary MeshPrivate::landBoundaryC(size_t index) const {
  if (index < this->numLandBoundaries()) {
    return this->m_landBoundaries[index];
  } else {
    adcircmodules_throw_exception("Mesh: Land boundary index out of bounds");
    return Boundary();
  }
}

/**
 * @brief Sets the mesh projection using an EPSG code. Note this does not
 * reproject the mesh.
 * @param epsg EPSG code for the mesh
 * @param isLatLon defines if the current projection is a lat/lon projection.
 * This helps define the significant digits when writing the mesh file
 */
void MeshPrivate::defineProjection(int epsg, bool isLatLon) {
  this->m_epsg = epsg;
  this->m_isLatLon = isLatLon;
}

/**
 * @brief Returns the EPSG code for the current mesh projection
 * @return EPSG code
 */
int MeshPrivate::projection() const { return this->m_epsg; }

/**
 * @brief Returns true if the mesh is in a geographic projection
 * @return boolean value for mesh projection type
 */
bool MeshPrivate::isLatLon() const { return this->m_isLatLon; }

/**
 * @brief Reprojects a mesh into the specified projection
 * @param epsg EPSG coordinate system to convert the mesh into
 */
void MeshPrivate::reproject(int epsg) {
  std::vector<double> xin, xout, yin, yout;
  xin.reserve(this->numNodes());
  yin.reserve(this->numNodes());

  for (const auto &n : this->m_nodes) {
    xin.push_back(n.x());
    yin.push_back(n.y());
  }

  bool isLatLon = false;
  int ierr = Adcirc::Projection::transform(this->projection(), epsg, xin, yin,
                                           xout, yout, isLatLon);

  if (ierr != 0) {
    adcircmodules_throw_exception("Mesh: Proj library error");
  }

  for (size_t i = 0; i < this->numNodes(); ++i) {
    this->node(i)->setX(xout[i]);
    this->node(i)->setY(yout[i]);
  }

  this->defineProjection(epsg, isLatLon);
}

/**
 * @brief Writes the mesh nodes into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void MeshPrivate::toNodeShapefile(const std::string &outputFile) {
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

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }
}

/**
 * @brief Writes the .prj file associated with a shapefile
 * @param outputFile name of output file (.shp) being created
 */
void MeshPrivate::writePrjFile(const std::string &outputFile) const {
#ifdef USE_GDAL
  std::string outputPrj =
      Adcirc::FileIO::Generic::getFileWithoutExtension(outputFile);
  outputPrj += ".prj";

  OGRSpatialReference s;
  OGRErr e = s.importFromEPSG(this->m_epsg);
  if (e != 0) {
    Adcirc::Logging::warning("Could not convert EPSG to write PRJ file");
    return;
  }

  //...Generate the Wkt string
  char *pj;
  s.morphToESRI();
  s.exportToWkt(&pj);

  //...Write the output file
  std::ofstream f(outputPrj);
  f << pj;
  f.close();
  CPLFree(pj);
#else
  Adcirc::Logging::warning(
      "ESRI .prj file not written because GDAL not enabled.");
#endif
}

/**
 * @brief Generates a table containing the node-to-node links that form elements
 * @return vector of unique node pairs
 */
std::vector<std::pair<Node *, Node *>> MeshPrivate::generateLinkTable() {
  std::vector<std::pair<Node *, Node *>> legs;
  legs.reserve(this->numElements() * 4);
  for (auto &e : this->m_elements) {
    Element e_s = e;
    e_s.sortVerticesAboutCenter();
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
void MeshPrivate::toConnectivityShapefile(const std::string &outputFile) {
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

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }
}

/**
 * @brief Writes the mesh elements as polygons into ESRI shapefile format
 * @param outputFile output file with .shp extension
 */
void MeshPrivate::toElementShapefile(const std::string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_POLYGON);
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
        SHPCreateSimpleObject(SHPT_POLYGON, e.n(), longitude, latitude, nodez);
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

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }
}

void MeshPrivate::toBoundaryShapefile(const std::string &outputFile) {
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

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }
}

void MeshPrivate::toBoundaryLineShapefile(const std::string &outputFile,
                                          const bool bothSides) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_ARCZ);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());
  DBFAddField(dbfid, "bndId", FTInteger, 16, 0);
  DBFAddField(dbfid, "bndCode", FTInteger, 16, 0);

  int bcid = 0;
  for (auto &b : this->m_openBoundaries) {
    std::vector<double> lon(b.size());
    std::vector<double> lat(b.size());
    std::vector<double> elv(b.size());

    for (size_t i = 0; i < b.size(); ++i) {
      lon[i] = b.node1(i)->x();
      lat[i] = b.node1(i)->y();
      elv[i] = -9999.0;
    }

    SHPObject *shpobj = SHPCreateSimpleObject(SHPT_ARCZ, b.size(), lon.data(),
                                              lat.data(), elv.data());
    int shp_index = SHPWriteObject(shpid, -1, shpobj);
    SHPDestroyObject(shpobj);
    DBFWriteIntegerAttribute(dbfid, shp_index, 0, bcid);
    DBFWriteIntegerAttribute(dbfid, shp_index, 1, b.boundaryCode());
    bcid++;
  }

  for (auto &b : this->m_landBoundaries) {
    std::vector<double> lon1;
    std::vector<double> lat1;
    std::vector<double> elv1;
    std::vector<double> lon2;
    std::vector<double> lat2;

    lon1.reserve(b.size());
    lat1.reserve(b.size());
    elv1.reserve(b.size());
    if (b.isInternalWeir()) {
      lon2.reserve(b.size());
      lat2.reserve(b.size());
    }

    if (b.isInternalWeir() && bothSides) {
      for (size_t i = 0; i < b.size(); ++i) {
        lon1.push_back(b.node1(i)->x());
        lat1.push_back(b.node1(i)->y());
        lon2.push_back(b.node2(i)->x());
        lat2.push_back(b.node2(i)->y());
        elv1.push_back(b.crestElevation(i));

        SHPObject *shpobj1 = SHPCreateSimpleObject(
            SHPT_ARCZ, lon1.size(), lon1.data(), lat1.data(), elv1.data());
        int shp_index1 = SHPWriteObject(shpid, -1, shpobj1);
        SHPDestroyObject(shpobj1);
        DBFWriteIntegerAttribute(dbfid, shp_index1, 0, bcid);
        DBFWriteIntegerAttribute(dbfid, shp_index1, 1, b.boundaryCode());

        SHPObject *shpobj2 = SHPCreateSimpleObject(
            SHPT_ARCZ, lon2.size(), lon2.data(), lat2.data(), elv1.data());
        int shp_index2 = SHPWriteObject(shpid, -1, shpobj2);
        SHPDestroyObject(shpobj2);
        DBFWriteIntegerAttribute(dbfid, shp_index2, 0, bcid);
        DBFWriteIntegerAttribute(dbfid, shp_index2, 1, b.boundaryCode());
      }
    } else {
      for (size_t i = 0; i < b.size(); ++i) {
        if (b.isInternalWeir()) {
          lon1.push_back((b.node1(i)->x() + b.node2(i)->x()) / 2.0);
          lat1.push_back((b.node1(i)->y() + b.node2(i)->y()) / 2.0);
          elv1.push_back(b.crestElevation(i));
        } else if (b.isExternalWeir()) {
          lon1.push_back(b.node1(i)->x());
          lat1.push_back(b.node1(i)->y());
          elv1.push_back(b.crestElevation(i));
        } else {
          lon1.push_back(b.node1(i)->x());
          lat1.push_back(b.node1(i)->y());
          elv1.push_back(-9999.0);
        }
      }
      SHPObject *shpobj = SHPCreateSimpleObject(
          SHPT_ARCZ, lon1.size(), lon1.data(), lat1.data(), elv1.data());
      int shp_index = SHPWriteObject(shpid, -1, shpobj);
      SHPDestroyObject(shpobj);
      DBFWriteIntegerAttribute(dbfid, shp_index, 0, bcid);
      DBFWriteIntegerAttribute(dbfid, shp_index, 1, b.boundaryCode());
    }
    bcid++;
  }

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }

  DBFClose(dbfid);
  SHPClose(shpid);
}

void MeshPrivate::toWeirPolygonShapefile(const std::string &outputFile) {
  SHPHandle shpid = SHPCreate(outputFile.c_str(), SHPT_POLYGON);
  DBFHandle dbfid = DBFCreate(outputFile.c_str());
  DBFAddField(dbfid, "bndId", FTInteger, 16, 0);

  size_t bndid = 0;
  for (auto &b : this->m_landBoundaries) {
    if (b.isInternalWeir()) {
      std::vector<double> lon;
      std::vector<double> lat;
      // std::vector<double> elv;
      lon.reserve((b.size() * 2) + 1);
      lat.reserve((b.size() * 2) + 1);
      // elv.reserve((b.size() * 2) + 1);
      for (size_t j = 0; j < b.size(); ++j) {
        lon.push_back(b.node1(j)->x());
        lat.push_back(b.node1(j)->y());
        // elv.push_back(b.crestElevation(j));
      }
      for (size_t j = b.size(); j > 0; --j) {
        lon.push_back(b.node2(j - 1)->x());
        lat.push_back(b.node2(j - 1)->y());
        // elv.push_back(b.crestElevation(j - 1));
      }
      lon.push_back(b.node1(0)->x());
      lat.push_back(b.node1(0)->y());
      // elv.push_back(b.crestElevation(0));

      SHPObject *shpobj = SHPCreateSimpleObject(
          SHPT_POLYGON, lon.size(), lon.data(), lat.data(), nullptr);

      int shp_index = SHPWriteObject(shpid, -1, shpobj);
      SHPDestroyObject(shpobj);
      DBFWriteIntegerAttribute(dbfid, shp_index, 0, bndid);

      bndid++;
    }
  }

  if (this->m_epsg != 0) {
    this->writePrjFile(outputFile);
  }

  SHPClose(shpid);
  DBFClose(dbfid);
}

/**
 * @brief Builds a kd-tree object with the mesh nodes as the search locations
 */
void MeshPrivate::buildNodalSearchTree() {
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
}

/**
 * @brief Builds a kd-tree object with the element centers as the search
 * locations
 */
void MeshPrivate::buildElementalSearchTree() {
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
}

/**
 * @brief Deletes the nodal search tree
 */
void MeshPrivate::deleteNodalSearchTree() {
  if (this->nodalSearchTreeInitialized()) {
    this->m_nodalSearchTree.reset();
  }
}

/**
 * @brief Deletes the elemental search tree
 */
void MeshPrivate::deleteElementalSearchTree() {
  if (this->elementalSearchTreeInitialized()) {
    this->m_elementalSearchTree.reset();
  }
}

/**
 * @brief Returns a boolean value determining if the nodal search tree has
 * been initialized
 * @return true if the search tree is initialized
 */
bool MeshPrivate::nodalSearchTreeInitialized() {
  return this->m_nodalSearchTree->initialized();
}

/**
 * @brief Returns a boolean value determining if the elemental search tree has
 * been initialized
 * @return true of the search tree is initialized
 */
bool MeshPrivate::elementalSearchTreeInitialized() {
  return this->m_elementalSearchTree->initialized();
}

/**
 * @brief Resizes the vectors within the mesh
 * @param numNodes Number of nodes
 * @param numElements Number of elements
 * @param numOpenBoundaries Number of open boundaries
 * @param numLandBoundaries Number of land boundaries
 */
void MeshPrivate::resizeMesh(size_t numNodes, size_t numElements,
                             size_t numOpenBoundaries,
                             size_t numLandBoundaries) {
  if (numNodes != this->numNodes()) {
    this->setNumNodes(numNodes);
  }

  if (numElements != this->numElements()) {
    this->setNumElements(numElements);
  }

  if (numOpenBoundaries != this->numOpenBoundaries()) {
    this->setNumOpenBoundaries(numOpenBoundaries);
  }

  if (numLandBoundaries != this->numLandBoundaries()) {
    this->setNumLandBoundaries(numLandBoundaries);
  }
}

/**
 * @brief Adds a node at the specified index in the node vector
 * @param index location where the node should be added
 * @param node Reference to an Node object
 */
void MeshPrivate::addNode(size_t index, const Node &node) {
  if (index < this->numNodes()) {
    this->m_nodes[index] = node;
  } else if (index == this->numNodes()) {
    this->m_nodes.push_back(node);
  } else {
    adcircmodules_throw_exception("Mesh: Node index > number of nodes");
  }
}
void MeshPrivate::addNode(size_t index, const Node *node) {
  if (index < this->numNodes()) {
    this->m_nodes[index] =
        Adcirc::Geometry::Node(node->id(), node->x(), node->y(), node->z());
  } else {
    adcircmodules_throw_exception("Mesh: Node index > number of nodes");
  }
}

/**
 * @brief Deletes an Node object at the specified index and shifts the
 * remaining nodes forward in the vector
 * @param index location where the node should be deleted from
 */
void MeshPrivate::deleteNode(size_t index) {
  if (index < this->numNodes()) {
    this->m_nodes.erase(this->m_nodes.begin() + index);
    this->setNumNodes(this->m_nodes.size());
  } else {
    adcircmodules_throw_exception("Mesh: Node index > number of nodes");
  }
}

/**
 * @brief Adds an Element at the specified position in the element vector
 * @param index location where the element should be added
 * @param element reference to the Element to add
 */
void MeshPrivate::addElement(size_t index, const Element &element) {
  if (index < this->numElements()) {
    this->m_elements[index] = element;
  } else if (index == this->numElements()) {
    this->m_elements.push_back(element);
  } else {
    adcircmodules_throw_exception("Mesh: Element index > number of elements");
  }
}

/**
 * @brief Deletes an Element object at the specified index and shifts the
 * remaining elements forward in the vector
 * @param index location where the element should be deleted from
 */
void MeshPrivate::deleteElement(size_t index) {
  if (index < this->numElements()) {
    this->m_elements.erase(this->m_elements.begin() + index);
    this->setNumElements(this->m_elements.size());
  } else {
    adcircmodules_throw_exception("Mesh: Element index > number of elements");
  }
}

void MeshPrivate::addLandBoundary(size_t index, const Boundary &bnd) {
  if (index < this->numLandBoundaries()) {
    this->m_landBoundaries[index] = bnd;
  } else if (index == this->numLandBoundaries()) {
    this->m_landBoundaries.push_back(bnd);
  } else {
    adcircmodules_throw_exception(
        "Mesh: Land boundary index > number of boundaries");
  }
}

void MeshPrivate::deleteLandBoundary(size_t index) {
  if (index < this->numLandBoundaries()) {
    this->m_landBoundaries.erase(this->m_landBoundaries.begin() + index);
  } else {
    adcircmodules_throw_exception(
        "Mesh: Land boundary index > number of boundaries");
  }
}

void MeshPrivate::addOpenBoundary(size_t index, const Boundary &bnd) {
  if (index < this->numOpenBoundaries()) {
    this->m_openBoundaries[index] = bnd;
  } else if (index == this->numOpenBoundaries()) {
    this->m_openBoundaries.push_back(bnd);
  } else {
    adcircmodules_throw_exception(
        "Mesh: Open boundary index > number of boundaries");
  }
}

void MeshPrivate::deleteOpenBoundary(size_t index) {
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
void MeshPrivate::write(const std::string &outputFile, MeshFormat format) {
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
void MeshPrivate::writeAdcircMesh(const std::string &filename) {
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
}

/**
 * @brief Writes an Mesh object to disk in Aquaveo 2dm ASCII format
 * @param filename name of the output file to write
 */
void MeshPrivate::write2dmMesh(const std::string &filename) {
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
}

/**
 * @brief Get the maximum number of nodes per element
 * @return max nodes per element
 */
size_t MeshPrivate::getMaxNodesPerElement() {
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
void MeshPrivate::writeDflowMesh(const std::string &filename) {
  std::vector<std::pair<Node *, Node *>> links = this->generateLinkTable();
  size_t nlinks = links.size();
  size_t maxelemnode = this->getMaxNodesPerElement();

  std::vector<double> xarray(this->numNodes());
  std::vector<double> yarray(this->numNodes());
  std::vector<double> zarray(this->numNodes());
  std::vector<int> linkArray(nlinks * 2);
  std::vector<int> linkTypeArray(nlinks * 2);
  std::vector<int> netElemNodearray(this->numElements() * maxelemnode);

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
  if (ierr != NC_NOERR)
    adcircmodules_throw_exception("Error creating D-Flow netCDF format file");

  ierr += nc_def_dim(ncid, "nNetNode", this->numNodes(), &dimid_nnode);
  ierr += nc_def_dim(ncid, "nNetLink", nlinks, &dimid_nlink);
  ierr += nc_def_dim(ncid, "nNetElem", this->numElements(), &dimid_nelem);
  ierr += nc_def_dim(ncid, "nNetElemMaxNode", maxelemnode, &dimid_maxnode);
  ierr += nc_def_dim(ncid, "nNetLinkPts", 2, &dimid_nlinkpts);
  if (ierr != NC_NOERR)
    adcircmodules_throw_exception(
        "Error creating dimensions for D-Flow netCDF format file");

  int dim1d[1];
  int dim2d[2];

  int varid_mesh2d, varid_netnodex, varid_netnodey, varid_netnodez;
  ierr += nc_def_var(ncid, "Mesh2D", NC_INT, 0, nullptr, &varid_mesh2d);
  dim1d[0] = dimid_nnode;
  ierr += nc_def_var(ncid, "NetNode_x", NC_DOUBLE, 1, dim1d, &varid_netnodex);
  ierr += nc_def_var(ncid, "NetNode_y", NC_DOUBLE, 1, dim1d, &varid_netnodey);
  ierr += nc_def_var(ncid, "NetNode_z", NC_DOUBLE, 1, dim1d, &varid_netnodez);

  dim1d[0] = dimid_nlink;
  dim2d[0] = dimid_nlink;
  dim2d[1] = dimid_nlinkpts;

  int varid_netlinktype, varid_netlink, varid_crs;
  ierr += nc_def_var(ncid, "NetLinkType", NC_INT, 1, dim1d, &varid_netlinktype);
  ierr += nc_def_var(ncid, "NetLink", NC_INT, 2, dim2d, &varid_netlink);
  ierr += nc_def_var(ncid, "crs", NC_INT, 0, nullptr, &varid_crs);

  dim2d[0] = dimid_nelem;
  dim2d[1] = dimid_maxnode;

  int varid_netelemnode;
  ierr += nc_def_var(ncid, "NetElemNode", NC_INT, 2, dim2d, &varid_netelemnode);
  if (ierr != NC_NOERR)
    adcircmodules_throw_exception(
        "Error defining variables for D-Flow netCDF format file");

  //...Define attributes
  ierr = nc_put_att_text(ncid, varid_mesh2d, "cf_role", 13, "mesh_topology");
  dim1d[0] = 2;
  ierr += nc_put_att_int(ncid, varid_mesh2d, "topology_dimension", NC_INT, 1,
                         dim1d);
  ierr += nc_put_att_text(ncid, varid_mesh2d, "node_coordinates", 19,
                          "NetNode_x NetNode_y");
  ierr += nc_put_att_text(ncid, varid_mesh2d, "node_dimension", 8, "nNetNode");
  ierr += nc_put_att_text(ncid, varid_mesh2d, "face_node_connectivity", 11,
                          "NetElemNode");
  ierr += nc_put_att_text(ncid, varid_mesh2d, "face_dimension", 8, "nNetElem");
  ierr += nc_put_att_text(ncid, varid_mesh2d, "edge_node_connectivity", 7,
                          "NetLink");
  ierr = nc_put_att_text(ncid, varid_mesh2d, "edge_dimension", 8, "nNetLink");

  if (this->isLatLon()) {
    ierr += nc_put_att_text(ncid, varid_netnodex, "axis", 5, "theta");
    ierr += nc_put_att_text(ncid, varid_netnodex, "long_name", 19,
                            "longitude of vertex");
    ierr += nc_put_att_text(ncid, varid_netnodex, "units", 12, "degrees_east");
    ierr +=
        nc_put_att_text(ncid, varid_netnodex, "standard_name", 9, "longitude");

    ierr += nc_put_att_text(ncid, varid_netnodey, "axis", 3, "phi");
    ierr += nc_put_att_text(ncid, varid_netnodey, "long_name", 18,
                            "latitude of vertex");
    ierr += nc_put_att_text(ncid, varid_netnodey, "units", 13, "degrees_north");
    ierr +=
        nc_put_att_text(ncid, varid_netnodey, "standard_name", 8, "latitude");
    dim1d[0] = 1;
    ierr += nc_put_att_int(ncid, NC_GLOBAL, "Spherical", NC_INT, 1, dim1d);

    dim1d[0] = this->m_epsg;
    ierr += nc_put_att_int(ncid, varid_crs, "EPSG", NC_INT, 1, dim1d);
  } else {
    ierr += nc_put_att_text(ncid, varid_netnodex, "axis", 1, "X");
    ierr += nc_put_att_text(ncid, varid_netnodex, "long_name", 32,
                            "x-coordinate in Cartesian system");
    ierr += nc_put_att_text(ncid, varid_netnodex, "units", 5, "metre");
    ierr += nc_put_att_text(ncid, varid_netnodex, "standard_name", 23,
                            "projection_x_coordinate");

    ierr += nc_put_att_text(ncid, varid_netnodey, "axis", 1, "Y");
    ierr += nc_put_att_text(ncid, varid_netnodey, "long_name", 32,
                            "x-coordinate in Cartesian system");
    ierr += nc_put_att_text(ncid, varid_netnodey, "units", 5, "metre");
    ierr += nc_put_att_text(ncid, varid_netnodey, "standard_name", 23,
                            "projection_y_coordinate");

    dim1d[0] = 0;
    ierr += nc_put_att_int(ncid, NC_GLOBAL, "Spherical", NC_INT, 1, dim1d);

    dim1d[0] = this->m_epsg;
    ierr += nc_put_att_int(ncid, varid_crs, "EPSG", NC_INT, 1, dim1d);
  }

  ierr += nc_put_att_text(ncid, varid_netnodez, "axis", 1, "Z");
  ierr += nc_put_att_text(ncid, varid_netnodez, "long_name", 32,
                          "z-coordinate in Cartesian system");
  ierr += nc_put_att_text(ncid, varid_netnodez, "units", 5, "metre");
  ierr += nc_put_att_text(ncid, varid_netnodez, "standard_name", 23,
                          "projection_z_coordinate");
  ierr += nc_put_att_text(ncid, varid_netnodez, "mesh", 6, "Mesh2D");
  ierr += nc_put_att_text(ncid, varid_netnodez, "location", 4, "node");

  dim1d[0] = 1;
  ierr += nc_put_att_int(ncid, varid_netlink, "start_index", NC_INT, 1, dim1d);
  ierr +=
      nc_put_att_int(ncid, varid_netelemnode, "start_index", NC_INT, 1, dim1d);
  ierr += nc_put_att_text(ncid, NC_GLOBAL, "Conventions", 9, "UGRID-0.9");

  ierr = nc_enddef(ncid);
  if (ierr != NC_NOERR)
    adcircmodules_throw_exception(
        "Error writing variable attributes for D-Flow netCDF format file");

  ierr += nc_put_var_double(ncid, varid_netnodex, xarray.data());
  ierr += nc_put_var_double(ncid, varid_netnodey, yarray.data());
  ierr += nc_put_var_double(ncid, varid_netnodez, zarray.data());
  ierr += nc_put_var_int(ncid, varid_netlink, linkArray.data());
  ierr += nc_put_var_int(ncid, varid_netlinktype, linkTypeArray.data());
  ierr += nc_put_var_int(ncid, varid_netelemnode, netElemNodearray.data());
  ierr += nc_close(ncid);

  if (ierr != NC_NOERR)
    adcircmodules_throw_exception("Error writing D-Flow netCDF format file");
}

/**
 * @brief Allows the user to know if the code has determined that the node
 * ordering is logcical (i.e. sequential) or not
 * @return true if node ordering is sequential
 */
bool MeshPrivate::nodeOrderingIsLogical() const {
  return this->m_nodeOrderingLogical;
}

/**
 * @brief Allows the user to know if the code has determined that the element
 * ordering is logcical (i.e. sequential) or not
 * @return true if element ordering is sequential
 */
bool MeshPrivate::elementOrderingIsLogical() const {
  return this->m_elementOrderingLogical;
}

/**
 * @brief Returns the position in the array by node id
 * @param id nodal id
 * @return array position
 */
size_t MeshPrivate::nodeIndexById(size_t id) {
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
size_t MeshPrivate::elementIndexById(size_t id) {
  if (this->m_elementOrderingLogical) {
    return id - 1;
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
std::vector<double> MeshPrivate::x() {
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
std::vector<double> MeshPrivate::y() {
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
std::vector<double> MeshPrivate::z() {
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
std::vector<std::vector<double>> MeshPrivate::xyz() {
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
std::vector<std::vector<size_t>> MeshPrivate::connectivity() {
  std::vector<std::vector<size_t>> conn;
  conn.reserve(this->numElements());
  for (auto &e : this->m_elements) {
    std::vector<size_t> v;
    v.resize(e.n());
    for (size_t i = 0; i < e.n(); ++i) {
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
void MeshPrivate::cpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    double xout, yout;
    Adcirc::Projection::cpp(lambda, phi, n.x(), n.y(), xout, yout);
    n.setX(xout);
    n.setY(yout);
  }
}

/**
 * @brief Convertes mesh back from the carte parallelogrammatique projection
 */
void MeshPrivate::inverseCpp(double lambda, double phi) {
  for (auto &n : this->m_nodes) {
    double xout, yout;
    Adcirc::Projection::inverseCpp(lambda, phi, n.x(), n.y(), xout, yout);
    n.setX(xout);
    n.setY(yout);
  }
}

/**
 * @brief Finds the nearest mesh node to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest node index
 */
size_t MeshPrivate::findNearestNode(double x, double y) {
  if (!this->nodalSearchTreeInitialized()) {
    this->buildNodalSearchTree();
  }
  return this->m_nodalSearchTree->findNearest(x, y);
}

/**
 * @param location location to search
 * @return nearest node index
 */
size_t MeshPrivate::findNearestNode(Point &location) {
  return this->findNearestNode(location.x(), location.y());
}

/**
 * @brief Finds the nearest mesh element to the provided x,y
 * @param x location to search
 * @param y location to search
 * @return nearest element index
 */
size_t MeshPrivate::findNearestElement(double x, double y) {
  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }
  return this->m_elementalSearchTree->findNearest(x, y);
}

/**
 * @param location location to search
 * @return nearest element index
 */
size_t MeshPrivate::findNearestElement(Point &location) {
  return this->findNearestElement(location.x(), location.y());
}

/**
 * @brief Finds the mesh element that a given location lies within
 * @param x location to search
 * @param y location to search
 * @return index of nearest element, large integer if not found
 */
size_t MeshPrivate::findElement(double x, double y) {
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
size_t MeshPrivate::findElement(double x, double y,
                                std::vector<double> &weights) {
  constexpr int searchDepth = 20;

  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }

  std::vector<size_t> indicies =
      this->elementalSearchTree()->findXNearest(x, y, searchDepth);
  auto en = adcircmodules_default_value<size_t>();

  for (auto i : indicies) {
    if (this->element(i)->isInside(x, y)) {
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
size_t MeshPrivate::findElement(Point &location) {
  return this->findElement(location.x(), location.y());
}

/**
 * @brief Computes average size of the element edges connected to each node
 * @return vector containing size at each node
 */
std::vector<double> MeshPrivate::computeMeshSize(int epsg) {
  if (!this->topology()->elementTable()->initialized()) {
    this->topology()->elementTable()->build();
  }

  int epsg_original = this->projection();
  if (epsg != 0 && epsg_original != epsg) {
    this->reproject(epsg);
  }

  std::vector<double> meshsize(this->numNodes());

  for (size_t i = 0; i < this->numNodes(); ++i) {
    std::vector<Element *> l =
        this->topology()->elementTable()->elementList(this->node(i));
    double a = 0.0;

    for (auto &j : l) {
      a += j->elementSize(false);
    }

    if (!l.empty())
      meshsize[i] = a / l.size();
    else
      meshsize[i] = 0.0;

    if (meshsize[i] < 0.0) {
      adcircmodules_throw_exception("Error computing mesh size table.");
    }
  }

  if (epsg != 0 && epsg_original != epsg) {
    this->reproject(epsg_original);
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
std::vector<std::vector<double>> MeshPrivate::orthogonality() {
  std::vector<std::vector<double>> o;
  o.reserve(this->numElements() * 2);

  //...Build the node pairs with duplicates
  std::vector<std::tuple<Node *, Node *, Element *>> edg;
  edg.reserve(this->numElements() * 4);
  for (auto &e : this->m_elements) {
    e.sortVerticesAboutCenter();
    for (size_t i = 0; i < e.n(); i++) {
      std::pair<Node *, Node *> p = e.elementLeg(i);
      Node *a = p.first;
      Node *b = p.second;
      if (a->id() < b->id()) {
        edg.emplace_back(a, b, &e);
      } else {
        edg.emplace_back(b, a, &e);
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

std::vector<Adcirc::Geometry::Node *> MeshPrivate::boundaryNodes() {
  std::vector<std::pair<Node *, Node *>> links = this->generateLinkTable();
  std::vector<size_t> count(links.size());
  std::fill(count.begin(), count.end(), 0);

  std::unordered_map<std::pair<Node *, Node *>, size_t,
                     boost::hash<std::pair<Node *, Node *>>>
      lookup;

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

std::string MeshPrivate::hash(bool force) {
  if (this->m_hash == nullptr || force) this->generateHash(force);
  return std::string(this->m_hash.get());
}

void MeshPrivate::generateHash(bool force) {
  Adcirc::Cryptography::Hash h(this->m_hashType);
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
  this->m_hash.reset(h.getHash());
}

Adcirc::Cryptography::HashType MeshPrivate::hashType() const {
  return this->m_hashType;
}

void MeshPrivate::setHashType(const Adcirc::Cryptography::HashType &hashType) {
  this->m_hashType = hashType;
}

std::vector<Adcirc::Geometry::Node> *MeshPrivate::nodes() {
  return &this->m_nodes;
}

std::vector<Adcirc::Geometry::Element> *MeshPrivate::elements() {
  return &this->m_elements;
}

std::vector<Adcirc::Geometry::Boundary> *MeshPrivate::openBoundaries() {
  return &this->m_openBoundaries;
}

std::vector<Adcirc::Geometry::Boundary> *MeshPrivate::landBoundaries() {
  return &this->m_landBoundaries;
}

bool MeshPrivate::containsNode(const Adcirc::Geometry::Node *n, size_t &index) {
  auto id = std::find(this->m_nodes.begin(), this->m_nodes.end(), n);
  bool found = id != this->m_nodes.end();
  if (found) {
    index = id - this->m_nodes.begin();
  } else {
    index = std::numeric_limits<size_t>::max();
  }
  return found;
}

bool MeshPrivate::containsNode(const Adcirc::Geometry::Node &n, size_t &index) {
  auto id = std::find(this->m_nodes.begin(), this->m_nodes.end(), n);
  bool found = id != this->m_nodes.end();
  if (found) {
    index = id - this->m_nodes.begin();
  } else {
    index = std::numeric_limits<size_t>::max();
  }
  return found;
}

bool MeshPrivate::containsElement(const Adcirc::Geometry::Element *e,
                                  size_t &index) {
  auto id = std::find(this->m_elements.begin(), this->m_elements.end(), e);
  bool found = id != this->m_elements.end();
  if (found) {
    index = id - this->m_elements.begin();
  } else {
    index = std::numeric_limits<size_t>::max();
  }
  return found;
}

bool MeshPrivate::containsElement(const Adcirc::Geometry::Element &e,
                                  size_t &index) {
  auto id = std::find(this->m_elements.begin(), this->m_elements.end(), e);
  bool found = id != this->m_elements.end();
  if (found) {
    index = id - this->m_elements.begin();
  } else {
    index = std::numeric_limits<size_t>::max();
  }
  return found;
}

std::vector<double> MeshPrivate::extent() const {
  double xmax = -std::numeric_limits<double>::max();
  double xmin = std::numeric_limits<double>::max();
  double ymax = -std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double zmax = -std::numeric_limits<double>::max();
  double zmin = std::numeric_limits<double>::max();
  for (auto &n : this->m_nodes) {
    double x = n.x();
    double y = n.y();
    double z = n.z();
    xmin = std::min(xmin, x);
    xmax = std::max(xmax, x);
    ymin = std::min(ymin, y);
    ymax = std::max(ymax, y);
    zmin = std::min(zmin, z);
    zmax = std::max(zmax, z);
  }
  return std::vector<double>{xmin, ymin, xmax, ymax, zmin, zmax};
}

void MeshPrivate::toRaster(const std::string &filename,
                           const std::vector<double> &z,
                           const std::vector<double> &extent,
                           const double resolution, const float nullvalue,
                           const std::string &description,
                           const std::string &units,
                           const bool partialWetting) {
#ifndef USE_GDAL
  adcircmodules_throw_exception("GDAL is not enabled.");
#else

  auto getfmt = [&](const std::string &ext) -> std::string {
    if (ext == ".tif") return "GTiff";
    if (ext == ".img") return "HFA";
    return std::string();
  };

  std::string fmt = getfmt(Adcirc::FileIO::Generic::getFileExtension(filename));

  if (fmt.empty()) {
    adcircmodules_throw_exception("Could not determine the raster format");
  }

  GDALAllRegister();
  GDALDriver *driver = GetGDALDriverManager()->GetDriverByName(fmt.c_str());

  char **options = nullptr;
  if (fmt == "GTiff") {
    options = CSLSetNameValue(options, "BIGTIFF", "IF_SAFER");
    options = CSLSetNameValue(options, "TILED", "YES");
    options = CSLSetNameValue(options, "COMPRESS", "LZW");
  } else {
    options = CSLSetNameValue(options, "COMPRESS", "YES");
  }

  double xmax = std::max(extent[0], extent[2]);
  double xmin = std::min(extent[0], extent[2]);
  double ymax = std::max(extent[3], extent[1]);
  double ymin = std::min(extent[3], extent[1]);
  int nx = std::floor(std::abs(xmax - xmin) / resolution) + 1;
  int ny = std::floor(std::abs(ymax - ymin) / resolution) + 1;

  //...Make sure to fix the extents vector
  std::vector<double> extent_ordered = {xmin, ymin, xmax, ymax};

  if (nx < 3 || ny < 3) {
    adcircmodules_throw_exception("Invalid resolution specified.");
  }

  GDALDataset *raster =
      driver->Create(filename.c_str(), nx, ny, 1, GDT_Float32, options);

  double transform[6] = {xmin, resolution, 0, ymax, 0, -resolution};
  raster->SetGeoTransform(transform);

  char *cwkt = nullptr;
  OGRSpatialReference sref;
  std::string srefstr = boost::str(boost::format("EPSG:%i") % this->m_epsg);
  sref.SetWellKnownGeogCS(srefstr.c_str());
  sref.exportToWkt(&cwkt);
  raster->SetProjection(cwkt);
  CPLFree(cwkt);

  GDALRasterBand *band = raster->GetRasterBand(1);
  band->SetDescription(description.c_str());
  band->SetUnitType(units.c_str());
  band->SetNoDataValue(nullvalue);
  band->Fill(nullvalue);

  std::vector<std::vector<double>> weight;
  std::vector<size_t> elements;
  std::tie(weight, elements) =
      this->computeRasterInterpolationWeights(extent_ordered, nx, ny, resolution);
  std::vector<float> zr =
      this->getRasterValues(z, nullvalue, elements, weight, partialWetting);

  CPLErr cr = band->RasterIO(GF_Write, 0, 0, nx, ny, zr.data(), nx, ny,
                             GDT_Float32, 0, 0);
  if (cr != CE_None) {
    GDALClose(static_cast<GDALDatasetH>(raster));
    CSLDestroy(options);
    adcircmodules_throw_exception("Error during Raster I/O in GDAL library");
  }

  double bmin, bmax, bmean, bsigma;
  band->ComputeStatistics(true, &bmin, &bmax, &bmean, &bsigma, nullptr,
                          nullptr);
  band->SetStatistics(bmin, bmax, bmean, bsigma);

  GDALClose(static_cast<GDALDatasetH>(raster));
  CSLDestroy(options);

#endif
}

std::vector<float> MeshPrivate::getRasterValues(
    const std::vector<double> &z, const double nullvalue,
    const std::vector<size_t> &elements,
    const std::vector<std::vector<double>> &weights,
    const bool partialWetting) {
  size_t sz = elements.size();
  auto zv = std::vector<float>(sz);
  for (size_t i = 0; i < sz; ++i) {
    size_t e = elements[i];
    if (e == adcircmodules_default_value<size_t>()) {
      zv[i] = nullvalue;
    } else {
      size_t n1 = this->element(e)->node(0)->id() - 1;
      size_t n2 = this->element(e)->node(1)->id() - 1;
      size_t n3 = this->element(e)->node(2)->id() - 1;
      double v1 = z[n1];
      double v2 = z[n2];
      double v3 = z[n3];
      zv[i] = partialWetting ? MeshPrivate::calculateValueWithPartialWetting(
                                   v1, v2, v3, nullvalue, weights[i])
                             : MeshPrivate::calculateValueWithoutPartialWetting(
                                   v1, v2, v3, nullvalue, weights[i]);
    }
  }
  return zv;
}

std::pair<std::vector<std::vector<double>>, std::vector<size_t>>
MeshPrivate::computeRasterInterpolationWeights(
    const std::vector<double> &extent, const size_t nx, const size_t ny,
    const double resolution) {
  double xmin = extent[0];
  double ymax = extent[3];

  std::vector<size_t> elements;
  std::vector<std::vector<double>> weight;
  elements.resize(nx * ny);
  weight.resize(nx * ny);

#ifdef _OPENMP
  std::string parmessage = boost::str(
      boost::format("Using %i threads to compute interpolation weights.") %
      omp_get_num_procs());
  Adcirc::Logging::log(parmessage);
#endif

  if (!this->elementalSearchTreeInitialized()) {
    this->buildElementalSearchTree();
  }

#pragma omp parallel for shared(weight, elements, xmin, ymax) \
    schedule(dynamic) collapse(2)
  for (size_t j = 0; j < ny; ++j) {
    for (size_t i = 0; i < nx; ++i) {
      size_t k = j * nx + i;
      double x, y;
      std::tie(x, y) =
          MeshPrivate::pixelToCoordinate(i, j, resolution, xmin, ymax);
      std::vector<double> w;
      elements[k] = this->findElement(x, y, weight[k]);
    }
  }
  return {weight, elements};
}

std::pair<double, double> MeshPrivate::pixelToCoordinate(
    const size_t i, const size_t j, const double resolution, const double xmin,
    const double ymax) {
  return {i * resolution + xmin + 0.5 * resolution,
          ymax - (j + 1) * resolution + 0.5 * resolution};
}

float MeshPrivate::calculateValueWithoutPartialWetting(
    const double v1, const double v2, const double v3, const double nullvalue,
    const std::vector<double> &weight) {
  return FpCompare::equalTo(v1, nullvalue) ||
                 FpCompare::equalTo(v2, nullvalue) ||
                 FpCompare::equalTo(v3, nullvalue)
             ? static_cast<float>(nullvalue)
             : static_cast<float>(v1 * weight[0] + v2 * weight[1] +
                                  v3 * weight[2]);
}

float MeshPrivate::calculateValueWithPartialWetting(
    const double v1, const double v2, const double v3, const double nullvalue,
    const std::vector<double> &weight) {
  bool b1 = FpCompare::equalTo(v1, nullvalue) ||
            FpCompare::equalTo(v1, adcircmodules_default_value<double>());
  bool b2 = FpCompare::equalTo(v2, nullvalue) ||
            FpCompare::equalTo(v2, adcircmodules_default_value<double>());
  bool b3 = FpCompare::equalTo(v3, nullvalue) ||
            FpCompare::equalTo(v3, adcircmodules_default_value<double>());
  if (!b1 && !b2 && !b3) {
    return weight[0] * v1 + weight[1] * v2 + weight[2] * v3;
  } else if (b1 && b2 && b3) {
    return static_cast<float>(nullvalue);
  } else if (b1 && b2 && !b3) {
    return static_cast<float>(v3);
  } else if (b1 && b3 && !b2) {
    return static_cast<float>(v2);
  } else if (b2 && b3 && !b1) {
    return static_cast<float>(v1);
  } else if (b1 && b2 && !b3) {
    double f = 1.0 / (weight[0] + weight[1]);
    return static_cast<float>(weight[0] * f * v1 + weight[1] * f * v2);
  } else if (b1 && b3 && !b2) {
    double f = 1.0 / (weight[0] + weight[2]);
    return static_cast<float>(weight[0] * f * v1 + weight[2] * f * v3);
  } else if (b2 && b3 && !b1) {
    double f = 1.0 / (weight[1] + weight[2]);
    return static_cast<float>(weight[1] * f * v2 + weight[2] * f * v3);
  }
  return std::numeric_limits<float>::max();
}

Adcirc::Geometry::Topology *MeshPrivate::topology() { return m_topology.get(); }
