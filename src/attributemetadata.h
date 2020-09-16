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
#ifndef ADCMOD_ATTRIBUTEMETADATA_H
#define ADCMOD_ATTRIBUTEMETADATA_H

#include <string>
#include <vector>
#include "adcircmodules_global.h"
#include "attribute.h"

namespace Adcirc {

namespace ModelParameters {

/**
 * @class AttributeMetadata
 * @author Zachary Cobell
 * @brief Class handles the organization of nodal attribute metadata, i.e. name,
 * default value, etc
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved.
 * @license This project is released under the terms of the GNU General Public
 * License v3
 *
 * The AttributeMetadata class handles metadata found generally in the header
 * of an adcirc unit 13 file (fort.13). Things like units, default values,
 * number of values per node, and the name of the attribute itself are contained
 * within
 *
 */

class AttributeMetadata {
 public:
  ADCIRCMODULES_EXPORT AttributeMetadata(
      const std::string &name = "defaultName",
      const std::string &units = "none", size_t numValues = 1);

  std::string ADCIRCMODULES_EXPORT name() const;
  void ADCIRCMODULES_EXPORT setName(const std::string &name);

  std::string ADCIRCMODULES_EXPORT units() const;
  void ADCIRCMODULES_EXPORT setUnits(const std::string &units);

  size_t ADCIRCMODULES_EXPORT numberOfValues() const;
  void ADCIRCMODULES_EXPORT setNumberOfValues(size_t numValues);

  std::vector<double> ADCIRCMODULES_EXPORT getDefaultValues() const;
  double ADCIRCMODULES_EXPORT defaultValue(size_t index = 0) const;
  void ADCIRCMODULES_EXPORT setDefaultValue(const double &value);
  void ADCIRCMODULES_EXPORT setDefaultValue(size_t index, const double &value);
  void ADCIRCMODULES_EXPORT setDefaultValue(const std::vector<double> &value);
  bool ADCIRCMODULES_EXPORT
  checkIfDefaultValue(const std::vector<double> &value) const;
  bool ADCIRCMODULES_EXPORT checkIfDefaultValue(const double &value) const;
  bool ADCIRCMODULES_EXPORT
  checkIfDefaultValue(const Adcirc::ModelParameters::Attribute &a) const;

  std::string ADCIRCMODULES_EXPORT headerString();

 private:
  /// Name of the nodal attribute
  std::string m_name;

  /// Units for the nodal parameter
  std::string m_units;

  /// Number of values associated with each node for this nodal attribute
  size_t m_numValues;

  /// Default value(s) to be used when writing the fort.13 file
  std::vector<double> m_defaultValue;
};
}  // namespace ModelParameters
}  // namespace Adcirc
#endif  // ADCMOD_ATTRIBUTEMETADATA_H
