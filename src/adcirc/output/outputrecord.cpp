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
#include "outputrecord.h"
#include <assert.h>
#include <math.h>
#include "adcirc/architecture/error.h"

using namespace Adcirc::Output;

OutputRecord::OutputRecord(size_t record, size_t numNodes, bool isVector) {
  assert(numNodes != 0);

  this->m_record = record;
  this->m_isVector = isVector;
  this->m_time = 0;
  this->m_iteration = 0;
  this->m_numNodes = numNodes;
  this->m_defaultValue = Adcirc::Output::DefaultOutputValue;

  if (this->isVector()) {
    this->m_u.resize(this->numNodes());
    this->m_v.resize(this->numNodes());
  } else {
    this->m_u.resize(this->numNodes());
  }
}

long long OutputRecord::iteration() const { return this->m_iteration; }

void OutputRecord::setIteration(long long iteration) {
  this->m_iteration = iteration;
}

void OutputRecord::fill(double value) {
  std::fill(this->m_u.begin(), this->m_u.end(), value);
  if (this->m_isVector) {
    std::fill(this->m_v.begin(), this->m_v.end(), value);
  }
  return;
}

double OutputRecord::time() const { return this->m_time; }

void OutputRecord::setTime(double time) { this->m_time = time; }

size_t OutputRecord::numNodes() const { return this->m_numNodes; }

void OutputRecord::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

bool OutputRecord::isVector() const { return this->m_isVector; }

void OutputRecord::setIsVector(bool isVector) { this->m_isVector = isVector; }

double OutputRecord::defaultValue() const { return this->m_defaultValue; }

void OutputRecord::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

size_t OutputRecord::record() const { return this->m_record; }

void OutputRecord::setRecord(size_t record) { this->m_record = record; }

void OutputRecord::setU(size_t index, double value) {
  assert(this->isVector());
  assert(index < this->numNodes());
  this->m_u[index] = value;
  return;
}

void OutputRecord::setV(size_t index, double value) {
  assert(this->isVector());
  assert(index < this->numNodes());
  this->m_v[index] = value;
  return;
}

void OutputRecord::set(size_t index, double value) {
  assert(!this->isVector());
  assert(index < this->numNodes());
  this->m_u[index] = value;
  return;
}

void OutputRecord::set(size_t index, double value_u, double value_v) {
  assert(this->isVector());
  assert(index < this->numNodes());
  this->m_u[index] = value_u;
  this->m_v[index] = value_v;
}

double OutputRecord::value(size_t index) {
  assert(index < this->numNodes());
  if (this->isVector())
    return pow(this->m_u[index], 2.0) + pow(this->m_v[index], 2.0);
  else
    return this->m_u[index];
}

double OutputRecord::direction(size_t index) {
  assert(this->isVector());
  assert(index < this->numNodes());
  return atan2(this->m_v[index], this->m_u[index]);
}

double OutputRecord::u(size_t index) {
  assert(this->isVector());
  assert(index < this->numNodes());
  return this->m_u[index];
}

double OutputRecord::v(size_t index) {
  assert(this->isVector());
  assert(index < this->numNodes());
  return this->m_v[index];
}

void OutputRecord::setAllU(std::vector<double> values) {
  assert(this->isVector());
  assert(this->m_numNodes == values.size());
  if (!this->isVector()) return;
  if (values.size() != this->m_numNodes) return;
  this->m_u = values;
  return;
}

void OutputRecord::setAllV(std::vector<double> values) {
  assert(this->isVector());
  assert(this->m_numNodes == values.size());
  if (!this->isVector()) return;
  if (values.size() != this->m_numNodes) return;
  this->m_v = values;
  return;
}

void OutputRecord::setAll(std::vector<double> values_u,
                          std::vector<double> values_v) {
  assert(this->isVector());
  assert(this->m_numNodes == values_u.size());
  assert(this->m_numNodes == values_v.size());
  if (!this->isVector()) return;
  if (values_u.size() != this->m_numNodes) return;
  if (values_v.size() != this->m_numNodes) return;
  this->m_u = values_u;
  this->m_v = values_v;
  return;
}

void OutputRecord::setAll(std::vector<double> values) {
  assert(!this->isVector());
  assert(this->m_numNodes == values.size());
  if (!this->isVector()) return;
  if (values.size() != this->m_numNodes) return;
  this->m_u = values;
  return;
}

std::vector<double> OutputRecord::values(size_t column) {
  if (this->isVector()) {
    if (column == 0) {
      return this->m_u;
    } else if (column == 1) {
      return this->m_v;
    } else {
      Adcirc::Error::throwError("Invalid column specified");
      return std::vector<double>();
    }
  } else {
    return this->m_u;
  }
}
