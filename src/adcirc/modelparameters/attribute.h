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
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "adcirc/geometry/node.h"
#include "adcirc/modelparameters/attribute.h"
#include <vector>

namespace Adcirc {

namespace ModelParameters {

class Attribute {
public:
  explicit Attribute();

  explicit Attribute(int size);

  void resize(int size);

  double value(int index) const;
  vector<double> values() const;

  void setValue(const double &value);
  void setValue(int index, const double &value);
  void setValue(const vector<double> &values);

  Adcirc::Geometry::Node *node();
  void setNode(Adcirc::Geometry::Node *node);

  int size() const;

  int id() const;
  void setId(int id);

private:
  /// Number of values in this dataset
  int m_size;

  /// ID number in the Adcirc Nodal Attributes file
  int m_id;

  /// Value(s) for nodal parameter at this node
  vector<double> m_values;

  /// Node that this value applies to
  Adcirc::Geometry::Node *m_node;
};
}
}
#endif // ATTRIBUTE_H
