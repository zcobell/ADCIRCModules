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
#include "nodalattributes.h"

#include "nodalattributes_private.h"

namespace Adcirc {
namespace ModelParameters {

/**
 * @brief Default constructor
 */
NodalAttributes::NodalAttributes()
    : m_impl(new Adcirc::Private::NodalAttributesPrivate()) {}

/**
 * @brief Constructor with filename and mesh
 * @param[in] filename name of nodal attributes file
 * @param[in] mesh optional mesh object to reference the nodal attributes to
 */
NodalAttributes::NodalAttributes(const std::string &filename,
                                 Adcirc::Geometry::Mesh *mesh)
    : m_impl(new Adcirc::Private::NodalAttributesPrivate(filename, mesh)) {}

/**
 * @brief Reads the nodal attributes file
 */
void NodalAttributes::read() { this->m_impl->read(); }

/**
 * @brief NodalAttributes::setFilename
 * @param[in] filename sets the filename of the nodal attributes file to be read
 */
void NodalAttributes::setFilename(const std::string &filename) {
  this->m_impl->setFilename(filename);
}

/**
 * @brief Returns the filename currently being used
 * @return name of file that has been or will be read
 */
std::string NodalAttributes::filename() { return this->m_impl->filename(); }

/**
 * @brief Sets a pointer to the mesh currently being used
 * @param[in] mesh mesh pointer
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
 * @param[in] outputFilename name of ouptut file
 */
void NodalAttributes::write(const std::string &outputFilename) {
  this->m_impl->write(outputFilename);
}

/**
 * @brief Returns the name of the nodal attribute at the specified index
 * @param[in] index position for the requested attribute
 * @return name of requested attribute
 */
std::string NodalAttributes::attributeNames(size_t index) {
  return this->m_impl->attributeNames(index);
}

/**
 * @brief Locates the position of the nodal attribute in the object by name
 * @param[in] attributeName name to search for
 * @return position of the nodal attribute in the object
 */
size_t NodalAttributes::locateAttribute(const std::string &attributeName) {
  return this->m_impl->locateAttribute(attributeName);
}

/**
 * @brief Header of the nodal attribute file
 * @return header string
 */
std::string NodalAttributes::header() const { return this->m_impl->header(); }

/**
 * @brief Sets the header string in the nodal attribute object
 * @param[in] title header string to put into the object
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
 * @param[in] numParameters number of parameters in the nodal attributes file
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
 * @param[in] numNodes number of nodes
 *
 * Note: No internal array resizing is done
 */
void NodalAttributes::setNumNodes(size_t numNodes) {
  this->m_impl->setNumNodes(numNodes);
}

/**
 * @brief Returns a pointer to the nodal attribute object for a specified node
 * @param[in] parameter index where the parameter is located
 * @param[in] node node to return the data for
 * @return NodalAttributes object
 */
Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(size_t parameter,
                                                               size_t node) {
  return this->m_impl->attribute(parameter, node);
}

/**
 * @brief Returns a pointer to the nodal attribute object for a specified node
 * @param[in] name name of the nodal parameter to return data for
 * @param[in] node node to return the data for
 * @return Attribute object
 */
Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(
    const std::string &name, size_t node) {
  return this->m_impl->attribute(name, node);
}

/**
 * @brief Returns the nodal attribute metadata for the given index
 * @param[in] parameter index for the parameter to return
 * @return AttributeMetadata object
 */
Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    size_t parameter) {
  return this->m_impl->metadata(parameter);
}

/**
 * @brief Returns the nodal attribute metadata for a given name
 * @param[in] name nodal attribute name
 * @return AttributeMetadata object
 */
Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    const std::string &name) {
  return this->m_impl->metadata(name);
}

/**
 * @brief Adds a new set of nodal attributes to the object
 * @param[in] metadata object metadata
 * @param[in] data object data
 */
void NodalAttributes::addAttribute(AttributeMetadata &metadata,
                                   std::vector<Attribute> &data) {
  this->m_impl->addAttribute(metadata, data);
}

void NodalAttributes::setAttributeData( size_t parameter, const std::vector<double> &values ){
  this->m_impl->setAttributeData(parameter,values);
}

void NodalAttributes::setAttributeData( size_t parameter, const std::vector<std::vector<double>> &values ){
  this->m_impl->setAttributeData(parameter,values);
}

}  // namespace ModelParameters
}  // namespace Adcirc
