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

HmdfStation::HmdfStation() {
  this->m_coordinate = Coordinate();
  this->m_name = "noname";
  this->m_id = "noid";
  this->m_isNull = true;
  this->m_stationIndex = 0;
  this->m_nullValue = HmdfStation::nullDataValue();
}

void HmdfStation::clear() {
  this->m_coordinate = Coordinate();
  this->m_name = "noname";
  this->m_id = "noid";
  this->m_isNull = true;
  this->m_stationIndex = 0;
  this->m_data.clear();
  this->m_date.clear();
  return;
}

std::string HmdfStation::name() const { return this->m_name; }

void HmdfStation::setName(const std::string &name) { this->m_name = name; }

std::string HmdfStation::id() const { return this->m_id; }

void HmdfStation::setId(const std::string &id) { this->m_id = id; }

size_t HmdfStation::numSnaps() const { return this->m_data.size(); }

size_t HmdfStation::stationIndex() const { return this->m_stationIndex; }

void HmdfStation::setStationIndex(size_t stationIndex) {
  this->m_stationIndex = stationIndex;
}

long long HmdfStation::date(size_t index) const {
  assert(index < this->numSnaps());
  if (index < this->numSnaps())
    return this->m_date[index];
  else
    return 0;
}

double HmdfStation::data(size_t index) const {
  assert(index < this->numSnaps());
  if (index < this->numSnaps())
    return this->m_data[index];
  else
    return 0;
}

void HmdfStation::setData(const double &data, size_t index) {
  assert(index < this->numSnaps());
  if (index < this->numSnaps()) this->m_data[index] = data;
}

void HmdfStation::setDate(const long long &date, size_t index) {
  assert(index < this->numSnaps());
  if (index < this->numSnaps()) this->m_date[index] = date;
}

bool HmdfStation::isNull() const { return this->m_isNull; }

void HmdfStation::setIsNull(bool isNull) { this->m_isNull = isNull; }

void HmdfStation::setDate(const std::vector<long long> &date) {
  this->m_date = date;
  return;
}

void HmdfStation::setData(const std::vector<double> &data) {
  this->m_data = data;
  return;
}

void HmdfStation::setData(const std::vector<float> &data) {
  this->m_data.resize(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    this->m_data[i] = static_cast<double>(data[i]);
  }
  return;
}

void HmdfStation::setNext(const long long &date, const double &data) {
  this->m_date.push_back(date);
  this->m_data.push_back(data);
}

std::vector<long long> HmdfStation::allDate() const { return this->m_date; }

std::vector<double> HmdfStation::allData() const { return this->m_data; }

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

void HmdfStation::dataBounds(long long &minDate, long long &maxDate,
                             double &minValue, double &maxValue) {
  minDate = *std::min_element(this->m_date.begin(), this->m_date.end());
  maxDate = *std::max_element(this->m_date.begin(), this->m_date.end());

  std::vector<double> sortedData = this->m_data;
  std::sort(sortedData.begin(), sortedData.end());

  if (FpCompare::equalTo(sortedData.front(), sortedData.back())) {
    minValue = *std::upper_bound(sortedData.begin(), sortedData.end(),
                                 this->nullDataValue());
  } else {
    minValue = sortedData.front();
  }
  maxValue = sortedData.back();

  return;
}

double HmdfStation::nullValue() const { return this->m_nullValue; }

void HmdfStation::setNullValue(double nullValue) {
  this->m_nullValue = nullValue;
}
