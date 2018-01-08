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
#include "adcircnodalparameters.h"
#include "io.h"
#include "stringconversion.h"
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>

#define CHECK_RETURN_AND_CLOSE(ierr)                                           \
  if (ierr != Adcirc::NoError) {                                               \
    fid.close();                                                               \
    return ierr;                                                               \
  }

#define CHECK_FILEREAD_RETURN(ok)                                              \
  if (!ok) {                                                                   \
    fid.close();                                                               \
    return FileIO::GenericFileReadError;                                       \
  }

AdcircNodalParameters::AdcircNodalParameters() {
  this->m_mesh = nullptr;
  this->m_numParameters = 0;
  this->m_numNodes = 0;
  this->m_filename = std::string();
}

AdcircNodalParameters::AdcircNodalParameters(std::string filename,
                                             AdcircMesh *mesh) {
  this->m_mesh = mesh;
  if (this->m_mesh != nullptr)
    this->m_numNodes = mesh->numNodes();
  this->m_numParameters = 0;
  this->m_filename = filename;
}

AdcircNodalParameters::AdcircNodalParameters(const char *filename,
                                             AdcircMesh *mesh) {
  this->m_mesh = mesh;
  if (this->m_mesh != nullptr)
    this->m_numNodes = mesh->numNodes();
  this->m_numParameters = 0;
  this->m_filename = std::string(filename);
}

void AdcircNodalParameters::setFilename(const char *filename) {
  this->m_filename = std::string(filename);
}

void AdcircNodalParameters::setFilename(std::string filename) {
  this->m_filename = filename;
}

std::string AdcircNodalParameters::filename() { return this->m_filename; }

void AdcircNodalParameters::setMesh(AdcircMesh *mesh) { this->m_mesh = mesh; }

AdcircMesh *AdcircNodalParameters::mesh() { return this->m_mesh; }

int AdcircNodalParameters::locateAttribute(const char *attributeName) {
  return this->locateAttribute(std::string(attributeName));
}

int AdcircNodalParameters::locateAttribute(std::string attributeName) {
  return this->m_attributeLocations[attributeName];
}

int AdcircNodalParameters::read() {
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

  if (this->m_mesh != nullptr)
    this->_mapNodes();

  return Adcirc::NoError;
}

int AdcircNodalParameters::_readFort13Header(std::fstream &fid) {

  std::string tempLine;
  bool ok;

  std::getline(fid, tempLine);

  this->setHeader(tempLine);

  std::getline(fid, tempLine);

  int numnodes = StringConversion::stringToInt(tempLine, ok);
  if (!ok)
    return FileIO::GenericFileReadError;

  std::getline(fid, tempLine);

  int numparams = StringConversion::stringToInt(tempLine, ok);
  if (!ok)
    return FileIO::GenericFileReadError;

  this->setNumParameters(numparams);

  if (this->m_mesh != nullptr) {
    if (this->m_mesh->numNodes() != numnodes)
      return Adcirc::NodalParameters::MeshMismatch;
  } else
    this->setNumNodes(numnodes);

  this->m_nodalParameters.resize(this->numParameters());
  this->m_nodalData.resize(this->numParameters());
  for (int i = 0; i < this->numParameters(); i++)
    this->m_nodalData[i].resize(this->numNodes());

  return Adcirc::NoError;
}

int AdcircNodalParameters::_readFort13Defaults(std::fstream &fid) {
  std::string name, units, tempLine;
  std::vector<std::string> tempList;
  double defaultValue;
  std::vector<double> defaultValueVector;
  int nValues, ierr;
  bool ok;

  for (int i = 0; i < this->numParameters(); i++) {
    std::getline(fid, name);
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

    std::getline(fid, units);

    std::getline(fid, tempLine);
    nValues = StringConversion::stringToInt(tempLine, ok);
    assert(ok);
    if (!ok)
      return FileIO::GenericFileReadError;

    if (nValues == 1) {
      std::getline(fid, tempLine);
      defaultValue = StringConversion::stringToDouble(tempLine, ok);
      assert(ok);
      if (!ok)
        return FileIO::GenericFileReadError;

      this->m_nodalParameters[i] =
          AdcircNodalAttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValue);
      this->m_attributeLocations[name] = i;

    } else {
      std::getline(fid, tempLine);
      ierr = IO::splitString(tempLine, tempList);
      assert(ierr == FileIO::NoError);
      if (ierr != FileIO::NoError)
        return FileIO::GenericFileReadError;

      defaultValueVector.resize(nValues);
      for (int i = 0; i < nValues; i++) {
        defaultValueVector[i] =
            StringConversion::stringToDouble(tempList[i], ok);
        assert(ok);
        if (!ok)
          return FileIO::GenericFileReadError;
      }

      this->m_nodalParameters[i] =
          AdcircNodalAttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValueVector);
      this->m_attributeLocations[name] = i;
    }
  }
  return Adcirc::NoError;
}

void AdcircNodalParameters::_fillDefaultValues() {
  for (int i = 0; i < this->numParameters(); i++) {
    if (this->m_nodalParameters[i].numberOfValues() == 1) {
      for (int j = 0; j < this->numNodes(); j++) {
        this->m_nodalData[i][j].resize(1);
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValue());
      }
    } else {
      for (int j = 0; j < this->numNodes(); j++) {
        this->m_nodalData[i][j].resize(this->m_nodalParameters[i].numberOfValues());
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValues());
      }
    }
  }
}

void AdcircNodalParameters::_mapNodes() {
  for (int i = 0; i < this->numParameters(); i++) {
    for (int j = 0; j < this->numNodes(); j++) {
      this->m_nodalData[i][j].setNode(this->mesh()->node(
          this->mesh()->nodeIndexById(this->m_nodalData[i][j].id())));
    }
  }
  return;
}

int AdcircNodalParameters::_readFort13Body(std::fstream &fid) {
  std::string tempLine, name;
  int index, numNonDefault, nValues;
  int node, ierr;
  double value;
  bool ok;
  std::vector<double> values;
  std::vector<std::string> tempList;
  values.resize(12);

  for (int i = 0; i < this->numParameters(); i++) {
    std::getline(fid, name);
    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
    index = this->m_attributeLocations[name];

    std::getline(fid, tempLine);
    numNonDefault = StringConversion::stringToInt(tempLine, ok);
    assert(ok);
    if (!ok)
      return FileIO::GenericFileReadError;

    nValues = this->m_nodalParameters[index].numberOfValues();

    for (int j = 0; j < numNonDefault; j++) {
      std::getline(fid, tempLine);

      if (nValues == 1) {
        ierr = IO::splitStringNodalAttribute1Format(tempLine, node, value);
        assert(ierr == FileIO::NoError);
        if (ierr != FileIO::NoError)
          return ierr;

        if (this->m_mesh != nullptr) {
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setValue(
              value);
          this->m_nodalData[i][this->mesh()->nodeIndexById(node)].setId(node);
        } else {
          this->m_nodalData[i][node].setValue(value);
          this->m_nodalData[i][node].setId(node);
        }

      } else if (nValues == 12) {
        ierr = IO::splitStringNodalAttribute12Format(tempLine, node, values);
        assert(ierr == FileIO::NoError);
        if (ierr != FileIO::NoError)
          return ierr;

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
        node = StringConversion::stringToInt(tempList[0], ok);
        assert(ok);
        if (!ok)
          return FileIO::GenericFileReadError;

        for (int j = 1; j < tempList.size(); j++) {
          value = StringConversion::stringToDouble(tempList[j], ok);
          assert(ok);
          if (!ok)
            return FileIO::GenericFileReadError;

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

int AdcircNodalParameters::numNodes() const { return this->m_numNodes; }

void AdcircNodalParameters::setNumNodes(int numNodes) {
  this->m_numNodes = numNodes;
}

int AdcircNodalParameters::numParameters() const {
  return this->m_numParameters;
}

void AdcircNodalParameters::setNumParameters(int numParameters) {
  this->m_numParameters = numParameters;
}

std::string AdcircNodalParameters::header() const { return this->m_header; }

void AdcircNodalParameters::setHeader(const std::string &header) {
  this->m_header = header;
}

AdcircNodalAttribute *AdcircNodalParameters::nodalAttribute(int parameter,
                                                            int node) {
  assert(node >= 0 && node < this->numNodes());
  assert(parameter >= 0 && parameter < this->numParameters());

  if (node >= 0 && node < this->numNodes() && parameter >= 0 &&
      parameter < this->numParameters()) {
    return &this->m_nodalData[parameter][node];
  } else
    return nullptr;
}

AdcircNodalAttribute *
AdcircNodalParameters::nodalAttribute(std::string parameter, int node) {
  int index = this->locateAttribute(parameter);
  return this->nodalAttribute(index, node);
}

AdcircNodalAttribute *
AdcircNodalParameters::nodalAttribute(const char *parameter, int node) {
  return this->nodalAttribute(std::string(parameter), node);
}

int AdcircNodalParameters::write(const char *filename) {
  return this->write(std::string(filename));
}

int AdcircNodalParameters::write(std::string outputFilename) {
  return Adcirc::NoError;
}
