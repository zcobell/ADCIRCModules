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
#include "harmonicsrecord_private.h"
#include <cassert>
#include "default_values.h"
#include "logging.h"

using namespace Adcirc::Private;

HarmonicsRecordPrivate::HarmonicsRecordPrivate()
    : m_name("none"),
      m_numNodes(0),
      m_frequency(0.0),
      m_equilibriumArg(0.0),
      m_nodalFactor(0.0) {}

void HarmonicsRecordPrivate::resize(size_t numNodes) {
  this->m_numNodes = numNodes;
  this->m_data.resize(numNodes);
}

double HarmonicsRecordPrivate::frequency() const { return this->m_frequency; }

void HarmonicsRecordPrivate::setFrequency(double frequency) {
  this->m_frequency = frequency;
}

double HarmonicsRecordPrivate::nodalFactor() const {
  return this->m_nodalFactor;
}

void HarmonicsRecordPrivate::setNodalFactor(double nodalFactor) {
  this->m_nodalFactor = nodalFactor;
}

double HarmonicsRecordPrivate::equilibriumArg() const {
  return this->m_equilibriumArg;
}

void HarmonicsRecordPrivate::setEquilibriumArg(double equilibriumArg) {
  this->m_equilibriumArg = equilibriumArg;
}

std::string HarmonicsRecordPrivate::name() const { return this->m_name; }

void HarmonicsRecordPrivate::setName(const std::string &name) {
  this->m_name = name;
}

std::vector<double> HarmonicsRecordPrivate::values() { return this->m_data; }

double HarmonicsRecordPrivate::value(size_t index) {
  assert(index < this->m_data.size());
  if (index < this->m_data.size()) {
    return this->m_data[index];
  } else {
    adcircmodules_throw_exception("HarmonicsRecord: Index out of bounds");
    return adcircmodules_default_value<double>();
  }
}

void HarmonicsRecordPrivate::set(size_t index, double data) {
  assert(index < this->m_data.size());
  if (index < this->m_data.size()) {
    this->m_data[index] = data;
  } else {
    adcircmodules_throw_exception("HarmonicsRecord: Index out of bounds");
    return;
  }
  return;
}

void HarmonicsRecordPrivate::set(const std::vector<double> &value) {
  assert(value.size() == this->m_data.size());
  if (value.size() != this->m_data.size()) {
    adcircmodules_throw_exception("HarmonicsRecord: Invalid data size");
    return;
  }
  this->m_data = value;
  return;
}
