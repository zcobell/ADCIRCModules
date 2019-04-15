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
#include <cassert>
#include <cmath>
#include "constants.h"
#include "logging.h"

using namespace Adcirc::Output;

OutputRecord::OutputRecord()
    : m_record(0),
      m_isVector(false),
      m_time(0),
      m_iteration(0),
      m_defaultValue(Adcirc::Output::defaultOutputValue()),
      m_numNodes(0) {}

OutputRecord::OutputRecord(size_t record, size_t numNodes, bool isVector)
    : m_record(record),
      m_isVector(isVector),
      m_time(0),
      m_numNodes(numNodes),
      m_iteration(0),
      m_defaultValue(Adcirc::Output::defaultOutputValue()) {
  assert(numNodes != 0);
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
  if (index < this->numNodes() && this->isVector()) {
    this->m_u[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: setU invalid input");
  }
  return;
}

void OutputRecord::setV(size_t index, double value) {
  assert(this->isVector());
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->isVector()) {
    this->m_v[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: setV invalid input");
  }
  return;
}

void OutputRecord::set(size_t index, double value) {
  assert(!this->isVector());
  assert(index < this->numNodes());
  if (index < this->numNodes() && !this->isVector()) {
    this->m_u[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: set invalid input");
  }
  return;
}

void OutputRecord::set(size_t index, double value_u, double value_v) {
  assert(this->isVector());
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->isVector()) {
    this->m_u[index] = value_u;
    this->m_v[index] = value_v;
  } else {
    adcircmodules_throw_exception("OutputRecord: set invalid input");
  }
}

double OutputRecord::z(size_t index) {
  assert(index < this->numNodes());
  if (index < this->numNodes()) {
    if (this->isVector()) {
      adcircmodules_throw_exception("OutputRecord: Datatype is a vector");
      return 0.0;
    } else {
      return this->m_u[index];
    }
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::magnitude(size_t index) {
  assert(index < this->numNodes());
  assert(this->isVector());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    return pow(pow(this->m_u[index], 2.0) + pow(this->m_v[index], 2.0), 0.5);
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::angle(double x, double y, AngleUnits units) {
  double a = atan2(y, x);
  if (units == AngleUnits::Degrees) {
    a = a * 180.0 / Constants::pi();
    if (a >= 360.0) {
      a = a - 360.0;
    } else if (a < 0.0) {
      a = a + 360.0;
    }
  } else if (units == AngleUnits::Radians) {
    if (a >= Constants::twoPi()) {
      a = a - Constants::twoPi();
    } else if (a < 0.0) {
      a = a + Constants::twoPi();
    }
  } else {
    adcircmodules_throw_exception("Invalid angle units");
    return 0.0;
  }
  return a;
}

double OutputRecord::direction(size_t index, AngleUnits angleType) {
  assert(this->isVector());
  assert(index < this->numNodes());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    return this->angle(this->m_u[index], this->m_v[index], angleType);
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::u(size_t index) {
  assert(this->isVector());
  assert(index < this->numNodes());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    return this->m_u[index];
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::v(size_t index) {
  assert(this->isVector());
  assert(index < this->numNodes());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    return this->m_v[index];
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

void OutputRecord::setAllU(const std::vector<double>& values) {
  assert(this->isVector());
  assert(this->m_numNodes == values.size());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return;
  }
  if (values.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
  this->m_u = values;
  return;
}

void OutputRecord::setAllV(const std::vector<double>& values) {
  assert(this->isVector());
  assert(this->m_numNodes == values.size());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return;
  }
  if (values.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
  this->m_v = values;
  return;
}

void OutputRecord::setAll(const std::vector<double>& values_u,
                          const std::vector<double>& values_v) {
  assert(this->isVector());
  assert(this->m_numNodes == values_u.size());
  assert(this->m_numNodes == values_v.size());
  if (!this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return;
  }
  if (values_u.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
  if (values_v.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
  this->m_u = values_u;
  this->m_v = values_v;
  return;
}

void OutputRecord::setAll(const std::vector<double>& values) {
  assert(!this->isVector());
  assert(this->m_numNodes == values.size());
  if (this->isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is a vector");
    return;
  }
  if (values.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
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
      adcircmodules_throw_exception("OutputRecord: Invalid column specified");
      return std::vector<double>();
    }
  } else {
    return this->m_u;
  }
}

void OutputRecord::setAll(size_t size, const double* values) {
  assert(size == this->m_numNodes);
  if (this->m_numNodes != size) {
    adcircmodules_throw_exception("OutputRecord: Array size mismatch");
    return;
  }
  this->m_u = std::vector<double>(values, values + size);
  return;
}

void OutputRecord::setAll(size_t size, const double* values_u,
                          const double* values_v) {
  assert(size == this->m_numNodes);
  if (this->m_numNodes != size) {
    adcircmodules_throw_exception("OutputRecord: Array size mismatch");
    return;
  }
  this->m_u = std::vector<double>(values_u, values_u + size);
  this->m_v = std::vector<double>(values_v, values_v + size);
  return;
}

std::vector<double> OutputRecord::magnitudes() {
  assert(this->isVector());
  if (!this->isVector()) {
    adcircmodules_throw_exception(
        "OutputRecord: Must be a vector to compute magnitude");
    return std::vector<double>();
  }
  std::vector<double> m;
  m.resize(this->m_numNodes);
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    m[i] = pow(pow(this->m_u[i], 2.0) + pow(this->m_v[i], 2.0), 0.5);
  }
  return m;
}

std::vector<double> OutputRecord::directions(AngleUnits angleType) {
  assert(this->isVector());
  if (!this->isVector()) {
    adcircmodules_throw_exception(
        "OutputRecord: Must be a vector to compute direction");
    return std::vector<double>();
  }
  std::vector<double> d;
  d.resize(this->m_numNodes);
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    d[i] = this->angle(this->m_u[i], this->m_v[i], angleType);
  }
  return d;
}
