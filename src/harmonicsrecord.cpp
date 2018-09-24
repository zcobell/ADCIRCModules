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
#include <cassert>
#include "error.h"

using namespace std;
using namespace Adcirc::Output;

HarmonicsRecord::HarmonicsRecord() : m_name("none") {
  this->m_numNodes = 0;
  this->m_frequency = 0.0;
  this->m_equilibriumArg = 0.0;
  this->m_nodalFactor = 0.0;
}

void HarmonicsRecord::resize(size_t numNodes) {
  this->m_numNodes = numNodes;
  this->m_data.resize(numNodes);
}

double HarmonicsRecord::frequency() const { return this->m_frequency; }

void HarmonicsRecord::setFrequency(double frequency) {
  this->m_frequency = frequency;
}

double HarmonicsRecord::nodalFactor() const { return this->m_nodalFactor; }

void HarmonicsRecord::setNodalFactor(double nodalFactor) {
  this->m_nodalFactor = nodalFactor;
}

double HarmonicsRecord::equilibriumArg() const {
  return this->m_equilibriumArg;
}

void HarmonicsRecord::setEquilibriumArg(double equilibriumArg) {
  this->m_equilibriumArg = equilibriumArg;
}

string HarmonicsRecord::name() const { return this->m_name; }

void HarmonicsRecord::setName(const string &name) { this->m_name = name; }

vector<double> HarmonicsRecord::values() { return this->m_data; }

double HarmonicsRecord::value(size_t index) {
  assert(index < this->m_data.size());
  if (index < this->m_data.size()) {
    return this->m_data[index];
  } else {
    adcircmodules_throw_exception("HarmonicsRecord: Index out of bounds");
    return -9999;
  }
}

void HarmonicsRecord::set(size_t index, double data) {
  assert(index < this->m_data.size());
  if (index < this->m_data.size()) {
    this->m_data[index] = data;
  } else {
    adcircmodules_throw_exception("HarmonicsRecord: Index out of bounds");
    return;
  }
  return;
}

void HarmonicsRecord::set(const std::vector<double> &value) {
  assert(value.size() == this->m_data.size());
  if (value.size() != this->m_data.size()) {
    adcircmodules_throw_exception("HarmonicsRecord: Invalid data size");
    return;
  }
  this->m_data = value;
  return;
}
