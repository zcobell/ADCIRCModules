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
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vector>
#include "attribute.h"
#include "node.h"

namespace Adcirc {
namespace ModelParameters {

class Attribute {
 public:
  explicit Attribute();

  explicit Attribute(size_t size);

  void resize(size_t size);

  double value(size_t index) const;
  std::vector<double> values() const;

  void setValue(const double &value);
  void setValue(size_t index, const double &value);
  void setValue(const std::vector<double> &values);

  Adcirc::Geometry::Node *node();
  void setNode(Adcirc::Geometry::Node *node);

  size_t size() const;

  size_t id() const;
  void setId(size_t id);

 private:
  /// ID number in the Adcirc Nodal Attributes file
  size_t m_id;

  /// Value(s) for nodal parameter at this node
  std::vector<double> m_values;

  /// Node that this value applies to
  Adcirc::Geometry::Node *m_node;
};
}  // namespace ModelParameters
}  // namespace Adcirc
#endif  // ATTRIBUTE_H
