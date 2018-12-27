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
#include "boost/format.hpp"
#include "error.h"

using namespace Adcirc::ModelParameters;

/**
 * @brief Default Constructor
 */
Attribute::Attribute()
    : m_node(nullptr), m_id(std::numeric_limits<size_t>::max()) {
  this->resize(1);
}

/**
 * @brief Constructor that accepts the number of values in this nodal attribute
 * @param size Number of nodal attributes to size this object for
 */
Attribute::Attribute(size_t size)
    : m_node(nullptr), m_id(std::numeric_limits<size_t>::max()) {
  this->resize(size);
}

/**
 * @brief Resizes the object to a new number of values in this nodal attribute
 */
void Attribute::resize(size_t size) { this->m_values.resize(size); }

/**
 * @brief Returns the value of the nodal attribute at the specified index
 * @param index Index of the value to return
 * @return value of nodal attribute at specified index
 */
double Attribute::value(size_t index) const {
  assert(index < this->size());

  if (index < this->size()) {
    return m_values[index];
  } else {
    adcircmodules_throw_exception("Attribute: Index out of bounds");
    return -9999.0;
  }
}

/**
 * @brief Returns a vector of all values for this nodal parameter
 * @return values for this nodal attribute
 */
std::vector<double> Attribute::values() const { return this->m_values; }

/**
 * @brief Set all values in the object to a single value
 * @param value Value applied to all indicies in the nodal attribute for the
 * present node
 */
void Attribute::setValue(const double &value) {
  std::fill(this->m_values.begin(), this->m_values.end(), value);
}

/**
 * @brief Sets a specific index to a specified value for the nodal attribute
 * @param index position to set to a value
 * @param value to set at index
 */
void Attribute::setValue(size_t index, const double &value) {
  assert(index < this->size());

  if (index < this->size()) {
    this->m_values[index] = value;
  } else {
    adcircmodules_throw_exception("Attribute: Index out of bounds");
  }
}

/**
 * @brief Uses a vector of values to set nodal attributes
 * @param values Vector of values. Must have the same size has this->size()
 */
void Attribute::setValue(const std::vector<double> &values) {
  assert(values.size() == this->size());

  if (values.size() == this->size()) {
    this->m_values = values;
  } else {
    adcircmodules_throw_exception("Attribute: Index out of bounds");
  }
}

/**
 * @brief Returns a pointer to the node for which the current nodal attribute
 * applies
 */
Adcirc::Geometry::Node *Attribute::node() { return this->m_node; }

/**
 * @brief Sets the pointer to the node that this nodal attribute is applied to
 */
void Attribute::setNode(Adcirc::Geometry::Node *node) { this->m_node = node; }

/**
 * @brief Returns the current size of this nodal attribute
 * @return size of nodal attribute
 */
size_t Attribute::size() const { return this->m_values.size(); }

/**
 * @brief Returns idenfitier for the current nodal attribute as specified in the
 * fort.13, usually node number
 * @return size of nodal attribute
 */
size_t Attribute::id() const { return this->m_id; }

/**
 * @brief Sets the identifier for the current nodal attribute, usually the node
 * number specified in fort.13 file
 */
void Attribute::setId(size_t id) { this->m_id = id; }

/**
 * @brief Generates a string to print in a nodal attributes file for this object
 * @return string formatted attribute
 */
std::string Attribute::write() {
  std::string f = boost::str(boost::format("%11i  ") % this->m_id);
  for (auto &v : this->m_values) {
    f = f + boost::str(boost::format("%12.6f  ") % v);
  }
  f = f + "\n";
  return f;
}
