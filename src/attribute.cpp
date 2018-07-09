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
#include "attribute.h"
#include <cassert>
#include <limits>
#include "error.h"

using namespace std;
using namespace Adcirc::ModelParameters;

Attribute::Attribute() {
  this->m_node = nullptr;
  this->m_id = std::numeric_limits<size_t>::max();
  this->resize(1);
}

Attribute::Attribute(size_t size) {
  this->m_node = nullptr;
  this->m_id = std::numeric_limits<size_t>::max();
  this->resize(size);
}

void Attribute::resize(size_t size) {
  if (size != this->m_size) {
    this->m_values.resize(size);
    this->m_size = size;
  }
}

double Attribute::value(size_t index) const {
  assert(index < this->m_size);

  if (index < this->m_size) {
    return m_values[index];
  } else {
    Adcirc::Error::throwError("Attribute: Index out of bounds");
    return -9999.0;
  }
}

vector<double> Attribute::values() const { return this->m_values; }

void Attribute::setValue(const double &value) {
  std::fill(this->m_values.begin(), this->m_values.end(), value);
}

void Attribute::setValue(size_t index, const double &value) {
  assert(index < this->m_size);

  if (index < this->m_size) {
    this->m_values[index] = value;
  } else {
    Adcirc::Error::throwError("Attribute: Index out of bounds");
  }
}

void Attribute::setValue(const vector<double> &values) {
  assert(values.size() == this->m_size);

  if (values.size() == this->m_size) {
    this->m_values = values;
  } else {
    Adcirc::Error::throwError("Attribute: Index out of bounds");
  }
}

Adcirc::Geometry::Node *Attribute::node() { return this->m_node; }

void Attribute::setNode(Adcirc::Geometry::Node *node) { this->m_node = node; }

size_t Attribute::size() const { return this->m_size; }

size_t Attribute::id() const { return this->m_id; }

void Attribute::setId(size_t id) { this->m_id = id; }
