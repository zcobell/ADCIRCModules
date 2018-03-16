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
#ifndef ATTRIBUTEMETADATA_H
#define ATTRIBUTEMETADATA_H

#include <string>
#include <vector>

using namespace std;

namespace Adcirc {

namespace ModelParameters {

class AttributeMetadata {
 public:
  explicit AttributeMetadata(string name = "defaultName", string units = "none",
                             int numValues = 1);

  string name() const;
  void setName(const string &name);

  string units() const;
  void setUnits(const string &units);

  int numberOfValues() const;
  void setNumberOfValues(int numValues);

  vector<double> getDefaultValues() const;
  double getDefaultValue(int index = 0) const;
  void setDefaultValue(const double &value);
  void setDefaultValue(int index, const double &value);
  void setDefaultValue(const vector<double> &value);

 private:
  /// Name of the nodal attribute
  string m_name;

  /// Units for the nodal parameter
  string m_units;

  /// Number of values associated with each node for this nodal attribute
  int m_numValues;

  /// Default value(s) to be used when writing the fort.13 file
  vector<double> m_defaultValue;
};
}
}
#endif  // ATTRIBUTEMETADATA_H
