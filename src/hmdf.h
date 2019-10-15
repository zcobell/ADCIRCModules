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
#ifndef ADCMOD_HMDF_H
#define ADCMOD_HMDF_H

#include <limits>
#include <string>
#include <vector>
#include "hmdfstation.h"

namespace Adcirc {

namespace Output {

class Hmdf {
 public:
  explicit Hmdf();

  void clear();

  enum HmdfFileType { HmdfImeds, HmdfCsv, HmdfNetCdf };

  int write(const std::string &filename, HmdfFileType fileType);
  int write(const std::string &filename);
  int writeImeds(const std::string &filename);
  int writeCsv(const std::string &filename);
  int writeNetcdf(const std::string &filename);

  int readImeds(const std::string &filename);
  int readNetcdf(const std::string &filename, bool stationsOnly = false);

  size_t nstations() const;
  // void setNstations(size_t nstations);

  std::string header1() const;
  void setHeader1(const std::string &header1);

  std::string header2() const;
  void setHeader2(const std::string &header2);

  std::string header3() const;
  void setHeader3(const std::string &header3);

  std::string units() const;
  void setUnits(const std::string &units);

  std::string datum() const;
  void setDatum(const std::string &datum);

  HmdfStation *station(size_t index);
  void setStation(size_t index, HmdfStation &station);
  void addStation(const HmdfStation &station);

  bool success() const;
  void setSuccess(bool success);

  bool null() const;
  void setNull(bool null);

  void dataBounds(long long &dateMin, long long &dateMax, double &minValue,
                  double &maxValue);

 private:
  void init();

  //...Variables
  bool m_success, m_null;

  std::string m_header1;
  std::string m_header2;
  std::string m_header3;
  std::string m_units;
  std::string m_datum;
  std::vector<HmdfStation> m_station;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_HMDF_H
