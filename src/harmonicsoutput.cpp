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
#include "harmonicsoutput_impl.h"

namespace Adcirc {
namespace Harmonics {

/**
 * @param filename name of the harmonics file to read
 * @param velocity true of the file contains velocity information, false if it
 * does not
 */
HarmonicsOutput::HarmonicsOutput(const std::string& filename)
    : m_impl(new HarmonicsOutputImpl(filename)) {}

/**
 * @brief Read the harmonics file
 */
void HarmonicsOutput::read() { this->m_impl->read(); }

/**
 * @brief Write the harmonics file
 * @param filename name of output file
 * @param format type of output to write (ascii or netcdf)
 */
void HarmonicsOutput::write(const std::string& filename,
                            const Adcirc::Harmonics::HarmonicsFormat& format) {
  this->m_impl->write(filename, format);
}

/**
 * @brief Get the filename for the current harmonics file
 * @return filename
 */
std::string HarmonicsOutput::filename() const {
  return this->m_impl->filename();
}

/**
 * @brief Sets the filename to be used
 * @param filename name of file to use
 */
void HarmonicsOutput::setFilename(const std::string& filename) {
  this->m_impl->setFilename(filename);
}

/**
 * @brief Returns the index for the specified harmonic constituent
 * @param name constituent name to search for
 * @return index of the constituent in the data
 */
size_t HarmonicsOutput::index(const std::string& name) {
  return this->m_impl->index(name);
}

/**
 * @brief Returns the name of the harmonic constituent at the specified index
 * @param index position for which to return the constituent name
 * @return name of harmonic constituent
 */
std::string HarmonicsOutput::name(size_t index) {
  return this->m_impl->name(index);
}

/**
 * @brief Returns the amplitude for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing amplitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::amplitude(
    const std::string& name) {
  return this->m_impl->amplitude(name);
}

/**
 * @brief Returns the amplitude for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing amplitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::amplitude(size_t index) {
  return this->m_impl->amplitude(index);
}

/**
 * @brief Returns the phase for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::phase(
    const std::string& name) {
  return this->m_impl->phase(name);
}

/**
 * @brief Returns the phase for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::phase(size_t index) {
  return this->m_impl->phase(index);
}

/**
 * @brief Returns the u-magnitude for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing u-magnitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_amplitude(
    const std::string& name) {
  return this->m_impl->u_amplitude(name);
}

/**
 * @brief Returns the u-magnitude for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing u-magnitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_amplitude(size_t index) {
  return this->m_impl->u_amplitude(index);
}

/**
 * @brief Returns the u-phase for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing u-phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_phase(
    const std::string& name) {
  return this->m_impl->u_phase(name);
}

/**
 * @brief Returns the u-phase for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing u-phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::u_phase(size_t index) {
  return this->m_impl->u_phase(index);
}

/**
 * @brief Returns the v-magnitude for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing v-magnitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_amplitude(
    const std::string& name) {
  return this->m_impl->v_amplitude(name);
}

/**
 * @brief Returns the v-magnitude for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing v-magnitude
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_amplitude(size_t index) {
  return this->m_impl->v_amplitude(index);
}

/**
 * @brief Returns the v-phase for a specified harmonic constituent
 * @param name constituent to get the record for
 * @return HarmonicsRecord object containing v-phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_phase(
    const std::string& name) {
  return this->m_impl->v_phase(name);
}

/**
 * @brief Returns the v-phase for a specified harmonic constituent
 * @param index constituent to get the record for
 * @return HarmonicsRecord object containing v-phase
 */
Adcirc::Harmonics::HarmonicsRecord* HarmonicsOutput::v_phase(size_t index) {
  return this->m_impl->v_phase(index);
}

/**
 * @brief Returns the number of constituents in the file
 * @return number of constituents
 */
size_t HarmonicsOutput::numConstituents() const {
  return this->m_impl->numConstituents();
}

/**
 * @brief Sets the number of constituents in the file
 * @param numConstituents number of constituents to reallocate for
 */
void HarmonicsOutput::setNumConstituents(const size_t& numConstituents) {
  this->m_impl->setNumConstituents(numConstituents);
}

/**
 * @brief Returns the number of nodes in the file
 * @return number of nodes
 */
size_t HarmonicsOutput::numNodes() const { return this->m_impl->numNodes(); }

/**
 * @brief Sets the number of nodes in the file
 * @param numNodes number of nodes
 */
void HarmonicsOutput::setNumNodes(const size_t& numNodes) {
  this->m_impl->setNumNodes(numNodes);
}

/**
 * @brief Returns the type data within the file, scalar (water levels) or
 * velocities
 * @return true if velocity, false if scalar
 */
bool HarmonicsOutput::isVelocity() const { return this->m_impl->isVelocity(); }

/**
 * @brief Returns the array position for a specified node id
 * @param id node id
 * @return array position
 */
size_t HarmonicsOutput::nodeIdToArrayIndex(size_t id) {
  return this->m_impl->nodeIdToArrayIndex(id);
}

/**
 * @brief Returns the file type
 * @return file type
 */
int HarmonicsOutput::filetype() const { return this->m_impl->filetype(); }

}  // namespace Harmonics
}  // namespace Adcirc
