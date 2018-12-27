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
#include "harmonicsoutput.h"
#include "pimpl/harmonicsoutput_impl.h"

namespace Adcirc {
namespace Harmonics {

HarmonicsOutput::HarmonicsOutput(const std::string& filename, bool velocity)
    : m_impl(new HarmonicsOutputImpl(filename, velocity)) {}

HarmonicsOutput::~HarmonicsOutput() { delete this->m_impl; }

void HarmonicsOutput::read() { this->m_impl->read(); }

void HarmonicsOutput::write(const std::string& filename) {
  this->m_impl->write(filename);
}

std::string HarmonicsOutput::filename() const {
  return std::move(this->m_impl->filename());
}

void HarmonicsOutput::setFilename(const std::string& filename) {
  this->m_impl->setFilename(filename);
}

size_t HarmonicsOutput::index(const std::string& name) {
  return this->m_impl->index(name);
}

std::string HarmonicsOutput::name(size_t index) {
  return std::move(this->m_impl->name(index));
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::amplitude(
    const std::string& name) {
  return this->m_impl->amplitude(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::amplitude(size_t index) {
  return this->m_impl->amplitude(index);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::phase(
    const std::string& name) {
  return this->m_impl->phase(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::phase(size_t index) {
  return this->m_impl->phase(index);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_magnitude(
    const std::string& name) {
  return this->m_impl->u_magnitude(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_magnitude(size_t index) {
  return this->m_impl->u_magnitude(index);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_phase(
    const std::string& name) {
  return this->m_impl->u_phase(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_phase(size_t index) {
  return this->m_impl->u_phase(index);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_magnitude(
    const std::string& name) {
  return this->m_impl->v_magnitude(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_magnitude(size_t index) {
  return this->m_impl->v_magnitude(index);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_phase(
    const std::string& name) {
  return this->m_impl->v_phase(name);
}

Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_phase(size_t index) {
  return this->m_impl->v_phase(index);
}

size_t HarmonicsOutput::numConstituents() const {
  return this->m_impl->numConstituents();
}

void HarmonicsOutput::setNumConstituents(const size_t& numConstituents) {
  this->m_impl->setNumConstituents(numConstituents);
}

size_t HarmonicsOutput::numNodes() const { return this->m_impl->numNodes(); }

void HarmonicsOutput::setNumNodes(const size_t& numNodes) {
  this->m_impl->setNumNodes(numNodes);
}

bool HarmonicsOutput::isVelocity() const { return this->m_impl->isVelocity(); }

size_t HarmonicsOutput::nodeIdToArrayIndex(size_t id) {
  return this->m_impl->nodeIdToArrayIndex(id);
}

int HarmonicsOutput::filetype() const { return this->m_impl->filetype(); }

}  // namespace Harmonics
}  // namespace Adcirc
