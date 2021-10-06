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

#include "AdcircModules_Global.h"
#include "HmdfStation.h"

namespace Adcirc {

namespace Output {

class Hmdf {
 public:
  ADCIRCMODULES_EXPORT explicit Hmdf(size_t dimension = 1);

  ADCIRCMODULES_EXPORT Hmdf(size_t dimension,
                            const Adcirc::Output::Hmdf *stations);

  void ADCIRCMODULES_EXPORT clear();

  enum HmdfFileType { HmdfImeds, HmdfCsv, HmdfNetCdf, HmdfAdcirc };

  int ADCIRCMODULES_EXPORT write(const std::string &filename,
                                 HmdfFileType fileType);
  int ADCIRCMODULES_EXPORT write(const std::string &filename);
  int ADCIRCMODULES_EXPORT writeImeds(const std::string &filename);
  int ADCIRCMODULES_EXPORT writeCsv(const std::string &filename);
  int ADCIRCMODULES_EXPORT writeNetcdf(const std::string &filename);
  int ADCIRCMODULES_EXPORT writeAdcirc(const std::string &filename);

  int ADCIRCMODULES_EXPORT readImeds(const std::string &filename);
  int ADCIRCMODULES_EXPORT readNetcdf(const std::string &filename,
                                      bool stationsOnly = false);

  void ADCIRCMODULES_EXPORT
  copyStationList(Adcirc::Output::Hmdf &templateStations);

  size_t ADCIRCMODULES_EXPORT nstations() const;

  std::string ADCIRCMODULES_EXPORT header1() const;
  void ADCIRCMODULES_EXPORT setHeader1(const std::string &header1);

  std::string ADCIRCMODULES_EXPORT header2() const;
  void ADCIRCMODULES_EXPORT setHeader2(const std::string &header2);

  std::string ADCIRCMODULES_EXPORT header3() const;
  void ADCIRCMODULES_EXPORT setHeader3(const std::string &header3);

  std::string ADCIRCMODULES_EXPORT units() const;
  void ADCIRCMODULES_EXPORT setUnits(const std::string &units);

  std::string ADCIRCMODULES_EXPORT datum() const;
  void ADCIRCMODULES_EXPORT setDatum(const std::string &datum);

  Adcirc::Output::HmdfStation ADCIRCMODULES_EXPORT *station(size_t index);
  const Adcirc::Output::HmdfStation ADCIRCMODULES_EXPORT *station(
      size_t index) const;

  void ADCIRCMODULES_EXPORT setStation(size_t index,
                                       Adcirc::Output::HmdfStation &station);
  void ADCIRCMODULES_EXPORT
  addStation(const Adcirc::Output::HmdfStation &station);

  bool ADCIRCMODULES_EXPORT success() const;
  void ADCIRCMODULES_EXPORT setSuccess(bool success);

  bool ADCIRCMODULES_EXPORT null() const;
  void ADCIRCMODULES_EXPORT setNull(bool null);

  void ADCIRCMODULES_EXPORT dataBounds(Adcirc::CDate &dateMin,
                                       Adcirc::CDate &dateMax, double &minValue,
                                       double &maxValue);

  size_t ADCIRCMODULES_EXPORT dimension() const;

  static Adcirc::Output::Hmdf::HmdfFileType ADCIRCMODULES_EXPORT
  getFiletype(const std::string &filename);

  int ADCIRCMODULES_EXPORT getEpsg() const;
  void ADCIRCMODULES_EXPORT setEpsg(int epsg);

  void ADCIRCMODULES_EXPORT reproject(int epsg);

 private:
  //...Variables
  bool m_success, m_null;

  int m_epsg;
  const size_t m_dimension;

  std::string m_header1;
  std::string m_header2;
  std::string m_header3;
  std::string m_units;
  std::string m_datum;
  std::vector<Adcirc::Output::HmdfStation> m_station;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_HMDF_H
