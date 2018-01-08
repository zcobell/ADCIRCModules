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
#include "adcircnodalattribute.h"
#include <assert.h>

AdcircNodalAttribute::AdcircNodalAttribute() {
  this->m_node = nullptr;
  this->resize(1);
}

AdcircNodalAttribute::AdcircNodalAttribute(int size) {
  this->m_node = nullptr;
  this->resize(size);
}

void AdcircNodalAttribute::resize(int size) {
  if (size != this->m_size) {
    this->m_values.resize(size);
    this->m_size = size;
  }
}

double AdcircNodalAttribute::value(int index) const {
  assert(index >= 0 && index < this->m_size);

  if (index >= 0 && index < this->m_size)
    return m_values[index];
  else
    return -9999.0;
}

std::vector<double> AdcircNodalAttribute::values() const {
  return this->m_values;
}

void AdcircNodalAttribute::setValue(const double &value) {
  std::fill(this->m_values.begin(), this->m_values.end(), value);
}

void AdcircNodalAttribute::setValue(int index, const double &value) {
  assert(index >= 0 && index < this->m_size);

  if (index >= 0 && index < this->m_size)
    this->m_values[index] = value;
}

void AdcircNodalAttribute::setValue(const std::vector<double> &values) {
  assert(values.size() == this->m_size);

  if (values.size() == this->m_size)
    this->m_values = values;
}

AdcircNode *AdcircNodalAttribute::node() { return this->m_node; }

void AdcircNodalAttribute::setNode(AdcircNode *node) { this->m_node = node; }

int AdcircNodalAttribute::size() const { return this->m_size; }

int AdcircNodalAttribute::id() const { return this->m_id; }

void AdcircNodalAttribute::setId(int id) { this->m_id = id; }
