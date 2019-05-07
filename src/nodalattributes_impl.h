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
#ifndef ADCMOD_NODALATTRIBUTESIMPL_H
#define ADCMOD_NODALATTRIBUTESIMPL_H

#include <string>
#include <vector>
#include "adcmap.h"
#include "attribute.h"
#include "attributemetadata.h"
#include "mesh.h"

class NodalAttributesImpl {
 public:
  NodalAttributesImpl();

  NodalAttributesImpl(const std::string &filename,
                      Adcirc::Geometry::Mesh *mesh = nullptr);

  void read();

  void setFilename(const std::string &filename);
  std::string filename();

  void setMesh(Adcirc::Geometry::Mesh *mesh);
  Adcirc::Geometry::Mesh *mesh();

  void write(const std::string &outputFilename);

  std::string attributeNames(size_t index);

  size_t locateAttribute(const std::string &attributeName);

  std::string header() const;
  void setHeader(const std::string &title);

  size_t numParameters() const;
  void setNumParameters(size_t numParameters);

  size_t numNodes() const;
  void setNumNodes(size_t numNodes);

  Adcirc::ModelParameters::Attribute *attribute(size_t parameter, size_t node);
  Adcirc::ModelParameters::Attribute *attribute(const std::string &name,
                                                size_t node);

  Adcirc::ModelParameters::AttributeMetadata *metadata(size_t parameter);
  Adcirc::ModelParameters::AttributeMetadata *metadata(const std::string &name);

 private:
  void _readFort13Header(std::fstream &fid);
  void _readFort13Defaults(std::fstream &fid);
  void _readFort13Body(std::fstream &fid);
  void _writeFort13Body(std::ofstream &fid);
  void _writeFort13Header(std::ofstream &fid);
  void _fillDefaultValues();
  void _mapNodes();
  size_t _countDefault(Adcirc::ModelParameters::AttributeMetadata &metadata,
                       std::vector<Adcirc::ModelParameters::Attribute> &values);

  /// Mapping function between the name of a nodal parameter and its position in
  /// the nodalParameters vector
  adcmap<std::string, size_t> m_attributeLocations;

  /// Filename of the file that will be read or was read
  std::string m_filename;

  /// Title/header found at the beginning of the nodal attributes file
  std::string m_header;

  /// Number of nodal attributes found within this file
  size_t m_numParameters;

  /// Number of nodes in the mesh that this nodal attribute files references
  size_t m_numNodes;

  /// Underlying adcirc_mesh object (if necessary)
  Adcirc::Geometry::Mesh *m_mesh;

  /// Vector of objects containing the nodal parameters read from the file
  std::vector<Adcirc::ModelParameters::AttributeMetadata> m_nodalParameters;

  /// Vector of objects for the nodal parameters
  std::vector<std::vector<Adcirc::ModelParameters::Attribute> > m_nodalData;
};

#endif  // ADCMOD_NODALATTRIBUTESIMPL_H
