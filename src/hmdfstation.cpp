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
#include "hmdfstation.h"

#include <cassert>

#include "fpcompare.h"
#include "logging.h"

using namespace Adcirc::Output;

struct m_DataPair {
  Adcirc::CDate m_date;
  double m_data_u = HmdfStation::nullDataValue();
  double m_data_v = HmdfStation::nullDataValue();
};

bool operator<(const m_DataPair &a, const m_DataPair &b) {
  return a.m_date < b.m_date;
}
bool operator==(const m_DataPair &a, const m_DataPair &b) {
  return a.m_date == b.m_date &&
         Adcirc::FpCompare::equalTo(a.m_data_u, b.m_data_u) &&
         Adcirc::FpCompare::equalTo(a.m_data_v, b.m_data_v);
}

HmdfStation::HmdfStation(bool isVector) {
  this->m_coordinate = Coordinate();
  this->m_name = "noname";
  this->m_id = "noid";
  this->m_isNull = true;
  this->m_stationIndex = 0;
  this->m_nullValue = HmdfStation::nullDataValue();
  this->m_isVector = isVector;
  this->m_positiveDirection = 0;
}

void HmdfStation::clear() {
  this->m_coordinate = Coordinate();
  this->m_name = "noname";
  this->m_id = "noid";
  this->m_isNull = true;
  this->m_stationIndex = 0;
  this->m_isVector = false;
  this->m_positiveDirection = 0;
  return;
}

std::string HmdfStation::name() const { return this->m_name; }

void HmdfStation::setName(const std::string &name) { this->m_name = name; }

std::string HmdfStation::id() const { return this->m_id; }

void HmdfStation::setId(const std::string &id) { this->m_id = id; }

size_t HmdfStation::numSnaps() const { return this->m_data_u.size(); }

size_t HmdfStation::stationIndex() const { return this->m_stationIndex; }

void HmdfStation::setStationIndex(size_t stationIndex) {
  this->m_stationIndex = stationIndex;
}

Adcirc::CDate HmdfStation::date(size_t index) const {
  assert(index < this->numSnaps());
  if (index < this->numSnaps())
    return this->m_date[index];
  else {
    adcircmodules_throw_exception("Out of bounds");
    return Adcirc::CDate();
  }
}

double HmdfStation::data(size_t index) const {
  assert(index < this->numSnaps());
  assert(!this->m_isVector);
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve scalar data from vector station.");
  }
  if (index < this->numSnaps())
    return this->m_data_u[index];
  else {
    adcircmodules_throw_exception("Out of bounds");
    return 0;
  }
}

double HmdfStation::data_u(size_t index) const {
  assert(this->isVector());
  assert(index < this->m_data_u.size());
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve vector data from scalar station.");
  }
  if (index < this->numSnaps())
    return this->m_data_u[index];
  else
    return 0;
}

double HmdfStation::data_v(size_t index) const {
  assert(this->isVector());
  assert(index < this->m_data_v.size());
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve vector data from scalar station.");
  }
  if (index < this->numSnaps())
    return this->m_data_v[index];
  else
    return 0;
}

void HmdfStation::setData(const double &data, size_t index) {
  assert(index < this->numSnaps());
  assert(!this->m_isVector);
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign scalar data to vector station.");
    return;
  }
  if (index < this->numSnaps()) this->m_data_u[index] = data;
}

void HmdfStation::setDate(const Adcirc::CDate &date, size_t index) {
  assert(index < this->numSnaps());
  if (index < this->numSnaps()) this->m_date[index] = date;
}

bool HmdfStation::isNull() const { return this->m_isNull; }

void HmdfStation::setIsNull(bool isNull) { this->m_isNull = isNull; }

void HmdfStation::setDate(const std::vector<Adcirc::CDate> &date) {
  this->m_date = date;
  return;
}

void HmdfStation::setData(const std::vector<double> &data) {
  assert(!this->m_isVector);
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign scalar data to vector station.");
    return;
  }
  this->m_data_u = data;
  return;
}

void HmdfStation::setData(const std::vector<float> &data) {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign scalar data to vector station.");
    return;
  }
  this->m_data_u.resize(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    this->m_data_u[i] = static_cast<double>(data[i]);
  }
  return;
}

void HmdfStation::setData(const std::vector<double> &data_u,
                          const std::vector<double> &data_v) {
  assert(this->m_isVector);
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign vector data to scalar station.");
    return;
  }
  this->m_data_u = data_u;
  this->m_data_v = data_v;
  return;
}

void HmdfStation::setData(const std::vector<float> &data_u,
                          const std::vector<float> &data_v) {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign vector data to scalar station.");
    return;
  }
  assert(data_u.size() == data_v.size());
  if (data_u.size() != data_v.size()) {
    adcircmodules_throw_exception(
        "Cannot assign different data sizes for vector components");
  }
  this->m_data_u.resize(data_u.size());
  for (size_t i = 0; i < data_u.size(); ++i) {
    this->m_data_u[i] = static_cast<double>(data_u[i]);
    this->m_data_v[i] = static_cast<double>(data_v[i]);
  }
  return;
}

void HmdfStation::setNext(const Adcirc::CDate &date, const double &data) {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign scalar data to vector station.");
    return;
  }
  this->m_date.push_back(date);
  this->m_data_u.push_back(data);
}

void HmdfStation::setNext(const Adcirc::CDate &date, const double &data_u,
                          const double &data_v) {
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign vector data to scalar station.");
    return;
  }
  this->m_date.push_back(date);
  this->m_data_u.push_back(data_u);
  this->m_data_v.push_back(data_v);
}

void HmdfStation::setNext(const Adcirc::CDate &date,
                          const std::tuple<double, double> &data) {
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to assign vector data to scalar station.");
    return;
  }
  this->m_date.push_back(date);
  this->m_data_u.push_back(std::get<0>(data));
  this->m_data_v.push_back(std::get<1>(data));
}

std::vector<Adcirc::CDate> HmdfStation::allDate() const { return this->m_date; }

std::vector<double> HmdfStation::allData() const {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve scalar data from vector station.");
  }
  return this->m_data_u;
}

std::vector<double> HmdfStation::allData_u() const {
  assert(this->m_isVector);
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve scalar data from vector station");
  }
  return this->m_data_u;
}

std::vector<double> HmdfStation::allData_v() const {
  assert(this->m_isVector);
  if (!this->m_isVector) {
    adcircmodules_throw_exception(
        "Attempt to retrieve scalar data from vector station");
  }
  return this->m_data_v;
}

void HmdfStation::setLatitude(const double latitude) {
  this->m_coordinate.latitude = latitude;
}

void HmdfStation::setLongitude(const double longitude) {
  this->m_coordinate.longitude = longitude;
}

double HmdfStation::latitude() const { return this->m_coordinate.latitude; }

double HmdfStation::longitude() const { return this->m_coordinate.longitude; }

void HmdfStation::setCoordinate(const Coordinate coordinate) {
  this->m_coordinate = coordinate;
}

HmdfStation::Coordinate *HmdfStation::coordinate() {
  return &this->m_coordinate;
}

std::tuple<double, double> HmdfStation::getVectorBounds(
    const std::vector<double> &v) {
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

double HmdfStation::positiveDirection() const {
  return this->m_positiveDirection;
}

void HmdfStation::setPositiveDirection(double positiveDirection) {
  if (positiveDirection < -360.0 || positiveDirection > 360.0) {
    adcircmodules_throw_exception("Positive direction is out of bounds");
  } else if (positiveDirection < -180.0) {
    positiveDirection += 360.0;
  } else if (positiveDirection > 180.0) {
    positiveDirection -= 360.0;
  }
  this->m_positiveDirection = positiveDirection;
}

void HmdfStation::sanitize(const double minValid, const double maxValid) {
  std::vector<m_DataPair> d(this->m_date.size());
  for (size_t i = 0; i < d.size(); ++i) {
    d[i].m_date = this->m_date[i];
    d[i].m_data_u = this->m_data_u[i];
    if (this->m_isVector) {
      d[i].m_data_v = this->m_data_v[i];
    }
  }
  std::sort(d.begin(), d.end());
  for (size_t i = 0; i < d.size(); ++i) {
    this->m_date[i] = d[i].m_date;
    this->m_data_u[i] =
        d[i].m_data_u < minValid
            ? this->nullDataValue()
            : d[i].m_data_u > maxValid ? this->nullDataValue() : d[i].m_data_u;
    if (this->m_isVector) {
      this->m_data_v[i] = d[i].m_data_v < minValid
                              ? this->nullDataValue()
                              : d[i].m_data_v > maxValid ? this->nullDataValue()
                                                         : d[i].m_data_v;
    }
  }
  return;
}

void HmdfStation::dataBounds(Adcirc::CDate &minDate, Adcirc::CDate &maxDate,
                             double &minValue, double &maxValue) {
  minDate = *std::min_element(this->m_date.begin(), this->m_date.end());
  maxDate = *std::max_element(this->m_date.begin(), this->m_date.end());
  if (this->isVector()) {
    double umin, umax, vmin, vmax;
    std::tie(umin, umax) = this->getVectorBounds(this->m_data_u);
    std::tie(vmin, vmax) = this->getVectorBounds(this->m_data_v);
    minValue = std::min(umin, vmin);
    maxValue = std::min(umax, vmax);
  } else {
    std::tie(minValue, maxValue) = this->getVectorBounds(this->m_data_u);
  }
  return;
}

double HmdfStation::nullValue() const { return this->m_nullValue; }

void HmdfStation::setNullValue(double nullValue) {
  this->m_nullValue = nullValue;
}

void HmdfStation::reserve(size_t size) {
  this->m_data_u.reserve(size);
  this->m_date.reserve(size);
  if (this->m_isVector) this->m_data_v.reserve(size);
}

void HmdfStation::setVector(const bool vector) {
  this->m_data_u.clear();
  this->m_date.clear();
  this->m_data_v.clear();
  this->m_isVector = vector;
}

bool HmdfStation::isVector() const { return this->m_isVector; }
