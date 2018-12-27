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
#include "harmonicsrecord.h"
#include "pimpl/harmonicsrecord_impl.h"

namespace Adcirc {
namespace Harmonics {

HarmonicsRecord::HarmonicsRecord() : m_impl(new HarmonicsRecordImpl()) {}

HarmonicsRecord::~HarmonicsRecord() { delete this->m_impl; }

std::string HarmonicsRecord::name() const {
  return std::move(this->m_impl->name());
}
void HarmonicsRecord::setName(const std::string& name) {
  this->m_impl->setName(name);
}

double HarmonicsRecord::value(size_t index) {
  return this->m_impl->value(index);
}
std::vector<double> HarmonicsRecord::values() {
  return std::move(this->m_impl->values());
}

void HarmonicsRecord::set(size_t index, double value) {
  this->m_impl->set(index, value);
}

void HarmonicsRecord::set(const std::vector<double>& value) {
  this->m_impl->set(value);
}

void HarmonicsRecord::resize(size_t numNodes) {
  this->m_impl->resize(numNodes);
}

double HarmonicsRecord::frequency() const { return this->m_impl->frequency(); }

void HarmonicsRecord::setFrequency(double frequency) {
  this->m_impl->setFrequency(frequency);
}

double HarmonicsRecord::nodalFactor() const {
  return this->m_impl->nodalFactor();
}

void HarmonicsRecord::setNodalFactor(double nodalFactor) {
  this->m_impl->setNodalFactor(nodalFactor);
}

double HarmonicsRecord::equilibriumArg() const {
  return this->m_impl->equilibriumArg();
}

void HarmonicsRecord::setEquilibriumArg(double equilibriumArg) {
  this->m_impl->setEquilibriumArg(equilibriumArg);
}

}  // namespace Harmonics
}  // namespace Adcirc
