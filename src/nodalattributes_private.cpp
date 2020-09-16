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
#include "nodalattributes_private.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include "attribute.h"
#include "boost/format.hpp"
#include "default_values.h"
#include "fileio.h"
#include "logging.h"
#include "nodalattributes.h"
#include "stringconversion.h"

using namespace Adcirc::ModelParameters;
using namespace Adcirc::Private;

NodalAttributes::~NodalAttributes() = default;

NodalAttributesPrivate::NodalAttributesPrivate()
    : m_filename(std::string()),
      m_numParameters(0),
      m_numNodes(0),
      m_mesh(nullptr){}

NodalAttributesPrivate::NodalAttributesPrivate(const std::string &filename,
                                               Adcirc::Geometry::Mesh *mesh)
    : m_filename(filename), m_numParameters(0), m_mesh(mesh) {
  if (this->m_mesh != nullptr) {
    this->m_numNodes = mesh->numNodes();
  }
}

void NodalAttributesPrivate::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

std::string NodalAttributesPrivate::filename() { return this->m_filename; }

void NodalAttributesPrivate::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_mesh = mesh;
}

Adcirc::Geometry::Mesh *NodalAttributesPrivate::mesh() { return this->m_mesh; }

size_t NodalAttributesPrivate::locateAttribute(
    const std::string &attributeName) {
  assert(this->m_attributeLocations.find(attributeName) !=
         this->m_attributeLocations.end());
  if (this->m_attributeLocations.find(attributeName) ==
      this->m_attributeLocations.end()) {
    return adcircmodules_default_value<size_t>();
  } else {
    return this->m_attributeLocations[attributeName];
  }
}

void NodalAttributesPrivate::read() {
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

void NodalAttributesPrivate::_readFort13Header(std::fstream &fid) {
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

void NodalAttributesPrivate::_readFort13Defaults(std::fstream &fid) {
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
      Adcirc::FileIO::Generic::splitString(tempLine, tempList);

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

void NodalAttributesPrivate::_fillDefaultValues() {
  for (size_t i = 0; i < this->numParameters(); ++i) {
    if (this->m_nodalParameters[i].numberOfValues() == 1) {
      for (size_t j = 0; j < this->numNodes(); ++j) {
        this->m_nodalData[i][j].resize(1);
        this->m_nodalData[i][j].setValue(
            this->m_nodalParameters[i].defaultValue());
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

void NodalAttributesPrivate::_mapNodes() {
  for (size_t i = 0; i < this->numParameters(); ++i) {
    for (size_t j = 0; j < this->numNodes(); ++j) {
      this->m_nodalData[i][j].setNode(
          this->mesh()->nodeById(this->m_nodalData[i][j].id()));
    }
  }
  return;
}

void NodalAttributesPrivate::_readFort13Body(std::fstream &fid) {
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
        if (!FileIO::AdcircIO::splitStringAttribute1Format(tempLine, node,
                                                           value)) {
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
        if (!FileIO::AdcircIO::splitStringAttributeNFormat(tempLine, node,
                                                           values)) {
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

size_t NodalAttributesPrivate::numNodes() const { return this->m_numNodes; }

void NodalAttributesPrivate::setNumNodes(size_t numNodes) {
  this->m_numNodes = numNodes;
}

size_t NodalAttributesPrivate::numParameters() const {
  return this->m_numParameters;
}

void NodalAttributesPrivate::setNumParameters(size_t numParameters) {
  this->m_numParameters = numParameters;
}

std::string NodalAttributesPrivate::header() const { return this->m_header; }

void NodalAttributesPrivate::setHeader(const std::string &header) {
  this->m_header = header;
}

Attribute *NodalAttributesPrivate::attribute(size_t parameter, size_t node) {
  assert(node < this->numNodes());
  assert(parameter < this->numParameters());

  if (node < this->numNodes() && parameter < this->numParameters()) {
    return &this->m_nodalData[parameter][node];
  }
  adcircmodules_throw_exception(
      "NodalAttributes: Attribute could not be located");
  return nullptr;
}

Attribute *NodalAttributesPrivate::attribute(const std::string &name,
                                             size_t node) {
  size_t index = this->locateAttribute(name);
  return this->attribute(index, node);
}

std::string NodalAttributesPrivate::attributeNames(size_t index) {
  assert(index < this->m_nodalParameters.size());
  if (index < this->m_nodalParameters.size()) {
    return this->m_nodalParameters[index].name();
  }
  adcircmodules_throw_exception("NodalAttributes: Request out of bounds.");
  return std::string();
}

void NodalAttributesPrivate::write(const std::string &outputFilename) {
  std::ofstream outputFile;
  outputFile.open(outputFilename);
  this->_writeFort13Header(outputFile);
  this->_writeFort13Body(outputFile);
  outputFile.close();
  return;
}

void NodalAttributesPrivate::_writeFort13Header(std::ofstream &fid) {
  fid << this->m_header << "\n";
  fid << boost::str(boost::format("%11i\n") % this->numNodes());
  fid << boost::str(boost::format("%11i\n") % this->numParameters());
  for (auto &v : this->m_nodalParameters) {
    fid << v.headerString();
  }
  return;
}

void NodalAttributesPrivate::_writeFort13Body(std::ofstream &fid) {
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

size_t NodalAttributesPrivate::_countDefault(const AttributeMetadata &metadata,
                                             const std::vector<Attribute> &values) {
  size_t n = 0;
  for (auto &v : values) {
    if (!metadata.checkIfDefaultValue(v)) {
      n++;
    }
  }
  return n;
}

AttributeMetadata *NodalAttributesPrivate::metadata(size_t parameter) {
  assert(parameter < this->numParameters());

  if (parameter < this->numParameters()) {
    return &this->m_nodalParameters[parameter];
  }
  adcircmodules_throw_exception(
      "NodalAttributes: Attribute could not be located");
  return nullptr;
}

AttributeMetadata *NodalAttributesPrivate::metadata(const std::string &name) {
  size_t index = this->locateAttribute(name);
  return this->metadata(index);
}

void NodalAttributesPrivate::addAttribute(AttributeMetadata &metadata,
                                          std::vector<Attribute> &attribute) {
  this->m_nodalParameters.push_back(metadata);
  this->m_nodalData.push_back(attribute);
  this->m_attributeLocations[metadata.name()] =
      this->m_nodalParameters.size() - 1;
  this->m_numParameters = this->m_nodalParameters.size();
}

void NodalAttributesPrivate::setAttributeData(size_t parameter, const std::vector<double> &values){
    assert(values.size()==this->numNodes());
    assert(values[0].size()==this->m_nodalParameters[parameter].numberOfValues());
    for(size_t i=0;i<this->numNodes();++i){
        this->m_nodalData[parameter][i].setValue(values[i]);
    }
}

void NodalAttributesPrivate::setAttributeData(size_t parameter, const std::vector<std::vector<double>> &values){
    assert(values.size()==this->numNodes());
    assert(values[0].size()==this->m_nodalParameters[parameter].numberOfValues());
    for(size_t i=0;i<this->numNodes();++i){
        this->m_nodalData[parameter][i].setValue(values[i]);
    }
}
