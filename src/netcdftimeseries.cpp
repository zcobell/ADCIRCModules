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
#include "netcdftimeseries.h"
#include <memory>
#include "boost/format.hpp"
#include "cdate.h"
#include "fileio.h"
#include "netcdf.h"

using namespace Adcirc::Output;

#define NCCHECK(ierr)     \
  if (ierr != NC_NOERR) { \
    nc_close(ncid);       \
    return ierr;          \
  }

NetcdfTimeseries::NetcdfTimeseries() {
  this->m_filename = std::string();
  this->m_epsg = 4326;
  this->m_units = "unknown";
  this->m_verticalDatum = "unknown";
  this->m_horizontalProjection = "WGS84";
  this->m_numStations = 0;
  this->m_hasData = false;
}

std::string NetcdfTimeseries::filename() const { return this->m_filename; }

void NetcdfTimeseries::setFilename(const std::string &filename) {
  this->m_filename = filename;
}

int NetcdfTimeseries::epsg() const { return this->m_epsg; }

void NetcdfTimeseries::setEpsg(int epsg) { this->m_epsg = epsg; }

int NetcdfTimeseries::read(bool stationsOnly = false) {
  if (this->m_filename == std::string()) return 1;

  std::string station_dim_string, station_time_var_string,
      station_data_var_string, stationNameString;
  size_t stationNameLength, length;
  int ncid;
  int dimid_nstations, dimidStationLength, dimid_stationNameLen;
  int varid_time, varid_data, varid_xcoor, varid_ycoor, varid_stationName;
  int epsg;
  char timeChar[80];

  NCCHECK(nc_open(this->m_filename.c_str(), NC_NOWRITE, &ncid))
  NCCHECK(nc_inq_dimid(ncid, "numStations", &dimid_nstations))
  NCCHECK(nc_inq_dimlen(ncid, dimid_nstations, &this->m_numStations))
  NCCHECK(nc_inq_dimid(ncid, "stationNameLen", &dimid_stationNameLen))
  NCCHECK(nc_inq_dimlen(ncid, dimid_stationNameLen, &stationNameLength))
  NCCHECK(nc_inq_varid(ncid, "stationXCoordinate", &varid_xcoor))
  NCCHECK(nc_inq_varid(ncid, "stationYCoordinate", &varid_ycoor))
  NCCHECK(nc_inq_varid(ncid, "stationName", &varid_stationName))
  NCCHECK(nc_get_att_int(ncid, varid_xcoor, "HorizontalProjectionEPSG", &epsg))

  this->setEpsg(epsg);

  std::unique_ptr<double> xcoor(new double[this->m_numStations]);
  std::unique_ptr<double> ycoor(new double[this->m_numStations]);

  NCCHECK(nc_get_var_double(ncid, varid_xcoor, xcoor.get()))
  NCCHECK(nc_get_var_double(ncid, varid_ycoor, ycoor.get()))

  for (size_t i = 0; i < this->m_numStations; i++) {
    this->m_xcoor.push_back(xcoor.get()[i]);
    this->m_ycoor.push_back(ycoor.get()[i]);
  }

  xcoor.reset(nullptr);
  ycoor.reset(nullptr);

  std::unique_ptr<char> stationName(
      new char[stationNameLength * this->m_numStations]);
  NCCHECK(nc_get_var_text(ncid, varid_stationName, stationName.get()))
  stationNameString = std::string(stationName.get());
  FileIO::Generic::sanitizeString(stationNameString);

  for (size_t i = 0; i < this->m_numStations; i++) {
    std::string s = stationNameString.substr(200 * i, 200);
    this->m_stationName.push_back(s);
  }

  if (!stationsOnly) {
    this->m_time.resize(this->m_numStations);
    this->m_data.resize(this->m_numStations);
  }

  for (size_t i = 0; i < this->m_numStations; i++) {
    std::string station_dim_string =
        boost::str(boost::format("stationLength_%04.4i") % (i + 1));
    std::string station_time_var_string =
        boost::str(boost::format("time_station_%04.4i") % (i + 1));
    std::string station_data_var_string =
        boost::str(boost::format("time_station_%04.4i") % (i + 1));

    NCCHECK(nc_inq_dimid(ncid, station_dim_string.c_str(), &dimidStationLength))
    NCCHECK(nc_inq_dimlen(ncid, dimidStationLength, &length))
    this->m_stationLength.push_back(length);

    NCCHECK(nc_inq_varid(ncid, station_time_var_string.c_str(), &varid_time))
    NCCHECK(nc_inq_varid(ncid, station_data_var_string.c_str(), &varid_data))
    NCCHECK(nc_get_att_text(ncid, varid_time, "referenceDate", timeChar))
    std::string timeString = std::string(timeChar).substr(0, 19);

    Date reftime;
    reftime.fromString(timeString);

    double fillValue;
    NCCHECK(nc_inq_var_fill(ncid, varid_data, NULL, &fillValue))
    if (fillValue == NC_FILL_DOUBLE) fillValue = -99999.0;
    this->m_fillValue.push_back(fillValue);

    if (!stationsOnly) {
      std::unique_ptr<long long> timeData(new long long[length]);
      std::unique_ptr<double> varData(new double[length]);

      NCCHECK(nc_get_var_double(ncid, varid_data, varData.get()))
      NCCHECK(nc_get_var_longlong(ncid, varid_time, timeData.get()))

      this->m_data[i].resize(length);
      this->m_time[i].resize(length);

      for (size_t j = 0; j < length; j++) {
        this->m_data[i][j] = varData.get()[j];
        Date t = reftime;
        t.add(timeData.get()[j]);
        this->m_time[i][j] = t.toMSeconds();
      }
    }
  }

  NCCHECK(nc_close(ncid))

  this->m_hasData = !stationsOnly;
  return 0;
}

int NetcdfTimeseries::toHmdf(Hmdf *hmdf) {
  hmdf->setDatum("unknown");
  hmdf->setHeader1("none");
  hmdf->setHeader2("none");
  hmdf->setHeader3("none");
  hmdf->setSuccess(false);

  for (size_t i = 0; i < this->m_numStations; i++) {
    HmdfStation station;
    if (this->m_hasData) {
      station.setDate(this->m_time[i]);
      station.setData(this->m_data[i]);
    }
    station.setLatitude(this->m_ycoor[i]);
    station.setLongitude(this->m_xcoor[i]);
    station.setName(this->m_stationName[i]);
    station.setId(this->m_stationName[i]);
    station.setStationIndex(i);
    station.setNullValue(this->m_fillValue[i]);
    hmdf->addStation(station);
  }

  hmdf->setSuccess(true);

  return 0;
}

int NetcdfTimeseries::getEpsg(const std::string &file) {
  int ncid, varid_xcoor, epsg;
  NCCHECK(nc_open(file.c_str(), NC_NOWRITE, &ncid))
  NCCHECK(nc_inq_varid(ncid, "stationXCoordinate", &varid_xcoor))
  NCCHECK(nc_get_att_int(ncid, varid_xcoor, "HorizontalProjectionEPSG", &epsg))
  NCCHECK(nc_close(ncid))
  return epsg;
}
