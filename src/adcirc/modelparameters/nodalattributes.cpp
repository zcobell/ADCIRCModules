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
#include "adcirc/modelparameters/nodalattributes.h"
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "adcirc/adcirc_codes.h"
#include "adcirc/io/io.h"
#include "adcirc/io/stringconversion.h"

using namespace Adcirc::ModelParameters;

#define CHECK_RETURN_AND_CLOSE(ierr) \
  if (ierr != Adcirc::NoError) {     \
    fid.close();                     \
    return ierr;                     \
  }

#define CHECK_FILEREAD_RETURN(ok)        \
  if (!ok) {                             \
    fid.close();                         \
    return FileIO::GenericFileReadError; \
  }

NodalAttributes::NodalAttributes() {
  this->m_mesh = nullptr;
  this->m_numParameters = 0;
  this->m_numNodes = 0;
  this->m_filename = string();
}

NodalAttributes::NodalAttributes(string filename,
                                 Adcirc::Geometry::Mesh *mesh) {
  this->m_mesh = mesh;
  if (this->m_mesh != nullptr) this->m_numNodes = mesh->numNodes();
  this->m_numParameters = 0;
  this->m_filename = filename;
}

void NodalAttributes::setFilename(string filename) {
  this->m_filename = filename;
}

string NodalAttributes::filename() { return this->m_filename; }

void NodalAttributes::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_mesh = mesh;
}

Adcirc::Geometry::Mesh *NodalAttributes::mesh() { return this->m_mesh; }

size_t NodalAttributes::locateAttribute(string attributeName) {
  assert(this->m_attributeLocations.find(attributeName) !=
         this->m_attributeLocations.end());
  if (this->m_attributeLocations.find(attributeName) ==
      this->m_attributeLocations.end())
    return -1;
  else
    return this->m_attributeLocations[attributeName];
}

int NodalAttributes::read() {
  int ierr;

  std::fstream fid(this->filename(), std::fstream::in);

  ierr = this->_readFort13Header(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  ierr = this->_readFort13Defaults(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  this->_fillDefaultValues();

  ierr = this->_readFort13Body(fid);
  CHECK_RETURN_AND_CLOSE(ierr);

  fid.close();

  if (this->m_mesh != nullptr) this->_mapNodes();

  return Adcirc::NoError;
}

int NodalAttributes::_readFort13Header(std::fstream &fid) {
  string tempLine;
  bool ok;

  std::getline(fid, tempLine);

  this->setHeader(tempLine);

  std::getline(fid, tempLine);

  size_t numnodes = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) return FileIO::GenericFileReadError;

  std::getline(fid, tempLine);

  size_t numparams = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) return FileIO::GenericFileReadError;

  this->setNumParameters(numparams);

  if (this->m_mesh != nullptr) {
    if (this->m_mesh->numNodes() != numnodes)
      return Adcirc::ModelParameters::MeshMismatch;
  } else
    this->setNumNodes(numnodes);

  this->m_nodalParameters.resize(this->numParameters());
  this->m_nodalData.resize(this->numParameters());
  for (size_t i = 0; i < this->numParameters(); i++)
    this->m_nodalData[i].resize(this->numNodes());

  return Adcirc::NoError;
}

int NodalAttributes::_readFort13Defaults(std::fstream &fid) {
  string name, units, tempLine;
  vector<string> tempList;
  double defaultValue;
  vector<double> defaultValueVector;
  int ierr;
  size_t nValues;
  bool ok;

  for (size_t i = 0; i < this->numParameters(); i++) {
    std::getline(fid, name);
    name = StringConversion::sanitizeString(name);

    std::getline(fid, units);

    std::getline(fid, tempLine);
    nValues = StringConversion::stringToSizet(tempLine, ok);
    assert(ok);
    if (!ok) return FileIO::GenericFileReadError;

    if (nValues == 1) {
      std::getline(fid, tempLine);
      defaultValue = StringConversion::stringToDouble(tempLine, ok);
      assert(ok);
      if (!ok) return FileIO::GenericFileReadError;

      this->m_nodalParameters[i] = AttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValue);
      this->m_attributeLocations[name] = i;

    } else {
      std::getline(fid, tempLine);
      ierr = IO::splitString(tempLine, tempList);
      assert(ierr == FileIO::NoError);
      if (ierr != FileIO::NoError) return FileIO::GenericFileReadError;

      defaultValueVector.resize(nValues);
      for (size_t j = 0; j < nValues; j++) {
        defaultValueVector[j] =
            StringConversion::stringToDouble(tempList[j], ok);
        assert(ok);
        if (!ok) return FileIO::GenericFileReadError;
      }

      this->m_nodalParameters[i] = AttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValueVector);
      this->m_attributeLocations[name] = i;
    }
  }
  return Adcirc::NoError;
}

void NodalAttributes::_fillDefaultValues() {
  for (size_t i = 0; i < this->numParameters(); i++) {
    if (this->m_nodalParameters[i].numberOfValues() == 1) {
      for (size_t j = 0; j < this->numNodes(); j++) {
        this->m_nodalData[i][j].resize(1);
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValue());
      }
    } else {
      for (size_t j = 0; j < this->numNodes(); j++) {
        this->m_nodalData[i][j].resize(
            this->m_nodalParameters[i].numberOfValues());
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValues());
      }
    }
  }
}

void NodalAttributes::_mapNodes() {
  for (size_t i = 0; i < this->numParameters(); i++) {
    for (size_t j = 0; j < this->numNodes(); j++) {
      this->m_nodalData[i][j].setNode(this->mesh()->node(
          this->mesh()->nodeIndexById(this->m_nodalData[i][j].id())));
    }
  }
  return;
}

int NodalAttributes::_readFort13Body(std::fstream &fid) {
  string tempLine, name;
  size_t index, numNonDefault, nValues;
  int ierr;
  size_t node;
  double value;
  bool ok;
  vector<double> values;
  vector<string> tempList;
  values.resize(12);

  for (size_t i = 0; i < this->numParameters(); i++) {
    std::getline(fid, name);
    name = StringConversion::sanitizeString(name);
    index = this->m_attributeLocations[name];

    std::getline(fid, tempLine);
    numNonDefault = StringConversion::stringToSizet(tempLine, ok);
    assert(ok);
    if (!ok) return FileIO::GenericFileReadError;

    nValues = this->m_nodalParameters[index].numberOfValues();

    for (size_t j = 0; j < numNonDefault; j++) {
      std::getline(fid, tempLine);

      if (nValues == 1) {
        ierr = IO::splitStringAttribute1Format(tempLine, node, value);
        assert(ierr == FileIO::NoError);
        if (ierr != FileIO::NoError) return ierr;

        if (this->m_mesh != nullptr) {
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setValue(
              value);
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setId(node);
        } else {
          this->m_nodalData[i][node].setValue(value);
          this->m_nodalData[i][node].setId(node);
        }

      } else if (nValues == 12) {
        ierr = IO::splitStringAttribute12Format(tempLine, node, values);
        assert(ierr == FileIO::NoError);
        if (ierr != FileIO::NoError) return ierr;

        if (this->m_mesh != nullptr) {
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setValue(
              values);
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setId(node);
        } else {
          this->m_nodalData[i][node].setValue(values);
          this->m_nodalData[i][node].setId(node);
        }

      } else {
        //...Generic routine here is slower than above, but there is
        //   no reason it will ever be used in ADCIRC's current state.
        //   Placed here as future-proofing.
        ierr = IO::splitString(tempLine, tempList);
        node = StringConversion::stringToSizet(tempList[0], ok);
        assert(ok);
        if (!ok) return FileIO::GenericFileReadError;

        for (size_t j = 1; j < tempList.size(); j++) {
          value = StringConversion::stringToDouble(tempList[j], ok);
          assert(ok);
          if (!ok) return FileIO::GenericFileReadError;

          if (this->m_mesh != nullptr)
            this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setValue(
                j, value);
          else
            this->m_nodalData[i][node].setValue(j, value);
        }
      }
    }
  }
  return Adcirc::NoError;
}

size_t NodalAttributes::numNodes() const { return this->m_numNodes; }

void NodalAttributes::setNumNodes(size_t numNodes) {
  this->m_numNodes = numNodes;
}

size_t NodalAttributes::numParameters() const { return this->m_numParameters; }

void NodalAttributes::setNumParameters(size_t numParameters) {
  this->m_numParameters = numParameters;
}

string NodalAttributes::header() const { return this->m_header; }

void NodalAttributes::setHeader(const string &header) {
  this->m_header = header;
}

Attribute *NodalAttributes::attribute(size_t parameter, size_t node) {
  assert(node < this->numNodes());
  assert(parameter < this->numParameters());

  if (node < this->numNodes() && parameter < this->numParameters()) {
    return &this->m_nodalData[parameter][node];
  } else
    return nullptr;
}

Attribute *NodalAttributes::attribute(string parameter, size_t node) {
  size_t index = this->locateAttribute(parameter);
  return this->attribute(index, node);
}

int NodalAttributes::write(string outputFilename) { return Adcirc::NoError; }

string NodalAttributes::attributeNames(size_t index) {
  assert(index < this->m_nodalParameters.size());
  if (index < this->m_nodalParameters.size())
    return this->m_nodalParameters[index].name();
  else
    return string("Request out of bounds.");
}
