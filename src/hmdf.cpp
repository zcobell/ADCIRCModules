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
#include "hmdf.h"
#include <fstream>
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/format.hpp"
#include "cdate.h"
#include "fileio.h"
#include "netcdf.h"
#include "netcdftimeseries.h"

using namespace Adcirc::Output;

#define NCCHECK(ierr)     \
  if (ierr != NC_NOERR) { \
    nc_close(ncid);       \
    return ierr;          \
  }

Hmdf::Hmdf() { this->init(); }

void Hmdf::init() {
  this->setHeader1("");
  this->setHeader2("");
  this->setHeader3("");
  this->setDatum("");
  this->setSuccess(false);
  this->setUnits("");
  this->setNull(true);
  return;
}

void Hmdf::clear() {
  this->m_station.clear();
  this->init();
  return;
}

size_t Hmdf::nstations() const { return this->m_station.size(); }

std::string Hmdf::header1() const { return this->m_header1; }

void Hmdf::setHeader1(const std::string &header1) { this->m_header1 = header1; }

std::string Hmdf::header2() const { return this->m_header2; }

void Hmdf::setHeader2(const std::string &header2) { this->m_header2 = header2; }

std::string Hmdf::header3() const { return this->m_header3; }

void Hmdf::setHeader3(const std::string &header3) { this->m_header3 = header3; }

std::string Hmdf::units() const { return this->m_units; }

void Hmdf::setUnits(const std::string &units) { this->m_units = units; }

std::string Hmdf::datum() const { return this->m_datum; }

void Hmdf::setDatum(const std::string &datum) { this->m_datum = datum; }

HmdfStation *Hmdf::station(size_t index) {
  assert(index < this->m_station.size());
  return &this->m_station[index];
}

void Hmdf::setStation(size_t index, HmdfStation &station) {
  assert(index < this->m_station.size());
  this->m_station[index] = station;
}

void Hmdf::addStation(const HmdfStation &station) {
  this->m_station.push_back(station);
}

bool Hmdf::success() const { return this->m_success; }

void Hmdf::setSuccess(bool success) { this->m_success = success; }

bool Hmdf::null() const { return this->m_null; }

void Hmdf::setNull(bool null) { this->m_null = null; }

int Hmdf::readImeds(const std::string &filename) {
  std::fstream fid(filename.c_str());
  if (fid.bad()) return -1;

  //...Read Header
  std::string templine;
  std::getline(fid, templine);
  this->m_header1 = Adcirc::FileIO::Generic::sanitizeString(templine);
  std::getline(fid, templine);
  this->m_header2 = Adcirc::FileIO::Generic::sanitizeString(templine);
  std::getline(fid, templine);
  this->m_header3 = Adcirc::FileIO::Generic::sanitizeString(templine);

  //...Read Body
  std::getline(fid, templine);

  while (!fid.eof()) {
    HmdfStation station;

    templine = Adcirc::FileIO::Generic::sanitizeString(templine);

    std::vector<std::string> templist;
    Adcirc::FileIO::Generic::splitString(templine, templist);

    station.setName(templist[0]);
    station.setLongitude(stod(templist[2]));
    station.setLatitude(stod(templist[1]));

    while (!fid.eof()) {
      std::getline(fid, templine);

      int year, month, day, hour, minute, second;
      double value;

      bool status = Adcirc::FileIO::HMDFIO::splitStringHmdfFormat(
          templine, year, month, day, hour, minute, second, value);

      if (status) {
        Date d(year, month, day, hour, minute, second);

        //...Append to the station data
        station.setNext(d.toMSeconds(), value);
      } else {
        break;
      }
    }

    //...Add the station
    this->addStation(station);
  }

  this->setNull(false);

  return 0;
}

int Hmdf::readNetcdf(const std::string &filename) {
  NetcdfTimeseries ncts;
  ncts.setFilename(filename);
  int ierr = ncts.read();
  if (ierr != 0) {
    return 1;
  }
  ierr = ncts.toHmdf(this);

  if (ierr != 0) return 1;

  this->setNull(false);

  return 0;
}

int Hmdf::writeCsv(const std::string &filename) {
  std::string value;
  std::ofstream out(filename);

  for (size_t s = 0; s < this->nstations(); ++s) {
    out << boost::str(boost::format("Station %4.4i\n") % (s + 1));
    out << boost::str(boost::format("Datum: %s\n") % this->datum());
    out << boost::str(boost::format("Units: %s\n") % this->units());

    for (size_t i = 0; i < this->station(s)->numSnaps(); i++) {
      Date d;
      d.fromSeconds(this->station(s)->date(i));
      double value = this->station(s)->data(i);
      out << boost::str(boost::format("%s,%10.4e\n") % d.toString() % value);
    }
    out << "\n\n\n";
  }
  out.close();
  return 0;
}

int Hmdf::writeImeds(const std::string &filename) {
  std::string value;
  std::ofstream out(filename);

  out << "% IMEDS generic format\n";
  out << "% year month day hour min sec value\n";
  out << "% ADCIRCModules UTC " << this->datum() << " " << this->units()
      << "\n";

  for (size_t s = 0; s < this->nstations(); ++s) {
    std::string stationname = this->station(s)->name();
    boost::algorithm::replace_all(stationname, " ", "_");
    boost::algorithm::replace_all(stationname, ",", "_");
    boost::algorithm::replace_all(stationname, "__", "_");

    out << boost::str(boost::format("%s   %16.10f   %16.10f\n") % stationname %
                      this->station(s)->latitude() %
                      this->station(s)->longitude());

    for (size_t i = 0; i < this->station(s)->numSnaps(); ++i) {
      Date t;
      t.fromSeconds(this->station(s)->date(i));
      double value = this->station(s)->data(i);
      out << boost::str(
          boost::format("%04.4i %02.2i %02.2i %02.2i %02.2i %02.2i %10.6e\n") %
          t.year() % t.month() % t.day() % t.hour() % t.minute() % t.second() %
          value);
    }
  }
  out.close();
  return 0;
}

int Hmdf::writeNetcdf(const std::string &filename) {
  int ncid;
  int dimid_nstations, dimid_stationNameLength;
  int varid_stationName, varid_stationx, varid_stationy;
  int varid_stationId;

  std::vector<int> dimid_stationLength;
  std::vector<int> varid_stationDate, varid_stationData;

  //...Open file
  NCCHECK(nc_create(filename.c_str(), NC_NETCDF4, &ncid))

  //...Dimensions
  NCCHECK(nc_def_dim(ncid, "numStations", this->nstations(), &dimid_nstations))
  NCCHECK(nc_def_dim(ncid, "stationNameLen", 200, &dimid_stationNameLength))
  for (size_t i = 0; i < this->nstations(); i++) {
    std::string dimname =
        boost::str(boost::format("stationLength_%04.4i") % (i + 1));
    int d;
    NCCHECK(nc_def_dim(ncid, dimname.c_str(), this->station(i)->numSnaps(), &d))
    dimid_stationLength.push_back(d);
  }

  //...Variables
  int stationNameDims[2] = {dimid_nstations, dimid_stationNameLength};
  int nstationDims[1] = {dimid_nstations};
  int wgs84[1] = {4326};

  NCCHECK(nc_def_var(ncid, "stationName", NC_CHAR, 2, stationNameDims,
                     &varid_stationName))
  NCCHECK(nc_def_var(ncid, "stationId", NC_CHAR, 2, stationNameDims,
                     &varid_stationId))
  NCCHECK(nc_def_var(ncid, "stationXCoordinate", NC_DOUBLE, 1, nstationDims,
                     &varid_stationx))
  NCCHECK(nc_def_var(ncid, "stationYCoordinate", NC_DOUBLE, 1, nstationDims,
                     &varid_stationy))
  NCCHECK(nc_put_att_text(ncid, varid_stationx, "HorizontalProjectionName", 5,
                          "WGS84"))
  NCCHECK(nc_put_att_text(ncid, varid_stationy, "HorizontalProjectionName", 5,
                          "WGS84"))

  NCCHECK(nc_put_att_int(ncid, varid_stationx, "HorizontalProjectionEPSG",
                         NC_INT, 1, wgs84))
  NCCHECK(nc_put_att_int(ncid, varid_stationy, "HorizontalProjectionEPSG",
                         NC_INT, 1, wgs84))

  for (size_t i = 0; i < this->nstations(); i++) {
    const int d[1] = {dimid_stationLength[i]};
    const char epoch[20] = "1970-01-01 00:00:00";
    const char utc[4] = "utc";
    const char timeunit[27] = "second since referenceDate";
    int v;

    std::string stationName =
        boost::str(boost::format("station_%04.4i") % (i + 1));
    std::string timeVarName = "time_" + stationName;
    std::string dataVarName = "data_" + stationName;

    NCCHECK(nc_def_var(ncid, timeVarName.c_str(), NC_INT64, 1, d, &v))
    NCCHECK(nc_put_att_text(ncid, v, "StationName",
                            this->station(i)->name().length(),
                            this->station(i)->name().c_str()))
    NCCHECK(nc_put_att_text(ncid, v, "StationID",
                            this->station(i)->id().length(),
                            this->station(i)->id().c_str()))
    NCCHECK(nc_put_att_text(ncid, v, "referenceDate", 20, epoch))
    NCCHECK(nc_put_att_text(ncid, v, "timezone", 3, utc))
    NCCHECK(nc_put_att_text(ncid, v, "units", 3, timeunit))
    NCCHECK(nc_def_var_deflate(ncid, v, 1, 1, 2))
    varid_stationDate.push_back(v);

    NCCHECK(nc_def_var(ncid, dataVarName.c_str(), NC_DOUBLE, 1, d, &v))
    NCCHECK(nc_put_att_text(ncid, v, "StationName",
                            this->station(i)->name().length(),
                            this->station(i)->name().c_str()))
    NCCHECK(nc_put_att_text(ncid, v, "StationID",
                            this->station(i)->id().length(),
                            this->station(i)->id().c_str()))
    NCCHECK(nc_put_att_text(ncid, v, "units", this->units().length(),
                            this->units().c_str()))
    NCCHECK(nc_put_att_text(ncid, v, "datum", this->datum().length(),
                            this->datum().c_str()))
    NCCHECK(nc_def_var_deflate(ncid, v, 1, 1, 2))
    varid_stationData.push_back(v);
  }

  //...Metadata
#ifndef _WIN32
  char *h = std::getenv("HOSTNAME");
#else
  char *h = getenv("COMPUTERNAME");
#endif
  std::string host;
  if (h == nullptr) {
    host = "unknown";
  } else {
    host = std::string(h);
  }

  std::string name = std::string(std::getenv("USER"));
  if (name == std::string()) name = std::string(std::getenv("USERNAME"));
  std::string createTime = Date::now().toString();
  std::string source = "ADCIRCModules";
  std::string ncVersion = std::string(nc_inq_libvers());
  std::string format = "20180123";

  NCCHECK(nc_put_att(ncid, NC_GLOBAL, "source", NC_CHAR, source.length(),
                     source.c_str()))
  NCCHECK(nc_put_att(ncid, NC_GLOBAL, "creation_date", NC_CHAR,
                     createTime.length(), createTime.c_str()))
  NCCHECK(nc_put_att(ncid, NC_GLOBAL, "created_by", NC_CHAR, name.length(),
                     name.c_str()))
  NCCHECK(
      nc_put_att(ncid, NC_GLOBAL, "host", NC_CHAR, host.length(), host.c_str()))
  NCCHECK(nc_put_att(ncid, NC_GLOBAL, "netCDF_version", NC_CHAR,
                     ncVersion.length(), ncVersion.c_str()))
  NCCHECK(nc_put_att(ncid, NC_GLOBAL, "fileformat", NC_CHAR, format.length(),
                     format.c_str()))
  NCCHECK(nc_enddef(ncid))

  for (size_t i = 0; i < this->nstations(); i++) {
    size_t index[2] = {i, 0};
    size_t stindex[1] = {i};
    size_t count[2] = {1, 200};
    double lat[1] = {this->station(i)->latitude()};
    double lon[1] = {this->station(i)->longitude()};

    std::unique_ptr<long long> time(
        new long long[this->station(i)->numSnaps()]);
    std::unique_ptr<double> data(new double[this->station(i)->numSnaps()]);
    std::unique_ptr<char> name(new char[200]);
    std::unique_ptr<char> id(new char[200]);

    memset(name.get(), ' ', 200);
    memset(id.get(), ' ', 200);

    this->station(i)->name().copy(name.get(), this->station(i)->name().length(),
                                  0);
    this->station(i)->id().copy(id.get(), this->station(i)->id().length(), 0);

    for (size_t j = 0; j < this->station(i)->numSnaps(); j++) {
      time.get()[j] = this->station(i)->date(j) / 1000;
      data.get()[j] = this->station(i)->data(j);
    }

    NCCHECK(nc_put_var1_double(ncid, varid_stationx, stindex, lon))
    NCCHECK(nc_put_var1_double(ncid, varid_stationy, stindex, lat))
    NCCHECK(nc_put_var_longlong(ncid, varid_stationDate[i], time.get()))
    NCCHECK(nc_put_var_double(ncid, varid_stationData[i], data.get()))
    NCCHECK(nc_put_vara_text(ncid, varid_stationName, index, count, name.get()))
    NCCHECK(nc_put_vara_text(ncid, varid_stationId, index, count, id.get()))
  }

  nc_close(ncid);

  return 0;
}

int Hmdf::write(const std::string &filename, HmdfFileType fileType) {
  if (fileType == HmdfImeds) {
    return this->writeImeds(filename);
  } else if (fileType == HmdfCsv) {
    return this->writeCsv(filename);
  } else if (fileType == HmdfNetCdf) {
    return this->writeNetcdf(filename);
  }
  return 1;
}

int Hmdf::write(const std::string &filename) {
  std::string extension = Adcirc::FileIO::Generic::getFileExtension(filename);
  boost::algorithm::to_lower(extension);
  if (extension == ".imeds") {
    return this->write(filename, HmdfImeds);
  } else if (extension == ".csv") {
    return this->write(filename, HmdfCsv);
  } else if (extension == ".nc") {
    return this->write(filename, HmdfNetCdf);
  }
  return 1;
}

void Hmdf::dataBounds(long long &dateMin, long long &dateMax, double &minValue,
                      double &maxValue) {
  dateMax = std::numeric_limits<long long>::max();
  dateMin = -std::numeric_limits<long long>::max();
  maxValue = -std::numeric_limits<double>::max();
  minValue = std::numeric_limits<double>::max();

  for (size_t i = 0; i < this->nstations(); i++) {
    long long tempDateMin, tempDateMax;
    double tempMinValue, tempMaxValue;
    if (!this->station(i)->isNull()) {
      this->station(i)->dataBounds(tempDateMin, tempDateMax, tempMinValue,
                                   tempMaxValue);
      dateMin = std::min(tempDateMin, dateMin);
      dateMax = std::max(tempDateMax, dateMax);
      minValue = std::min(tempMinValue, minValue);
      maxValue = std::max(tempMaxValue, maxValue);
    }
  }
  return;
}
