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
#ifndef OUTPUTRECORD_H
#define OUTPUTRECORD_H

#include <vector>
#include "adcirc/adcirc_codes.h"
#include "adcirc/geometry/node.h"

namespace Adcirc {

namespace Output {

class OutputRecord {
 public:
  explicit OutputRecord(size_t record, size_t numNodes, bool isVector);

  void fill(double value);

  void setU(size_t index, double value);
  void setV(size_t index, double value);
  void set(size_t index, double value);
  void set(size_t index, double value_u, double value_v);

  void setAllU(std::vector<double> values);
  void setAllV(std::vector<double> values);
  void setAll(std::vector<double> values);
  void setAll(std::vector<double> values_u, std::vector<double> values_v);

  std::vector<double> values(size_t column);

  double value(size_t index);
  double u(size_t index);
  double v(size_t index);
  double direction(size_t index);

  long long iteration() const;
  void setIteration(long long iteration);

  double time() const;
  void setTime(double time);

  size_t numNodes() const;
  void setNumNodes(size_t numNodes);

  bool isVector() const;
  void setIsVector(bool isVector);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  size_t record() const;
  void setRecord(size_t record);

 private:
  std::vector<double> m_u;
  std::vector<double> m_v;
  int m_record;
  long long m_iteration;
  double m_time;
  size_t m_numNodes;
  bool m_isVector;
  double m_defaultValue;
};
}  // namespace Output
}  // namespace Adcirc

#endif  // OUTPUTRECORD_H
