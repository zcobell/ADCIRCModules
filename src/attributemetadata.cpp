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
#include "attributemetadata.h"
#include <cassert>

using namespace Adcirc::ModelParameters;

/**
 * @brief Constructor
 * Defines the name, units and number of values within the specific
 * nodal attribute. Also, resizes the internal vector used to store
 * default values.
 *
 */
AttributeMetadata::AttributeMetadata(const string &name, const string &units,
                                     size_t numValues)
    : m_name(name), m_units(units) {
  this->setNumberOfValues(numValues);
}

/**
 * @brief Returns the name of the nodal attribute
 * @return name of nodal attribute
 */
string AttributeMetadata::name() const { return this->m_name; }

/**
 * @brief Sets the name of the nodal attribute
 * @param name string to use for name
 */
void AttributeMetadata::setName(const string &name) { this->m_name = name; }

/**
 * @brief Returns the units for the nodal attribute
 * @return units for the nodal attribute
 */
string AttributeMetadata::units() const { return this->m_units; }

/**
 * @brief Sets the units for the nodal attribute
 * @param units value to set for the units string
 */
void AttributeMetadata::setUnits(const string &units) { this->m_units = units; }

/**
 * @brief Returns the number of values for the nodal attribute
 * @return number of values for the nodal attribute
 */
size_t AttributeMetadata::numberOfValues() const { return this->m_numValues; }

/**
 * @brief Sets the number of values for the nodal attribute
 * @param numValues number of values to resize the object to use
 *
 * Sets the number of values for the nodal attribute and resizes the
 * internal vector
 */
void AttributeMetadata::setNumberOfValues(size_t numValues) {
  this->m_numValues = numValues;
  this->m_defaultValue.resize(this->numberOfValues());
}

/**
 * @brief Returns the default value vector
 * @return vector of default values for the nodal attribute
 */
vector<double> AttributeMetadata::getDefaultValues() const {
  return this->m_defaultValue;
}

/**
 * @brief Returns the default value for a specific position
 * @param index position to return
 * @return default value at position index
 */
double AttributeMetadata::getDefaultValue(size_t index) const {
  assert(index < this->m_numValues);

  if (index < this->m_numValues) {
    return this->m_defaultValue[index];
  } else {
    return this->m_defaultValue[0];
  }
}

/**
 * @brief Sets the default value to the data contained within a vector
 * @param value vector to use to fill the default values vector
 */
void AttributeMetadata::setDefaultValue(const vector<double> &value) {
  assert(value.size() == this->m_numValues);
  this->m_defaultValue = value;
}

/**
 * @brief Fills the default value vector with a specified value
 * @param value value to fill for all default values
 */
void AttributeMetadata::setDefaultValue(const double &value) {
  std::fill(this->m_defaultValue.begin(), this->m_defaultValue.end(), value);
}

/**
 * @brief Sets the default value at a specific index
 * @param index position to set the default value
 * @param value value to set at index
 */
void AttributeMetadata::setDefaultValue(size_t index, const double &value) {
  assert(index < this->m_numValues);
  this->m_defaultValue[index] = value;
}
