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
#include "nodalattributes_impl.h"

namespace Adcirc {
namespace ModelParameters {

/**
 * @brief Default constructor
 */
NodalAttributes::NodalAttributes() : m_impl(new NodalAttributesImpl()) {}

/**
 * @brief Constructor with filename and mesh
 * @param filename name of nodal attributes file
 * @param mesh optional mesh object to reference the nodal attributes to
 */
NodalAttributes::NodalAttributes(const std::string &filename,
                                 Adcirc::Geometry::Mesh *mesh)
    : m_impl(new NodalAttributesImpl(filename, mesh)) {}

/**
 * @brief Destructor
 */
NodalAttributes::~NodalAttributes() { delete this->m_impl; }

/**
 * @brief Reads the nodal attributes file
 */
void NodalAttributes::read() { this->m_impl->read(); }

/**
 * @brief NodalAttributes::setFilename
 * @param filename sets the filename of the nodal attributes file to be read
 */
void NodalAttributes::setFilename(const std::string &filename) {
  this->m_impl->setFilename(filename);
}

/**
 * @brief Returns the filename currently being used
 * @return name of file that has been or will be read
 */
std::string NodalAttributes::filename() {
  return this->m_impl->filename();
}

/**
 * @brief Sets a pointer to the mesh currently being used
 * @param mesh mesh pointer
 */
void NodalAttributes::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_impl->setMesh(mesh);
}

/**
 * @brief Returns the pointer to the mesh currently being used
 * @return mesh pointer
 */
Adcirc::Geometry::Mesh *NodalAttributes::mesh() { return this->m_impl->mesh(); }

/**
 * @brief Writes the current nodal attributes object to the specified filename
 * @param outputFilename name of ouptut file
 */
void NodalAttributes::write(const std::string &outputFilename) {
  this->m_impl->write(outputFilename);
}

/**
 * @brief Returns the name of the nodal attribute at the specified index
 * @param index position for the requested attribute
 * @return name of requested attribute
 */
std::string NodalAttributes::attributeNames(size_t index) {
  return this->m_impl->attributeNames(index);
}

/**
 * @brief Locates the position of the nodal attribute in the object by name
 * @param attributeName name to search for
 * @return position of the nodal attribute in the object
 */
size_t NodalAttributes::locateAttribute(const std::string &attributeName) {
  return this->m_impl->locateAttribute(attributeName);
}

/**
 * @brief Header of the nodal attribute file
 * @return header string
 */
std::string NodalAttributes::header() const {
  return this->m_impl->header();
}

/**
 * @brief Sets the header string in the nodal attribute object
 * @param title header string to put into the object
 */
void NodalAttributes::setHeader(const std::string &title) {
  this->m_impl->setHeader(title);
}

/**
 * @brief Returns the number of parameters in the nodal attributes file
 * @return number of attributes
 */
size_t NodalAttributes::numParameters() const {
  return this->m_impl->numParameters();
}

/**
 * @brief Sets the number of parameters in the nodal attributes file
 * @param numParameters number of parameters in the nodal attributes file
 *
 * Note: No internal array resizing is done
 */
void NodalAttributes::setNumParameters(size_t numParameters) {
  this->m_impl->setNumParameters(numParameters);
}

/**
 * @brief Returns the number of nodes in the nodal attributes file
 * @return number of nodes
 */
size_t NodalAttributes::numNodes() const { return this->m_impl->numNodes(); }

/**
 * @brief Sets the number of nodes in the nodal attributes file
 * @param numNodes number of nodes
 *
 * Note: No internal array resizing is done
 */
void NodalAttributes::setNumNodes(size_t numNodes) {
  this->m_impl->setNumNodes(numNodes);
}

/**
 * @brief Returns a pointer to the nodal attribute object for a specified node
 * @param parameter index where the parameter is located
 * @param node node to return the data for
 * @return NodalAttributes object
 */
Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(size_t parameter,
                                                               size_t node) {
  return this->m_impl->attribute(parameter, node);
}

/**
 * @brief Returns a pointer to the nodal attribute object for a specified node
 * @param name name of the nodal parameter to return data for
 * @param node node to return the data for
 * @return Attribute object
 */
Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(
    const std::string &name, size_t node) {
  return this->m_impl->attribute(name, node);
}

/**
 * @brief Returns the nodal attribute metadata for the given index
 * @param parameter index for the parameter to return
 * @return AttributeMetadata object
 */
Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    size_t parameter) {
  return this->m_impl->metadata(parameter);
}

/**
 * @brief Returns the nodal attribute metadata for a given name
 * @param name nodal attribute name
 * @return AttributeMetadata object
 */
Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    const std::string &name) {
  return this->m_impl->metadata(name);
}

}  // namespace ModelParameters
}  // namespace Adcirc
