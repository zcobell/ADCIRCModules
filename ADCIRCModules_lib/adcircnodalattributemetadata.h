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
#ifndef ADCIRCNODALATTRIBUTEMETADATA_H
#define ADCIRCNODALATTRIBUTEMETADATA_H

#include <string>
#include <vector>

class AdcircNodalAttributeMetadata {
public:
  explicit AdcircNodalAttributeMetadata(std::string name = "defaultName",
                                std::string units = "none", int numValues = 1);

  std::string name() const;
  void setName(const std::string &name);

  std::string units() const;
  void setUnits(const std::string &units);

  int numberOfValues() const;
  void setNumberOfValues(int numValues);

  std::vector<double> getDefaultValues() const;
  double getDefaultValue(int index = 0) const;
  void setDefaultValue(const double &value);
  void setDefaultValue(const std::vector<double> &value);

private:
  /// Name of the nodal attribute
  std::string m_name;

  /// Units for the nodal parameter
  std::string m_units;

  /// Number of values associated with each node for this nodal attribute
  int m_numValues;

  /// Default value(s) to be used when writing the fort.13 file
  std::vector<double> m_defaultValue;
};

#endif // ADCIRCNODALATTRIBUTEMETADATA_H
