/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
#ifndef ADCIRCMODULES_HMDFDATACONTAINER_H
#define ADCIRCMODULES_HMDFDATACONTAINER_H

#include <vector>

#include "CDate.h"

namespace Adcirc {
namespace Output {

class HmdfDataContainer {
 public:
  explicit HmdfDataContainer(size_t dim = 1);

  HmdfDataContainer(const Adcirc::CDate &value_date,
                    const std::vector<double> &value_data, size_t dim = 1);

  HmdfDataContainer(const Adcirc::CDate &value_date, double value_data);

  HmdfDataContainer(const Adcirc::CDate &value_date, double value_data_u,
                    double value_data_v);

  HmdfDataContainer(const Adcirc::CDate &value_date, double value_data_u,
                    double value_data_v, double value_data_w);

  double data(size_t dim = 0) const;
  void setData(double value, size_t dimension = 0);

  double adcircTime() const;
  void setAdcircTime(double value);

  size_t adcircIteration() const;
  void setAdcircIteration(size_t it);

  Adcirc::CDate datetime() const;
  void setDatetime(const Adcirc::CDate &d);

  static constexpr double defaultAdcircTime() {
    return -std::numeric_limits<double>::max();
  }

  static constexpr size_t defaultAdcircIteration() {
    return std::numeric_limits<size_t>::max();
  }

 private:
  std::vector<double> m_data;
  double m_adcirc_time;
  size_t m_adcirc_iteration;
  Adcirc::CDate m_datetime;
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCIRCMODULES_HMDFDATACONTAINER_H
