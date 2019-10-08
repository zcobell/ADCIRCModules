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

#include <limits>
#include <numeric>
#include <string>
#include <vector>

namespace Adcirc {

namespace Output {

class HmdfStation {
 public:
  struct Coordinate {
    double latitude;
    double longitude;
  };

  explicit HmdfStation();

  void clear();

  static constexpr double nullDataValue() {
    return -std::numeric_limits<double>::max();
  }

  static constexpr double nullDateValue() {
    return -std::numeric_limits<long long>::max();
  }

  Coordinate *coordinate();
  void setCoordinate(const Coordinate coordinate);

  double latitude() const;
  double longitude() const;

  void setLatitude(const double latitude);
  void setLongitude(const double longitude);

  std::string name() const;
  void setName(const std::string &name);

  std::string id() const;
  void setId(const std::string &id);

  size_t numSnaps() const;

  size_t stationIndex() const;
  void setStationIndex(size_t stationIndex);

  long long date(size_t index) const;
  void setDate(const long long &date, size_t index);
  void setDate(const std::vector<long long> &date);

  void setNext(const long long &date, const double &data);

  bool isNull() const;
  void setIsNull(bool isNull);

  double data(size_t index) const;
  void setData(const double &data, size_t index);
  void setData(const std::vector<double> &data);
  void setData(const std::vector<float> &data);

  std::vector<long long> allDate() const;
  std::vector<double> allData() const;

  void dataBounds(long long &minDate, long long &maxDate, double &minValue,
                  double &maxValue);

  double nullValue() const;
  void setNullValue(double nullValue);

 private:
  Coordinate m_coordinate;

  std::string m_name;
  std::string m_id;

  size_t m_stationIndex;

  double m_nullValue;

  std::vector<long long> m_date;
  std::vector<double> m_data;

  bool m_isNull;
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_HMDFSTATION
