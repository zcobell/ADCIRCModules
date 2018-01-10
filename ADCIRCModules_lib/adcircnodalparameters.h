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
#ifndef ADCIRCNODALPARAMETERS_H
#define ADCIRCNODALPARAMETERS_H

#include "adcircenum.h"
#include "adcircmesh.h"
#include "adcircnodalattribute.h"
#include "adcircnodalattributemetadata.h"
#include <string>
#include <unordered_map>
#include <vector>

class AdcircNodalParameters {
public:
  AdcircNodalParameters();

  explicit AdcircNodalParameters(std::string filename,
                                 AdcircMesh *mesh = nullptr);
  explicit AdcircNodalParameters(const char *filename,
                                 AdcircMesh *mesh = nullptr);

  int read();

  void setFilename(std::string filename);
  void setFilename(const char *filename);
  std::string filename();

  void setMesh(AdcircMesh *mesh);
  AdcircMesh *mesh();

  int write(std::string outputFilename);
  int write(const char *filename);

  int locateAttribute(std::string attributeName);
  int locateAttribute(const char *attributeName);

  std::string header() const;
  void setHeader(const std::string &title);

  int numParameters() const;
  void setNumParameters(int numParameters);

  int numNodes() const;
  void setNumNodes(int numNodes);

  AdcircNodalAttribute *nodalAttribute(int parameter, int node);
  AdcircNodalAttribute *nodalAttribute(std::string parameter, int node);
  AdcircNodalAttribute *nodalAttribute(const char* parameter, int node);

private:
  int _readFort13Header(std::fstream &fid);
  int _readFort13Defaults(std::fstream &fid);
  int _readFort13Body(std::fstream &fid);
  void _fillDefaultValues();
  void _mapNodes();

  /// Mapping function between the name of a nodal parameter and its position in
  /// the nodalParameters vector
  std::unordered_map<std::string, int> m_attributeLocations;

  /// Filename of the file that will be read or was read
  std::string m_filename;

  /// Title/header found at the beginning of the nodal attributes file
  std::string m_header;

  /// Number of nodal attributes found within this file
  int m_numParameters;

  /// Number of nodes in the mesh that this nodal attribute files references
  int m_numNodes;

  /// Underlying adcirc_mesh object (if necessary)
  AdcircMesh *m_mesh;

  /// Vector of objects containing the nodal parameters read from the file
  std::vector<AdcircNodalAttributeMetadata> m_nodalParameters;

  /// Vector of objects for the nodal parameters
  std::vector<std::vector<AdcircNodalAttribute> > m_nodalData;
};

#endif // ADCIRCNODALPARAMETERS_H
