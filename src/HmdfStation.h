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
#ifndef ADCMOD_HMDFSTATION
#define ADCMOD_HMDFSTATION

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "CDate.h"
#include "Coordinate.h"
#include "HmdfDataContainer.h"

namespace Adcirc {

namespace Output {

class HmdfStation {
 public:
  ADCIRCMODULES_EXPORT explicit HmdfStation(size_t dimension = 1);

  void ADCIRCMODULES_EXPORT clear();

  static constexpr double ADCIRCMODULES_EXPORT nullDataValue() {
    return -std::numeric_limits<double>::max();
  }

  static constexpr long long ADCIRCMODULES_EXPORT nullDateValue() {
    return std::numeric_limits<long long>::max();
  }

  static constexpr double ADCIRCMODULES_EXPORT defaultAdcircTime() {
    return HmdfDataContainer::defaultAdcircTime();
  }

  static constexpr size_t ADCIRCMODULES_EXPORT defaultAdcircIteration() {
    return HmdfDataContainer::defaultAdcircIteration();
  }

  Adcirc::Output::Coordinate ADCIRCMODULES_EXPORT *coordinate();

  void ADCIRCMODULES_EXPORT
  setCoordinate(Adcirc::Output::Coordinate coordinate);

  double ADCIRCMODULES_EXPORT latitude() const;
  double ADCIRCMODULES_EXPORT longitude() const;

  void ADCIRCMODULES_EXPORT setLatitude(double latitude);

  void ADCIRCMODULES_EXPORT setLongitude(double longitude);

  std::string ADCIRCMODULES_EXPORT name() const;

  void ADCIRCMODULES_EXPORT setName(const std::string &name);

  std::string ADCIRCMODULES_EXPORT id() const;

  void ADCIRCMODULES_EXPORT setId(const std::string &id);

  size_t ADCIRCMODULES_EXPORT numSnaps() const;

  size_t ADCIRCMODULES_EXPORT stationIndex() const;

  void ADCIRCMODULES_EXPORT setStationIndex(size_t stationIndex);

  Adcirc::CDate ADCIRCMODULES_EXPORT date(size_t index) const;

  double ADCIRCMODULES_EXPORT data(size_t index, size_t dim = 0) const;

  void ADCIRCMODULES_EXPORT setDate(const Adcirc::CDate &date, size_t index);

  void ADCIRCMODULES_EXPORT setDate(const std::vector<Adcirc::CDate> &date);

  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const std::vector<double> &data);

  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const double &data);

  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const double &data_u, const double &data_v);

  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const std::tuple<double, double> &data);

  bool ADCIRCMODULES_EXPORT isNull() const;
  void ADCIRCMODULES_EXPORT setIsNull(bool isNull);

  void ADCIRCMODULES_EXPORT setData(const double &data, size_t index,
                                    size_t dim = 0);

  template <typename T>
  void ADCIRCMODULES_EXPORT setData(const std::vector<T> &data,
                                    size_t dim = 0) {
    assert(data.size() == m_data.size());
    for (auto i = 0; i < m_data.size(); ++i) {
      m_data[i].setData(dim, data[i]);
    }
  }

  size_t ADCIRCMODULES_EXPORT adcircIteration(size_t index);

  void ADCIRCMODULES_EXPORT setAdcircIteration(size_t index, size_t it);

  double ADCIRCMODULES_EXPORT adcircTime(size_t index);

  void ADCIRCMODULES_EXPORT setAdcircTime(size_t index, double time);

  std::vector<Adcirc::CDate> ADCIRCMODULES_EXPORT allDate() const;

  std::vector<double> ADCIRCMODULES_EXPORT allData(size_t dim = 0) const;

  void ADCIRCMODULES_EXPORT dataBounds(Adcirc::CDate &minDate,
                                       Adcirc::CDate &maxDate, double &minValue,
                                       double &maxValue);

  double ADCIRCMODULES_EXPORT nullValue() const;

  void ADCIRCMODULES_EXPORT setNullValue(double nullValue);

  void ADCIRCMODULES_EXPORT reserve(size_t size);

  void ADCIRCMODULES_EXPORT resize(size_t size);

  size_t ADCIRCMODULES_EXPORT dimension() const;

  double ADCIRCMODULES_EXPORT positiveDirection() const;

  void ADCIRCMODULES_EXPORT setPositiveDirection(double positiveDirection);

  void ADCIRCMODULES_EXPORT sanitize(double minValid, double maxValid);

 private:
  std::tuple<double, double> getVectorBounds(const std::vector<double> &v);

  Adcirc::Output::Coordinate m_coordinate;
  std::string m_name;
  std::string m_id;
  size_t m_stationIndex;
  double m_nullValue;
  double m_positiveDirection;
  size_t m_dimension;
  bool m_isNull;
  std::vector<HmdfDataContainer> m_data;
};

#ifndef SWIG
template void ADCIRCMODULES_EXPORT
Adcirc::Output::HmdfStation::setData<float>(const std::vector<float> &, size_t);

template void ADCIRCMODULES_EXPORT Adcirc::Output::HmdfStation::setData<double>(
    const std::vector<double> &, size_t);
#endif
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_HMDFSTATION
