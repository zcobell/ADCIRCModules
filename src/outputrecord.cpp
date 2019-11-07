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
#include "outputrecord.h"
#include <cassert>
#include <cmath>
#include "constants.h"
#include "fpcompare.h"
#include "logging.h"

using namespace Adcirc::Output;

OutputRecord::OutputRecord()
    : m_record(0),
      m_time(0),
      m_iteration(0),
      m_defaultValue(Adcirc::Output::defaultOutputValue()),
      m_numNodes(0),
      m_metadata(OutputMetadata()) {}

OutputRecord::OutputRecord(size_t record, size_t numNodes,
                           OutputMetadata& metadata)
    : m_record(record), m_numNodes(numNodes), m_metadata(metadata) {
  assert(numNodes != 0);
  this->allocate();
}

OutputRecord::OutputRecord(size_t record, size_t numNodes, bool isVector,
                           bool isMax, size_t dimension)
    : m_record(record),
      m_time(0),
      m_numNodes(numNodes),
      m_iteration(0),
      m_defaultValue(Adcirc::Output::defaultOutputValue()) {
  assert(numNodes != 0);
  if (dimension == 1) {
    this->m_metadata =
        OutputMetadata("none", "none", "none", "none",
                       OutputMetadata::defaultConvention(), isMax);
  } else if (dimension == 2) {
    this->m_metadata = OutputMetadata(
        "none", "none", "none", "none", OutputMetadata::defaultConvention(),
        "none", "none", "none", "none", OutputMetadata::defaultConvention(),
        isMax, isVector);
  } else if (dimension == 3) {
    this->m_metadata = OutputMetadata(
        "none", "none", "none", "none", OutputMetadata::defaultConvention(),
        "none", "none", "none", "none", OutputMetadata::defaultConvention(),
        "none", "none", "none", "none", OutputMetadata::defaultConvention(),
        isMax, isVector);
  }
  this->allocate();
}

void OutputRecord::allocate() {
  if (this->m_metadata.dimension() == 3) {
    this->m_u.resize(this->numNodes());
    this->m_v.resize(this->numNodes());
    this->m_w.resize(this->numNodes());
  } else if (this->m_metadata.dimension() == 2) {
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
  if (this->m_metadata.dimension() > 1) {
    std::fill(this->m_v.begin(), this->m_v.end(), value);
  }
  if (this->m_metadata.dimension() > 2) {
    std::fill(this->m_w.begin(), this->m_w.end(), value);
  }
  return;
}

double OutputRecord::time() const { return this->m_time; }

void OutputRecord::setTime(double time) { this->m_time = time; }

size_t OutputRecord::numNodes() const { return this->m_numNodes; }

void OutputRecord::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

double OutputRecord::defaultValue() const { return this->m_defaultValue; }

void OutputRecord::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

size_t OutputRecord::record() const { return this->m_record; }

void OutputRecord::setRecord(size_t record) { this->m_record = record; }

void OutputRecord::setU(size_t index, double value) {
  assert(this->m_metadata.dimension() > 1);
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->m_metadata.isVector()) {
    this->m_u[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: setU invalid input");
  }
  return;
}

void OutputRecord::setV(size_t index, double value) {
  assert(this->m_metadata.dimension() > 1);
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->m_metadata.isVector()) {
    this->m_v[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: setV invalid input");
  }
  return;
}

void OutputRecord::setW(size_t index, double value) {
  assert(this->m_metadata.dimension() > 2);
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->m_metadata.isVector()) {
    this->m_w[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: setW invalid input");
  }
  return;
}

void OutputRecord::set(size_t index, double value) {
  assert(this->m_metadata.dimension() == 1);
  assert(index < this->numNodes());
  if (index < this->numNodes() && !this->m_metadata.isVector()) {
    this->m_u[index] = value;
  } else {
    adcircmodules_throw_exception("OutputRecord: set invalid input");
  }
  return;
}

void OutputRecord::set(size_t index, double value_u, double value_v) {
  assert(this->m_metadata.dimension() == 2);
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->m_metadata.dimension() == 2) {
    this->m_u[index] = value_u;
    this->m_v[index] = value_v;
  } else {
    std::cout << this->m_metadata.dimension() << " " << this->numNodes() << " "
              << index << std::endl;
    std::cout.flush();
    adcircmodules_throw_exception("OutputRecord: set invalid input");
  }
}

void OutputRecord::set(size_t index, double value_u, double value_v,
                       double value_w) {
  assert(this->m_metadata.dimension() == 3);
  assert(index < this->numNodes());
  if (index < this->numNodes() && this->m_metadata.dimension() == 3) {
    this->m_u[index] = value_u;
    this->m_v[index] = value_v;
    this->m_w[index] = value_w;
  } else {
    adcircmodules_throw_exception("OutputRecord: set invalid input");
  }
}

double OutputRecord::z(size_t index) const {
  assert(index < this->numNodes());
  if (index < this->numNodes()) {
    return this->m_u[index];
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::magnitude(size_t index) const {
  assert(index < this->numNodes());
  assert(this->m_metadata.isVector());
  if (!this->m_metadata.isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    if (this->isDefault(index)) return this->defaultValue();
    if (this->m_metadata.dimension() == 2) {
      return pow(pow(this->m_u[index], 2.0) + pow(this->m_v[index], 2.0), 0.5);
    } else if (this->m_metadata.dimension() == 3) {
      return pow(pow(this->m_u[index], 2.0) + pow(this->m_v[index], 2.0) +
                     pow(this->m_w[index], 2.0),
                 0.5);
    } else {
      return 0.0;
    }
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::angle(double x, double y, AngleUnits units) const {
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

double OutputRecord::direction(size_t index, AngleUnits angleType) const {
  assert(this->m_metadata.isVector());
  assert(index < this->numNodes());
  if (!this->m_metadata.isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    if (this->isDefault(index)) return this->defaultValue();
    return this->angle(this->m_u[index], this->m_v[index], angleType);
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

double OutputRecord::u(size_t index) const {
  assert(this->m_metadata.dimension() > 1);
  assert(index < this->numNodes());
  if (!this->m_metadata.isVector()) {
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

double OutputRecord::v(size_t index) const {
  assert(this->m_metadata.dimension() > 1);
  assert(index < this->numNodes());
  if (!this->m_metadata.isVector()) {
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

double OutputRecord::w(size_t index) const {
  assert(this->m_metadata.dimension() > 2);
  assert(index < this->numNodes());
  if (!this->m_metadata.isVector()) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not a vector");
    return 0.0;
  }
  if (index < this->numNodes()) {
    return this->m_w[index];
  } else {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return 0.0;
  }
}

void OutputRecord::setAllU(const std::vector<double>& values) {
  assert(this->m_metadata.dimension() > 1);
  assert(this->m_numNodes == values.size());
  if (!this->m_metadata.isVector()) {
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
  assert(this->m_metadata.dimension() > 1);
  assert(this->m_numNodes == values.size());
  if (!this->m_metadata.isVector()) {
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

void OutputRecord::setAllW(const std::vector<double>& values) {
  assert(this->m_metadata.dimension() > 2);
  assert(this->m_numNodes == values.size());
  if (!this->m_metadata.isVector()) {
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
  assert(this->m_metadata.dimension() == 2);
  assert(this->m_numNodes == values_u.size());
  assert(this->m_numNodes == values_v.size());
  if (this->m_metadata.dimension() != 2) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not 2D");
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

void OutputRecord::setAll(const std::vector<double>& values_u,
                          const std::vector<double>& values_v,
                          const std::vector<double>& values_w) {
  assert(this->m_metadata.dimension() == 3);
  assert(this->m_numNodes == values_u.size());
  assert(this->m_numNodes == values_v.size());
  assert(this->m_numNodes == values_w.size());
  if (this->m_metadata.dimension() != 3) {
    adcircmodules_throw_exception("OutputRecord: Datatype is not 3D");
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
  if (values_w.size() != this->m_numNodes) {
    adcircmodules_throw_exception("OutputRecord: Index out of range");
    return;
  }
  this->m_u = values_u;
  this->m_v = values_v;
  this->m_w = values_w;
  return;
}

void OutputRecord::setAll(const std::vector<double>& values) {
  assert(this->m_metadata.dimension() == 1);
  assert(this->m_numNodes == values.size());
  if (this->m_metadata.isVector()) {
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
  if (this->m_metadata.isVector()) {
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
  assert(this->m_metadata.dimension() == 1);
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
  assert(this->m_metadata.dimension() == 2);
  if (this->m_numNodes != size) {
    adcircmodules_throw_exception("OutputRecord: Array size mismatch");
    return;
  }
  this->m_u = std::vector<double>(values_u, values_u + size);
  this->m_v = std::vector<double>(values_v, values_v + size);
  return;
}

void OutputRecord::setAll(size_t size, const double* values_u,
                          const double* values_v, const double* values_w) {
  assert(size == this->m_numNodes);
  assert(this->m_metadata.dimension() == 3);
  if (this->m_numNodes != size) {
    adcircmodules_throw_exception("OutputRecord: Array size mismatch");
    return;
  }
  this->m_u = std::vector<double>(values_u, values_u + size);
  this->m_v = std::vector<double>(values_v, values_v + size);
  this->m_w = std::vector<double>(values_w, values_w + size);
  return;
}

std::vector<double> OutputRecord::magnitudes() {
  assert(this->m_metadata.isVector());
  if (!this->m_metadata.isVector()) {
    adcircmodules_throw_exception(
        "OutputRecord: Must be a vector to compute magnitude");
    return std::vector<double>();
  }
  std::vector<double> m;
  m.resize(this->m_numNodes);
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    if (this->m_metadata.dimension() == 2) {
      m[i] = this->magnitude(i);
    } else if (this->m_metadata.dimension() == 3) {
      m[i] = this->magnitude(i);
    }
  }
  return m;
}

std::vector<double> OutputRecord::directions(AngleUnits angleType) {
  assert(this->m_metadata.isVector());
  if (!this->m_metadata.isVector()) {
    adcircmodules_throw_exception(
        "OutputRecord: Must be a vector to compute direction");
    return std::vector<double>();
  }
  std::vector<double> d;
  d.resize(this->m_numNodes);
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    d[i] = this->direction(i);
  }
  return d;
}

bool OutputRecord::isDefault(size_t index) const {
  if (this->m_metadata.dimension() == 1 || this->m_metadata.isMax()) {
    return FpCompare::equalTo(this->m_u[index], this->defaultValue());
  } else if (this->m_metadata.dimension() == 2) {
    return FpCompare::equalTo(this->m_u[index], this->defaultValue()) &&
           FpCompare::equalTo(this->m_v[index], this->defaultValue());
  } else if (this->m_metadata.dimension() == 3) {
    return FpCompare::equalTo(this->m_u[index], this->defaultValue()) &&
           FpCompare::equalTo(this->m_v[index], this->defaultValue()) &&
           FpCompare::equalTo(this->m_w[index], this->defaultValue());
  } else {
    return false;
  }
}

size_t OutputRecord::numNonDefault() const {
  size_t count = 0;
  for (size_t i = 0; i < this->m_numNodes; ++i) {
    count += this->isDefault(i) ? 0 : 1;
  }
  return count;
}

Adcirc::Output::OutputMetadata* OutputRecord::metadata() {
  return &this->m_metadata;
}

void OutputRecord::setMetadata(const Adcirc::Output::OutputMetadata& metadata) {
  this->m_metadata = metadata;
}
