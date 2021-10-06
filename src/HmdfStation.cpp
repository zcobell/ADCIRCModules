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
#include "HmdfStation.h"

#include "FPCompare.h"

using namespace Adcirc::Output;

struct m_DataPair {
  Adcirc::CDate m_date;
  double m_data_u = Adcirc::Output::HmdfStation::nullDataValue();
  double m_data_v = Adcirc::Output::HmdfStation::nullDataValue();
};

bool operator<(const m_DataPair& a, const m_DataPair& b) {
  return a.m_date < b.m_date;
}

bool operator==(const m_DataPair& a, const m_DataPair& b) {
  return a.m_date == b.m_date &&
         Adcirc::FpCompare::equalTo(a.m_data_u, b.m_data_u) &&
         Adcirc::FpCompare::equalTo(a.m_data_v, b.m_data_v);
}

Adcirc::Output::HmdfStation::HmdfStation(size_t dimension)
    : m_coordinate(Coordinate()),
      m_name("noname"),
      m_id("noid"),
      m_isNull(true),
      m_stationIndex(0),
      m_nullValue(nullDataValue()),
      m_positiveDirection(0),
      m_dimension(dimension) {}

void Adcirc::Output::HmdfStation::clear() {
  m_coordinate = Coordinate();
  m_name = "noname";
  m_id = "noid";
  m_isNull = true;
  m_stationIndex = 0;
  m_positiveDirection = 0;
  m_data.clear();
}

Adcirc::Output::Coordinate* Adcirc::Output::HmdfStation::coordinate() {
  return &m_coordinate;
}

void Adcirc::Output::HmdfStation::setCoordinate(
    const Adcirc::Output::Coordinate coordinate) {
  m_coordinate = coordinate;
}

double Adcirc::Output::HmdfStation::latitude() const {
  return m_coordinate.latitude();
}

double Adcirc::Output::HmdfStation::longitude() const {
  return m_coordinate.longitude();
}

void Adcirc::Output::HmdfStation::setLatitude(const double latitude) {
  m_coordinate.setLatitude(latitude);
}

void Adcirc::Output::HmdfStation::setLongitude(const double longitude) {
  m_coordinate.setLongitude(longitude);
}

std::string Adcirc::Output::HmdfStation::name() const { return m_name; }
void Adcirc::Output::HmdfStation::setName(const std::string& name) {
  m_name = name;
}

std::string Adcirc::Output::HmdfStation::id() const { return m_id; }

void Adcirc::Output::HmdfStation::setId(const std::string& id) { m_id = id; }

size_t Adcirc::Output::HmdfStation::numSnaps() const { return m_data.size(); }

size_t Adcirc::Output::HmdfStation::stationIndex() const {
  return m_stationIndex;
}

void Adcirc::Output::HmdfStation::setStationIndex(size_t stationIndex) {
  m_stationIndex = stationIndex;
}

Adcirc::CDate Adcirc::Output::HmdfStation::date(size_t index) const {
  assert(index < m_data.size());
  return m_data[index].datetime();
}

void Adcirc::Output::HmdfStation::setDate(const Adcirc::CDate& date,
                                          size_t index) {
  assert(index < m_data.size());
  m_data[index].setDatetime(date);
}

void Adcirc::Output::HmdfStation::setDate(
    const std::vector<Adcirc::CDate>& date) {
  assert(date.size() == m_data.size());
  for (auto i = 0; i < m_data.size(); ++i) {
    m_data[i].setDatetime(date[i]);
  }
}

void Adcirc::Output::HmdfStation::setNext(const Adcirc::CDate& date,
                                          const std::vector<double>& data) {
  m_data.template emplace_back(date, data);
}

void Adcirc::Output::HmdfStation::setNext(const Adcirc::CDate& date,
                                          const double& data) {
  m_data.emplace_back(date, data);
}

void Adcirc::Output::HmdfStation::setNext(const Adcirc::CDate& date,
                                          const double& data_u,
                                          const double& data_v) {
  m_data.emplace_back(date, data_u, data_v);
}

void Adcirc::Output::HmdfStation::setNext(
    const Adcirc::CDate& date, const std::tuple<double, double>& data) {
  assert(m_dimension == 2);
  auto d1 = std::get<0>(data);
  auto d2 = std::get<1>(data);
  m_data.emplace_back(date, d1, d2);
}

bool Adcirc::Output::HmdfStation::isNull() const { return m_isNull; }

void Adcirc::Output::HmdfStation::setIsNull(bool isNull) { m_isNull = isNull; }

void Adcirc::Output::HmdfStation::setData(const double& data, size_t index,
                                          size_t dim) {
  assert(index < m_data.size());
  assert(dim < m_dimension);
  m_data[index].setData(data, dim);
}

std::vector<Adcirc::CDate> Adcirc::Output::HmdfStation::allDate() const {
  std::vector<Adcirc::CDate> dates;
  dates.reserve(m_data.size());
  for (const auto& i : m_data) {
    dates.push_back(i.datetime());
  }
  return dates;
}

std::vector<double> Adcirc::Output::HmdfStation::allData(size_t dim) const {
  assert(dim < m_dimension);
  std::vector<double> data;
  data.reserve(m_data.size());
  for (const auto& i : m_data) {
    data.push_back(i.data(dim));
  }
  return data;
}

void Adcirc::Output::HmdfStation::dataBounds(Adcirc::CDate& minDate,
                                             Adcirc::CDate& maxDate,
                                             double& minValue,
                                             double& maxValue) {
  auto data_u = this->allData(0);
  auto date = this->allDate();
  minDate = *std::min_element(date.begin(), date.end());
  maxDate = *std::max_element(date.begin(), date.end());
  if (this->m_dimension == 2) {
    auto data_v = this->allData(1);
    double umin, umax, vmin, vmax;
    std::tie(umin, umax) = this->getVectorBounds(data_u);
    std::tie(vmin, vmax) = this->getVectorBounds(data_v);
    minValue = std::min(umin, vmin);
    maxValue = std::min(umax, vmax);
  } else {
    std::tie(minValue, maxValue) = this->getVectorBounds(data_u);
  }
}

double Adcirc::Output::HmdfStation::nullValue() const { return m_nullValue; }

void Adcirc::Output::HmdfStation::setNullValue(double nullValue) {
  m_nullValue = nullValue;
}

void Adcirc::Output::HmdfStation::reserve(size_t size) { m_data.reserve(size); }

void Adcirc::Output::HmdfStation::resize(size_t size) { m_data.resize(size); }

size_t Adcirc::Output::HmdfStation::dimension() const { return m_dimension; }

double Adcirc::Output::HmdfStation::positiveDirection() const {
  return m_positiveDirection;
}

void Adcirc::Output::HmdfStation::setPositiveDirection(
    double positiveDirection) {
  if (positiveDirection < -360.0 || positiveDirection > 360.0) {
    adcircmodules_throw_exception("Positive direction is out of bounds");
  } else if (positiveDirection < -180.0) {
    positiveDirection += 360.0;
  } else if (positiveDirection > 180.0) {
    positiveDirection -= 360.0;
  }
  this->m_positiveDirection = positiveDirection;
}

void Adcirc::Output::HmdfStation::sanitize(const double minValid,
                                           const double maxValid) {
  std::vector<m_DataPair> d(m_data.size());
  for (auto i = 0; i < d.size(); ++i) {
    d[i].m_date = m_data[i].datetime();
    d[i].m_data_u = this->m_data[i].data(0);
    if (m_dimension == 2) {
      d[i].m_data_v = this->m_data[i].data(1);
    }
  }
  std::sort(d.begin(), d.end());
  for (size_t i = 0; i < d.size(); ++i) {
    this->m_data[i].setDatetime(d[i].m_date);
    this->m_data[i].setData(d[i].m_data_u < minValid
                                ? Adcirc::Output::HmdfStation::nullDataValue()
                            : d[i].m_data_u > maxValid
                                ? Adcirc::Output::HmdfStation::nullDataValue()
                                : d[i].m_data_u);
    if (m_dimension == 2) {
      this->m_data[i].setData(d[i].m_data_v < minValid
                                  ? Adcirc::Output::HmdfStation::nullDataValue()
                              : d[i].m_data_v > maxValid
                                  ? Adcirc::Output::HmdfStation::nullDataValue()
                                  : d[i].m_data_v,
                              1);
    }
  }
}

std::tuple<double, double> Adcirc::Output::HmdfStation::getVectorBounds(
    const std::vector<double>& v) {
  std::vector<double> sortedData = v;
  std::sort(sortedData.begin(), sortedData.end());
  double minValue;
  if (FpCompare::equalTo(sortedData.front(), sortedData.back())) {
    minValue = *std::upper_bound(sortedData.begin(), sortedData.end(),
                                 this->nullDataValue());
  } else {
    minValue = sortedData.front();
  }
  return std::make_tuple(minValue, sortedData.back());
}

size_t Adcirc::Output::HmdfStation::adcircIteration(size_t index) {
  assert(index < m_data.size());
  return m_data[index].adcircIteration();
}

void Adcirc::Output::HmdfStation::setAdcircIteration(size_t index, size_t it) {
  assert(index < m_data.size());
  m_data[index].setAdcircIteration(it);
}

double Adcirc::Output::HmdfStation::adcircTime(size_t index) {
  assert(index < m_data.size());
  return m_data[index].adcircTime();
}

void Adcirc::Output::HmdfStation::setAdcircTime(size_t index, double time) {
  assert(index < m_data.size());
  m_data[index].setAdcircTime(time);
}
double Adcirc::Output::HmdfStation::data(size_t index, size_t dim) const {
  assert(index < m_data.size());
  assert(dim < m_dimension);
  return m_data[index].data(dim);
}