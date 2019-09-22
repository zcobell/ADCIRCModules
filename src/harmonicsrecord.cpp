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
#include "harmonicsrecord.h"
#include "harmonicsrecord_private.h"

namespace Adcirc {
namespace Harmonics {

HarmonicsRecord::HarmonicsRecord() : m_impl(new Adcirc::Private::HarmonicsRecordPrivate()) {}

HarmonicsRecord::~HarmonicsRecord() { delete this->m_impl; }

/**
 * @brief Returns the name of the harmonic constituent in this object
 * @return name of harmonic constituent
 */
std::string HarmonicsRecord::name() const { return this->m_impl->name(); }

/**
 * @brief Sets the name of the harmonic constiuent in this object
 * @param name name
 */
void HarmonicsRecord::setName(const std::string& name) {
  this->m_impl->setName(name);
}

/**
 * @brief Returns the value of this record at the specified index
 * @param index array index
 * @return value at array index
 */
double HarmonicsRecord::value(size_t index) {
  return this->m_impl->value(index);
}

/**
 * @brief Returns a vector of all values in this object
 * @return vector containing all values in this object
 */
std::vector<double> HarmonicsRecord::values() { return this->m_impl->values(); }

/**
 * @brief Sets the value at the specified index to a value
 * @param index position to set
 * @param value value to set
 */
void HarmonicsRecord::set(size_t index, double value) {
  this->m_impl->set(index, value);
}

/**
 * @brief Sets values using a vector of values
 * @param value vector containing the values to set for this record
 *
 * Note that the vector must be the same size as the number of nodes in this
 * record
 *
 */
void HarmonicsRecord::set(const std::vector<double>& value) {
  this->m_impl->set(value);
}

/**
 * @brief Resizes this object to a specified size
 * @param numNodes new size for this object
 *
 * Note: All data will be destroyed
 */
void HarmonicsRecord::resize(size_t numNodes) {
  this->m_impl->resize(numNodes);
}

/**
 * @brief Returns the frequency for this harmonic constituent
 * @return frequency
 */
double HarmonicsRecord::frequency() const { return this->m_impl->frequency(); }

/**
 * @brief Sets the frequency for this harmonic constituent
 * @param frequency frequency for this harmonic constituent
 */
void HarmonicsRecord::setFrequency(double frequency) {
  this->m_impl->setFrequency(frequency);
}

/**
 * @brief Returns the nodal factor for this harmonic constituent
 * @return nodal factor
 */
double HarmonicsRecord::nodalFactor() const {
  return this->m_impl->nodalFactor();
}

/**
 * @brief Sets the nodal factor for this harmonic constituent
 * @param frequency nodal factor for this harmonic constituent
 */
void HarmonicsRecord::setNodalFactor(double nodalFactor) {
  this->m_impl->setNodalFactor(nodalFactor);
}

/**
 * @brief Returns the equilibrium argument for this harmonic constituent
 * @return equilibrium argument
 */
double HarmonicsRecord::equilibriumArg() const {
  return this->m_impl->equilibriumArg();
}

/**
 * @brief Sets the equilibrium argument for this harmonic constituent
 * @param frequency equilibrium argument for this harmonic constituent
 */
void HarmonicsRecord::setEquilibriumArg(double equilibriumArg) {
  this->m_impl->setEquilibriumArg(equilibriumArg);
}

}  // namespace Harmonics
}  // namespace Adcirc
