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
#ifndef OUTPUTRECORD_H
#define OUTPUTRECORD_H

#include <vector>
#include "adcirc/adcirc_errors.h"
#include "adcirc/geometry/node.h"

namespace Adcirc {

namespace Output {

class OutputRecord {
 public:
  explicit OutputRecord(int numNodes, bool isVector);

  void setU(int index, double value);
  void setV(int index, double value);
  void set(int index, double value);
  void set(int index, double value_u, double value_v);

  double value(int index);
  double u(int index);
  double v(int index);
  double direction(int index);

  long long iteration() const;
  void setIteration(long long iteration);

  double time() const;
  void setTime(double time);

  int numNodes() const;
  void setNumNodes(int numNodes);

  bool isVector() const;
  void setIsVector(bool isVector);

 private:
  std::vector<double> m_u;
  std::vector<double> m_v;
  long long m_iteration;
  double m_time;
  int m_numNodes;
  bool m_isVector;
};
}
}

#endif  // OUTPUTRECORD_H
