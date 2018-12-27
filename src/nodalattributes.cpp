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
#include "pimpl/nodalattributes_impl.h"

namespace Adcirc {
namespace ModelParameters {

NodalAttributes::NodalAttributes() : m_impl(new NodalAttributesImpl()) {}

NodalAttributes::NodalAttributes(const std::string &filename,
                                 Adcirc::Geometry::Mesh *mesh)
    : m_impl(new NodalAttributesImpl(filename, mesh)) {}

NodalAttributes::~NodalAttributes() { delete this->m_impl; }

void NodalAttributes::read() { this->m_impl->read(); }

void NodalAttributes::setFilename(const std::string &filename) {
  this->m_impl->setFilename(filename);
}

std::string NodalAttributes::filename() {
  return std::move(this->m_impl->filename());
}

void NodalAttributes::setMesh(Adcirc::Geometry::Mesh *mesh) {
  this->m_impl->setMesh(mesh);
}

Adcirc::Geometry::Mesh *NodalAttributes::mesh() { return this->m_impl->mesh(); }

void NodalAttributes::write(const std::string &outputFilename) {
  this->m_impl->write(outputFilename);
}

std::string NodalAttributes::attributeNames(size_t index) {
  return std::move(this->m_impl->attributeNames(index));
}

size_t NodalAttributes::locateAttribute(const std::string &attributeName) {
  return this->m_impl->locateAttribute(attributeName);
}

std::string NodalAttributes::header() const {
  return std::move(this->m_impl->header());
}

void NodalAttributes::setHeader(const std::string &title) {
  this->m_impl->setHeader(title);
}

size_t NodalAttributes::numParameters() const {
  return this->m_impl->numParameters();
}

void NodalAttributes::setNumParameters(size_t numParameters) {
  this->m_impl->setNumParameters(numParameters);
}

size_t NodalAttributes::numNodes() const { return this->m_impl->numNodes(); }

void NodalAttributes::setNumNodes(size_t numNodes) {
  this->m_impl->setNumNodes(numNodes);
}

Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(size_t parameter,
                                                               size_t node) {
  return this->m_impl->attribute(parameter, node);
}

Adcirc::ModelParameters::Attribute *NodalAttributes::attribute(
    const std::string &name, size_t node) {
  return this->m_impl->attribute(name, node);
}

Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    size_t parameter) {
  return this->m_impl->metadata(parameter);
}

Adcirc::ModelParameters::AttributeMetadata *NodalAttributes::metadata(
    const std::string &name) {
  return this->m_impl->metadata(name);
}

}  // namespace ModelParameters
}  // namespace Adcirc
