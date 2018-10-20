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
#ifndef ATTRIBUTEMETADATA_H
#define ATTRIBUTEMETADATA_H

#include <string>
#include <vector>
#include "attribute.h"

namespace Adcirc {

namespace ModelParameters {

/**
 * @class AttributeMetadata
 * @author Zachary Cobell
 * @brief Class handles the organization of nodal attribute metadata, i.e. name,
 * default value, etc
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved.
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
  explicit AttributeMetadata(const std::string &name = "defaultName",
                             const std::string &units = "none",
                             size_t numValues = 1);

  std::string name() const;
  void setName(const std::string &name);

  std::string units() const;
  void setUnits(const std::string &units);

  size_t numberOfValues() const;
  void setNumberOfValues(size_t numValues);

  std::vector<double> getDefaultValues() const;
  double defaultValue(size_t index = 0) const;
  void setDefaultValue(const double &value);
  void setDefaultValue(size_t index, const double &value);
  void setDefaultValue(const std::vector<double> &value);
  bool checkIfDefaultValue(const std::vector<double> &value);
  bool checkIfDefaultValue(const double &value);
  bool checkIfDefaultValue(const Adcirc::ModelParameters::Attribute &a);

  std::string headerString();

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
#endif  // ATTRIBUTEMETADATA_H
