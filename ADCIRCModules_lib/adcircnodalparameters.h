#ifndef ADCIRCNODALPARAMETERS_H
#define ADCIRCNODALPARAMETERS_H

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

  int write(std::string outputFilename);
  int write(const char *filename);

  int locateAttribute(std::string attributeName);
  int locateAttribute(const char *attributeName);

private:
  int readNodalAttributesFile();

  int writeNodalAttributesFile(std::string outputFilename,
                               bool userSpecifiedDefaultValues);

  std::vector<std::string> writeNodalParameter(int index);

  int getNumNonDefault(AdcircNodalParameters *nodalParam,
                       std::vector<AdcircNodalAttribute *> nodalAtt);

  /// Mapping function between the name of a nodal parameter and its position in
  /// the nodalParameters vector
  std::unordered_map<std::string, int> m_attributeLocations;

  /// Filename of the file that will be read or was read
  std::string m_filename;

  /// Title/header found at the beginning of the nodal attributes file
  std::string m_title;

  /// Number of nodal attributes found within this file
  int m_numParameters;

  /// Number of nodes in the mesh that this nodal attribute files references
  int m_numNodes;

  /// Underlying adcirc_mesh object (if necessary)
  AdcircMesh *m_mesh;

  /// Vector of objects containing the nodal parameters read from the file
  std::vector<AdcircNodalAttributeMetadata *> m_nodalParameters;

  /// Vector of objects for the nodal parameters
  std::vector<std::vector<AdcircNodalAttribute *>> m_nodalData;
};

#endif // ADCIRCNODALPARAMETERS_H
