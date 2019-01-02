#ifndef NODALATTRIBUTES_H
#define NODALATTRIBUTES_H

#include <string>
#include <vector>
#include "adcircmodules_global.h"
#include "attributemetadata.h"
#include "mesh.h"

class NodalAttributesImpl;

namespace Adcirc {
namespace ModelParameters {

/**
 * @class NodalAttributes
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The NodalAttributes class handles the reading and writing of Nodal
 * Attributes (i.e. fort.13) files
 *
 */

class NodalAttributes {
 public:
  ADCIRCMODULES_EXPORT NodalAttributes();

  ADCIRCMODULES_EXPORT NodalAttributes(const std::string &filename,
                                       Adcirc::Geometry::Mesh *mesh = nullptr);

  ADCIRCMODULES_EXPORT ~NodalAttributes();

  void ADCIRCMODULES_EXPORT read();

  void ADCIRCMODULES_EXPORT setFilename(const std::string &filename);
  std::string ADCIRCMODULES_EXPORT filename();

  void ADCIRCMODULES_EXPORT setMesh(Adcirc::Geometry::Mesh *mesh);
  Adcirc::Geometry::Mesh ADCIRCMODULES_EXPORT *mesh();

  void ADCIRCMODULES_EXPORT write(const std::string &outputFilename);

  std::string ADCIRCMODULES_EXPORT attributeNames(size_t index);

  size_t ADCIRCMODULES_EXPORT locateAttribute(const std::string &attributeName);

  std::string ADCIRCMODULES_EXPORT header() const;
  void ADCIRCMODULES_EXPORT setHeader(const std::string &title);

  size_t ADCIRCMODULES_EXPORT numParameters() const;
  void ADCIRCMODULES_EXPORT setNumParameters(size_t numParameters);

  size_t ADCIRCMODULES_EXPORT numNodes() const;
  void ADCIRCMODULES_EXPORT setNumNodes(size_t numNodes);

  Adcirc::ModelParameters::Attribute ADCIRCMODULES_EXPORT *attribute(
      size_t parameter, size_t node);
  Adcirc::ModelParameters::Attribute ADCIRCMODULES_EXPORT *attribute(
      const std::string &name, size_t node);

  Adcirc::ModelParameters::AttributeMetadata ADCIRCMODULES_EXPORT *metadata(
      size_t parameter);
  Adcirc::ModelParameters::AttributeMetadata ADCIRCMODULES_EXPORT *metadata(
      const std::string &name);

 private:
  NodalAttributesImpl *m_impl;
};
}  // namespace ModelParameters
}  // namespace Adcirc
#endif  // NODALATTRIBUTES_H
