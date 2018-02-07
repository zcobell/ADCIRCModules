//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "attribute.h"
#include <assert.h>

using namespace Adcirc::ModelParameters;

Attribute::Attribute() {
  this->m_node = nullptr;
  this->resize(1);
}

Attribute::Attribute(int size) {
  this->m_node = nullptr;
  this->resize(size);
}

void Attribute::resize(int size) {
  if (size != this->m_size) {
    this->m_values.resize(size);
    this->m_size = size;
  }
}

double Attribute::value(int index) const {
  assert(index >= 0 && index < this->m_size);

  if (index >= 0 && index < this->m_size)
    return m_values[index];
  else
    return -9999.0;
}

vector<double> Attribute::values() const { return this->m_values; }

void Attribute::setValue(const double &value) {
  std::fill(this->m_values.begin(), this->m_values.end(), value);
}

void Attribute::setValue(int index, const double &value) {
  assert(index >= 0 && index < this->m_size);

  if (index >= 0 && index < this->m_size)
    this->m_values[index] = value;
}

void Attribute::setValue(const vector<double> &values) {
  assert(values.size() == this->m_size);

  if (values.size() == this->m_size)
    this->m_values = values;
}

Adcirc::Geometry::Node *Attribute::node() { return this->m_node; }

void Attribute::setNode(Adcirc::Geometry::Node *node) { this->m_node = node; }

int Attribute::size() const { return this->m_size; }

int Attribute::id() const { return this->m_id; }

void Attribute::setId(int id) { this->m_id = id; }
