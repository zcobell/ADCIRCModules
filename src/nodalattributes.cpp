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
#include "nodalattributes.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include "attribute.h"
#include "boost/format.hpp"
#include "error.h"
#include "io.h"
#include "stringconversion.h"

using namespace Adcirc::ModelParameters;

NodalAttributes::NodalAttributes() : m_filename(std::string()) {
  this->m_mesh = nullptr;
  this->m_numParameters = 0;
  this->m_numNodes = 0;
}

NodalAttributes::NodalAttributes(const std::string &filename,
                                 Adcirc::Geometry::Mesh *mesh)
    : m_filename(filename) {
  this->m_mesh = mesh;
  if (this->m_mesh != nullptr) {
    this->m_numNodes = mesh->numNodes();
  }
  this->m_numParameters = 0;
}

void NodalAttributes::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

std::string NodalAttributes::filename() { return this->m_filename; }

void NodalAttributes::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_mesh = mesh;
}

Adcirc::Geometry::Mesh *NodalAttributes::mesh() { return this->m_mesh; }

size_t NodalAttributes::locateAttribute(const std::string &attributeName) {
  assert(this->m_attributeLocations.find(attributeName) !=
         this->m_attributeLocations.end());
  if (this->m_attributeLocations.find(attributeName) ==
      this->m_attributeLocations.end()) {
    return -1;
  } else {
    return this->m_attributeLocations[attributeName];
  }
}

void NodalAttributes::read() {
  std::fstream fid(this->filename(), std::fstream::in);

  this->_readFort13Header(fid);
  this->_readFort13Defaults(fid);
  this->_fillDefaultValues();
  this->_readFort13Body(fid);

  fid.close();

  if (this->m_mesh != nullptr) {
    this->_mapNodes();
  }

  return;
}

void NodalAttributes::_readFort13Header(std::fstream &fid) {
  std::string tempLine;
  bool ok;

  std::getline(fid, tempLine);

  this->setHeader(tempLine);

  std::getline(fid, tempLine);

  size_t numnodes = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) {
    adcircmodules_throw_exception("NodalAttributes: Error reading file data");
  }

  std::getline(fid, tempLine);

  size_t numparams = StringConversion::stringToSizet(tempLine, ok);
  if (!ok) {
    adcircmodules_throw_exception("NodalAttributes: Error reading file data");
  }

  this->setNumParameters(numparams);

  if (this->m_mesh != nullptr) {
    if (this->m_mesh->numNodes() != numnodes) {
      adcircmodules_throw_exception(
          "NodalAttributes: Number of nodes does not match provided mesh.");
    }
  } else {
    this->setNumNodes(numnodes);
  }

  this->m_nodalParameters.resize(this->numParameters());
  this->m_nodalData.resize(this->numParameters());
  for (size_t i = 0; i < this->numParameters(); ++i) {
    this->m_nodalData[i].resize(this->numNodes());
    for (size_t j = 0; j < this->numNodes(); ++j) {
      if (this->m_mesh != nullptr) {
        this->m_nodalData[i][j].setId(this->mesh()->node(j)->id());
      } else {
        this->m_nodalData[i][j].setId(j + 1);
      }
    }
  }

  return;
}

void NodalAttributes::_readFort13Defaults(std::fstream &fid) {
  std::string name, units, tempLine;
  std::vector<std::string> tempList;
  double defaultValue;
  std::vector<double> defaultValueVector;

  for (size_t i = 0; i < this->numParameters(); ++i) {
    std::getline(fid, name);
    name = StringConversion::sanitizeString(name);

    std::getline(fid, units);

    bool ok;
    std::getline(fid, tempLine);
    int nValues = StringConversion::stringToSizet(tempLine, ok);
    if (!ok) {
      adcircmodules_throw_exception("NodalAttributes: Error reading file data");
    }

    if (nValues == 1) {
      std::getline(fid, tempLine);
      defaultValue = StringConversion::stringToDouble(tempLine, ok);
      assert(ok);
      if (!ok) {
        adcircmodules_throw_exception(
            "NodalAttributes: Error reading file data");
      }

      this->m_nodalParameters[i] = AttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValue);
      this->m_attributeLocations[name] = i;

    } else {
      std::getline(fid, tempLine);
      int ierr = IO::splitString(tempLine, tempList);
      if (ierr != 0) {
        adcircmodules_throw_exception(
            "NodalAttributes: Error reading file data");
      }

      defaultValueVector.resize(nValues);
      for (size_t j = 0; j < nValues; ++j) {
        defaultValueVector[j] =
            StringConversion::stringToDouble(tempList[j], ok);
        assert(ok);
        if (!ok) {
          adcircmodules_throw_exception(
              "NodalAttributes: Error reading file data");
        }
      }

      this->m_nodalParameters[i] = AttributeMetadata(name, units, nValues);
      this->m_nodalParameters[i].setDefaultValue(defaultValueVector);
      this->m_attributeLocations[name] = i;
    }
  }
  return;
}

void NodalAttributes::_fillDefaultValues() {
  for (size_t i = 0; i < this->numParameters(); ++i) {
    if (this->m_nodalParameters[i].numberOfValues() == 1) {
      for (size_t j = 0; j < this->numNodes(); ++j) {
        this->m_nodalData[i][j].resize(1);
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValue());
      }
    } else {
      for (size_t j = 0; j < this->numNodes(); ++j) {
        this->m_nodalData[i][j].resize(
            this->m_nodalParameters[i].numberOfValues());
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].getDefaultValues());
      }
    }
  }
}

void NodalAttributes::_mapNodes() {
  for (size_t i = 0; i < this->numParameters(); ++i) {
    for (size_t j = 0; j < this->numNodes(); ++j) {
      this->m_nodalData[i][j].setNode(
          this->mesh()->nodeById(this->m_nodalData[i][j].id()));
    }
  }
  return;
}

void NodalAttributes::_readFort13Body(std::fstream &fid) {
  std::string tempLine, name;
  size_t node;
  double value;
  bool ok;

  for (size_t i = 0; i < this->numParameters(); ++i) {
    std::getline(fid, name);
    name = StringConversion::sanitizeString(name);
    size_t index = this->m_attributeLocations[name];

    std::getline(fid, tempLine);
    size_t numNonDefault = StringConversion::stringToSizet(tempLine, ok);
    assert(ok);
    if (!ok) {
      adcircmodules_throw_exception("NodalAttributes: Error reading file data");
    }

    size_t nValues = this->m_nodalParameters[index].numberOfValues();

    for (size_t j = 0; j < numNonDefault; ++j) {
      std::getline(fid, tempLine);

      if (nValues == 1) {
        if (!IO::splitStringAttribute1Format(tempLine, node, value)) {
          adcircmodules_throw_exception(
              "NodalAttributes: Error reading file data");
        }

        Attribute *a;
        if (this->m_mesh != nullptr) {
          a = &this->m_nodalData[i][this->mesh()->nodeIndexById(node)];
        } else {
          a = &this->m_nodalData[i][node - 1];
        }
        a->setValue(value);
        a->setId(node);

      } else {
        std::vector<double> values;
        values.reserve(nValues);
        if (!IO::splitStringAttributeNFormat(tempLine, node, values)) {
          adcircmodules_throw_exception(
              "NodalAttributes: Error reading file data");
        };

        Attribute *a;
        if (this->m_mesh != nullptr) {
          a = &this->m_nodalData[i][this->mesh()->nodeIndexById(node)];
        } else {
          a = &this->m_nodalData[i][node - 1];
        }
        a->setValue(values);
        a->setId(node);
      }
    }
  }
  return;
}

size_t NodalAttributes::numNodes() const { return this->m_numNodes; }

void NodalAttributes::setNumNodes(size_t numNodes) {
  this->m_numNodes = numNodes;
}

size_t NodalAttributes::numParameters() const { return this->m_numParameters; }

void NodalAttributes::setNumParameters(size_t numParameters) {
  this->m_numParameters = numParameters;
}

std::string NodalAttributes::header() const { return this->m_header; }

void NodalAttributes::setHeader(const std::string &header) {
  this->m_header = header;
}

Attribute *NodalAttributes::attribute(size_t parameter, size_t node) {
  assert(node < this->numNodes());
  assert(parameter < this->numParameters());

  if (node < this->numNodes() && parameter < this->numParameters()) {
    return &this->m_nodalData[parameter][node];
  }
  adcircmodules_throw_exception(
      "NodalAttributes: Attribute could not be located");
  return nullptr;
}

Attribute *NodalAttributes::attribute(const std::string &name, size_t node) {
  std::string n = name;
  std::cerr << n << "\n";
  std::cerr.flush();
  size_t index = this->locateAttribute(n);
  return this->attribute(index, node);
}

std::string NodalAttributes::attributeNames(size_t index) {
  assert(index < this->m_nodalParameters.size());
  if (index < this->m_nodalParameters.size()) {
    return this->m_nodalParameters[index].name();
  }
  adcircmodules_throw_exception("NodalAttributes: Request out of bounds.");
  return std::string();
}

void NodalAttributes::write(const std::string &outputFilename) {
  std::ofstream outputFile;
  outputFile.open(outputFilename);
  this->_writeFort13Header(outputFile);
  this->_writeFort13Body(outputFile);
  outputFile.close();
  return;
}

void NodalAttributes::_writeFort13Header(std::ofstream &fid) {
  fid << this->m_header << "\n";
  fid << boost::str(boost::format("%11i\n") % this->numNodes());
  fid << boost::str(boost::format("%11i\n") % this->numParameters());
  for (auto &v : this->m_nodalParameters) {
    fid << v.headerString();
  }
  return;
}

void NodalAttributes::_writeFort13Body(std::ofstream &fid) {
  for (size_t i = 0; i < this->numParameters(); ++i) {
    size_t ndefault =
        this->_countDefault(this->m_nodalParameters[i], this->m_nodalData[i]);
    fid << this->m_nodalParameters[i].name() << "\n";
    fid << boost::str(boost::format("%11i\n") % ndefault);

    for (size_t j = 0; j < this->numNodes(); ++j) {
      if (!this->m_nodalParameters[i].checkIfDefaultValue(
              this->m_nodalData[i][j])) {
        fid << this->m_nodalData[i][j].write();
      }
    }
  }
  return;
}

size_t NodalAttributes::_countDefault(
    Adcirc::ModelParameters::AttributeMetadata &metadata,
    std::vector<Adcirc::ModelParameters::Attribute> &values) {
  size_t n = 0;
  for (const auto &v : values) {
    if (!metadata.checkIfDefaultValue(v)) {
      n++;
    }
  }
  return n;
}
