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
#include <tuple>
#include <vector>
#include "fpcompare.h"

#include "cdate.h"

namespace Adcirc {

namespace Output {

class HmdfStation {
 public:
  struct Coordinate {
    double latitude;
    double longitude;
  };

  explicit ADCIRCMODULES_EXPORT HmdfStation(bool isVector = false);

  void ADCIRCMODULES_EXPORT clear();

  static constexpr double ADCIRCMODULES_EXPORT nullDataValue() {
    return -std::numeric_limits<double>::max();
  }

  static constexpr double ADCIRCMODULES_EXPORT nullDateValue() {
    return -std::numeric_limits<long long>::max();
  }

  Coordinate ADCIRCMODULES_EXPORT *coordinate();
  void ADCIRCMODULES_EXPORT setCoordinate(const Coordinate coordinate);

  double ADCIRCMODULES_EXPORT latitude() const;
  double ADCIRCMODULES_EXPORT longitude() const;

  void ADCIRCMODULES_EXPORT setLatitude(const double latitude);
  void ADCIRCMODULES_EXPORT setLongitude(const double longitude);

  std::string ADCIRCMODULES_EXPORT name() const;
  void ADCIRCMODULES_EXPORT setName(const std::string &name);

  std::string ADCIRCMODULES_EXPORT id() const;
  void ADCIRCMODULES_EXPORT setId(const std::string &id);

  size_t ADCIRCMODULES_EXPORT numSnaps() const;

  size_t ADCIRCMODULES_EXPORT stationIndex() const;
  void ADCIRCMODULES_EXPORT setStationIndex(size_t stationIndex);

  Adcirc::CDate ADCIRCMODULES_EXPORT date(size_t index) const;
  void ADCIRCMODULES_EXPORT setDate(const Adcirc::CDate &date, size_t index);
  void ADCIRCMODULES_EXPORT setDate(const std::vector<Adcirc::CDate> &date);

  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const double &data);
  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const double &data_u, const double &data_v);
  void ADCIRCMODULES_EXPORT setNext(const Adcirc::CDate &date,
                                    const std::tuple<double, double> &data);

  bool ADCIRCMODULES_EXPORT isNull() const;
  void ADCIRCMODULES_EXPORT setIsNull(bool isNull);

  double ADCIRCMODULES_EXPORT data(size_t index) const;
  double ADCIRCMODULES_EXPORT data_u(size_t index) const;
  double ADCIRCMODULES_EXPORT data_v(size_t index) const;

  void ADCIRCMODULES_EXPORT setData(const double &data, size_t index);
  void ADCIRCMODULES_EXPORT setData(const std::vector<double> &data);
  void ADCIRCMODULES_EXPORT setData(const std::vector<float> &data);
  void ADCIRCMODULES_EXPORT setData(const std::vector<double> &data_u,
                                    const std::vector<double> &data_v);
  void ADCIRCMODULES_EXPORT setData(const std::vector<float> &data_u,
                                    const std::vector<float> &data_v);

  std::vector<Adcirc::CDate> ADCIRCMODULES_EXPORT allDate() const;
  std::vector<double> ADCIRCMODULES_EXPORT allData() const;
  std::vector<double> ADCIRCMODULES_EXPORT allData_u() const;
  std::vector<double> ADCIRCMODULES_EXPORT allData_v() const;

  void ADCIRCMODULES_EXPORT dataBounds(Adcirc::CDate &minDate,
                                       Adcirc::CDate &maxDate, double &minValue,
                                       double &maxValue);

  double ADCIRCMODULES_EXPORT nullValue() const;
  void ADCIRCMODULES_EXPORT setNullValue(double nullValue);

  void ADCIRCMODULES_EXPORT reserve(size_t size);

  void ADCIRCMODULES_EXPORT setVector(const bool vector);
  bool ADCIRCMODULES_EXPORT isVector() const;

  double ADCIRCMODULES_EXPORT positiveDirection() const;
  void ADCIRCMODULES_EXPORT setPositiveDirection(double positiveDirection);

  void ADCIRCMODULES_EXPORT sanitize(const double minValid,
                                     const double maxValid);

 private:
  struct m_DataPair {
    Adcirc::CDate m_date;
    double m_data_u = HmdfStation::nullDataValue();
    double m_data_v = HmdfStation::nullDataValue();

    bool operator<(const m_DataPair &a) { return this->m_date < a.m_date; }
    bool operator==(const m_DataPair &a) {
      return this->m_date == a.m_date &&
             FpCompare::equalTo(this->m_data_u, a.m_data_u) &&
             FpCompare::equalTo(this->m_data_v, a.m_data_v);
    }
  };

  std::tuple<double, double> getVectorBounds(const std::vector<double> &v);

  Coordinate m_coordinate;

  std::string m_name;
  std::string m_id;

  size_t m_stationIndex;

  double m_nullValue;
  double m_positiveDirection;

  std::vector<Adcirc::CDate> m_date;
  std::vector<double> m_data_u;
  std::vector<double> m_data_v;

  bool m_isNull;
  bool m_isVector;
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_HMDFSTATION
