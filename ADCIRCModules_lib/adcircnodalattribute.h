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
#ifndef ADCIRCNODALATTRIBUTE_H
#define ADCIRCNODALATTRIBUTE_H

#include "adcircnodalattributemetadata.h"
#include "adcircnode.h"
#include <vector>

class AdcircNodalAttribute {
public:
  explicit AdcircNodalAttribute();

  explicit AdcircNodalAttribute(int size);

  void resize(int size);

  double value(int index) const;
  std::vector<double> values() const;

  void setValue(const double &value);
  void setValue(int index, const double &value);
  void setValue(const std::vector<double> &values);

  AdcircNode *node();
  void setNode(AdcircNode *node);

  int size() const;

  int id() const;
  void setId(int id);

private:
  /// Number of values in this dataset
  int m_size;

  /// ID number in the Adcirc Nodal Attributes file
  int m_id;

  /// Value(s) for nodal parameter at this node
  std::vector<double> m_values;

  /// Node that this value applies to
  AdcircNode *m_node;
};

#endif // ADCIRCNODALATTRIBUTE_H
