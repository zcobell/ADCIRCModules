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
/**
 * @class AdcircElement
 * @author Zachary Cobell
 * @brief The AdcircElement class describes an AdcircElement as an array
 * of AdcircNode pointers
 *
 */
#ifndef ADCIRCELEMENT_H
#define ADCIRCELEMENT_H

#include "adcircmodules_global.h"
#include "adcircnode.h"
#include <string>
#include <vector>

class AdcircElement {
public:
  explicit AdcircElement();
  explicit AdcircElement(int id, AdcircNode *n1, AdcircNode *n2,
                         AdcircNode *n3);

  void setElement(int id, AdcircNode *n1, AdcircNode *n2, AdcircNode *n3);

  int n() const;

  AdcircNode *node(int i);
  void setNode(int i, AdcircNode *node);

  int id() const;
  void setId(int id);

  std::string toString();

private:
  int m_n = 3;

  int m_id;
  std::vector<AdcircNode *> m_nodes;
};

#endif // ADCIRCELEMENT_H
