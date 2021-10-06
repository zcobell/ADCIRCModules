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

#include "HmdfDataContainer.h"

#include <cassert>

using namespace Adcirc::Output;

HmdfDataContainer::HmdfDataContainer(size_t dim)
    : m_data(dim),
      m_adcirc_time(HmdfDataContainer::defaultAdcircTime()),
      m_adcirc_iteration(HmdfDataContainer::defaultAdcircIteration()),
      m_datetime(1970, 0, 0) {}

HmdfDataContainer::HmdfDataContainer(const Adcirc::CDate& value_date,
                                     const std::vector<double>& value_data,
                                     size_t dim)
    : m_datetime(value_date),
      m_data(value_data),
      m_adcirc_time(HmdfDataContainer::defaultAdcircTime()),
      m_adcirc_iteration(HmdfDataContainer::defaultAdcircIteration()) {
  assert(dim == value_data.size());
}
HmdfDataContainer::HmdfDataContainer(const Adcirc::CDate& value_date,
                                     double value_data)
    : m_datetime(value_date),
      m_data({value_data}),
      m_adcirc_time(HmdfDataContainer::defaultAdcircTime()),
      m_adcirc_iteration(HmdfDataContainer::defaultAdcircIteration()) {}

HmdfDataContainer::HmdfDataContainer(const Adcirc::CDate& value_date,
                                     double value_data_u, double value_data_v)
    : m_datetime(value_date),
      m_data({value_data_u, value_data_v}),
      m_adcirc_time(HmdfDataContainer::defaultAdcircTime()),
      m_adcirc_iteration(HmdfDataContainer::defaultAdcircIteration()) {}

HmdfDataContainer::HmdfDataContainer(const Adcirc::CDate& value_date,
                                     double value_data_u, double value_data_v,
                                     double value_data_w)
    : m_datetime(value_date),
      m_data({value_data_u, value_data_v, value_data_w}),
      m_adcirc_time(HmdfDataContainer::defaultAdcircTime()),
      m_adcirc_iteration(HmdfDataContainer::defaultAdcircIteration()) {}

double HmdfDataContainer::data(size_t dim) const {
  assert(dim < m_data.size());
  return m_data[dim];
}

void HmdfDataContainer::setData(double value, size_t dimension) {
  assert(dimension < m_data.size());
  m_data[dimension] = value;
}

double HmdfDataContainer::adcircTime() const { return m_adcirc_time; }

void HmdfDataContainer::setAdcircTime(double value) { m_adcirc_time = value; }

size_t HmdfDataContainer::adcircIteration() const { return m_adcirc_iteration; }

void HmdfDataContainer::setAdcircIteration(size_t it) {
  m_adcirc_iteration = it;
}

Adcirc::CDate HmdfDataContainer::datetime() const { return m_datetime; }

void HmdfDataContainer::setDatetime(const Adcirc::CDate& d) { m_datetime = d; }
