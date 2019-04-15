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
#ifndef ADCMOD_ATTRIBUTE_H
#define ADCMOD_ATTRIBUTE_H

#include <vector>
#include "adcircmodules_global.h"
#include "node.h"

namespace Adcirc {
namespace ModelParameters {

/**
 * @class Attribute
 * @author Zachary Cobell
 * @brief Class handles nodal attributes, i.e. an ADCIRC unit 13 file
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 *
 * The Attribute class is designed to handle nodal attributes of variable
 * size (i.e. scalars like friction or 12-parametered values such as
 * directional wind reduction.
 *
 */

class Attribute {
 public:
  ADCIRCMODULES_EXPORT Attribute();

  ADCIRCMODULES_EXPORT Attribute(size_t size);

  void ADCIRCMODULES_EXPORT resize(size_t size);

  double ADCIRCMODULES_EXPORT value(size_t index) const;
  std::vector<double> ADCIRCMODULES_EXPORT values() const;

  void ADCIRCMODULES_EXPORT setValue(const double &value);
  void ADCIRCMODULES_EXPORT setValue(size_t index, const double &value);
  void ADCIRCMODULES_EXPORT setValue(const std::vector<double> &values);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node();
  void ADCIRCMODULES_EXPORT setNode(Adcirc::Geometry::Node *node);

  size_t ADCIRCMODULES_EXPORT size() const;

  size_t ADCIRCMODULES_EXPORT id() const;
  void ADCIRCMODULES_EXPORT setId(size_t id);

  std::string ADCIRCMODULES_EXPORT write();

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
