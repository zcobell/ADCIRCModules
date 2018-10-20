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
#include <algorithm>
#include <cassert>
#include "boost/format.hpp"

using namespace Adcirc::ModelParameters;

/**
 * @brief Constructor
 * Defines the name, units and number of values within the specific
 * nodal attribute. Also, resizes the internal vector used to store
 * default values.
 *
 */
AttributeMetadata::AttributeMetadata(const std::string &name,
                                     const std::string &units, size_t numValues)
    : m_name(name), m_units(units) {
  this->setNumberOfValues(numValues);
}

/**
 * @brief Returns the name of the nodal attribute
 * @return name of nodal attribute
 */
std::string AttributeMetadata::name() const { return this->m_name; }

/**
 * @brief Sets the name of the nodal attribute
 * @param name string to use for name
 */
void AttributeMetadata::setName(const std::string &name) {
  this->m_name = name;
}

/**
 * @brief Returns the units for the nodal attribute
 * @return units for the nodal attribute
 */
std::string AttributeMetadata::units() const { return this->m_units; }

/**
 * @brief Sets the units for the nodal attribute
 * @param units value to set for the units string
 */
void AttributeMetadata::setUnits(const std::string &units) {
  this->m_units = units;
}

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
std::vector<double> AttributeMetadata::getDefaultValues() const {
  return this->m_defaultValue;
}

/**
 * @brief Returns the default value for a specific position
 * @param index position to return
 * @return default value at position index
 */
double AttributeMetadata::defaultValue(size_t index) const {
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
void AttributeMetadata::setDefaultValue(const std::vector<double> &value) {
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

/**
 * @brief Creates the header string for writing a fort.13 file
 * @return formatted string for a fort.13 file header
 */
std::string AttributeMetadata::headerString() {
  std::string f = boost::str(boost::format("%s\n%s\n%11i\n") % this->m_name %
                             this->m_units % this->m_numValues);
  for (auto v : this->m_defaultValue) {
    f = f + boost::str(boost::format("%12.6f") % v);
  }
  f = f + "\n";
  return f;
}

/**
 * @brief Checks if the supplied value is a default value for this nodal
 *attribute
 * @param value Value to check
 * @return returns true if the supplied value is the default value
 *
 * Checks if the supplied value is the default value for this nodal attribute.
 * Note that if the nodal attribute contains multiple values, every value is
 *checked to ensure equality
 **/
bool AttributeMetadata::checkIfDefaultValue(const double &value) {
  for (const auto &v : this->m_defaultValue) {
    if (v != value) return false;
  }
  return true;
}

/**
 * @param value Value to check
 * @return returns true if the supplied value is the default value
 *
 * Vector version of above function. Checks to see if the supplied vector
 * is equal to the default value vector
 **/
bool AttributeMetadata::checkIfDefaultValue(const std::vector<double> &value) {
  if (value == this->m_defaultValue) return true;
  return false;
}

/**
 * @param value Value to check
 * @return returns true if the supplied value is the default value
 *
 * Attribute class based version of above function. Checks to see if the
 *supplied vector is equal to the default value vector
 **/
bool AttributeMetadata::checkIfDefaultValue(
    const Adcirc::ModelParameters::Attribute &a) {
  if (a.values() == this->m_defaultValue) return true;
  return false;
}
