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
#include "adcirc/modelparameters/attributemetadata.h"
#include <assert.h>

using namespace Adcirc::ModelParameters;

AttributeMetadata::AttributeMetadata(string name, string units, size_t numValues) {
  this->setName(name);
  this->setUnits(units);
  this->setNumberOfValues(numValues);
}

string AttributeMetadata::name() const { return this->m_name; }

void AttributeMetadata::setName(const string &name) { this->m_name = name; }

string AttributeMetadata::units() const { return this->m_units; }

void AttributeMetadata::setUnits(const string &units) { this->m_units = units; }

size_t AttributeMetadata::numberOfValues() const { return this->m_numValues; }

void AttributeMetadata::setNumberOfValues(size_t numValues) {
  this->m_numValues = numValues;
  this->m_defaultValue.resize(this->numberOfValues());
}

vector<double> AttributeMetadata::getDefaultValues() const {
  return this->m_defaultValue;
}

double AttributeMetadata::getDefaultValue(size_t index) const {
  assert(index >= 0 && index < this->m_numValues);

  if (index >= 0 || index < this->m_numValues)
    return this->m_defaultValue[index];
  else
    return this->m_defaultValue[0];
}

void AttributeMetadata::setDefaultValue(const vector<double> &value) {
  assert(value.size() == this->m_numValues);
  this->m_defaultValue = value;
}

void AttributeMetadata::setDefaultValue(const double &value) {
  std::fill(this->m_defaultValue.begin(), this->m_defaultValue.end(), value);
}

void AttributeMetadata::setDefaultValue(size_t index, const double &value) {
  assert(index >= 0 && index < this->m_numValues);
  this->m_defaultValue[index] = value;
}
