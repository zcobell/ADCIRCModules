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

#include <cmath>
#include <fstream>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif

#include <memory>

#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/format.hpp"
#include "cdate.h"
#include "fileio.h"
#include "formatting.h"
#include "logging.h"
#include "netcdf.h"
#include "netcdftimeseries.h"
#include "projection.h"

using namespace Adcirc::Output;

#define NCCHECK(ierr)     \
  if (ierr != NC_NOERR) { \
    nc_close(ncid);       \
    return ierr;          \
  }

Hmdf::Hmdf(bool isVector) : m_isVector(isVector) { this->init(); }

void Hmdf::init() {
  this->setHeader1("");
  this->setHeader2("");
  this->setHeader3("");
  this->setDatum("");
  this->setSuccess(false);
  this->setUnits("");
  this->setNull(true);
  this->setEpsg(-1);
  return;
}

int Hmdf::getEpsg() const { return this->m_epsg; }

void Hmdf::setEpsg(int epsg) { this->m_epsg = epsg; }

void Hmdf::reproject(int epsg) {
  if (this->m_epsg == -1) {
    adcircmodules_throw_exception(
        "Error: Must define projection before reprojecting");
  }

  for (auto &m : this->m_station) {
    double x = m.longitude();
    double y = m.latitude();
    double outx, outy;
    bool islatlon;
    Adcirc::Projection::transform(this->m_epsg, epsg, x, y, outx, outy,
                                  islatlon);
    m.setLongitude(outx);
    m.setLatitude(outy);
  }
  this->setEpsg(epsg);
  return;
}

bool Hmdf::isVector() const { return this->m_isVector; }

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
  if (index < this->m_station.size()) {
    return &this->m_station[index];
  } else {
    std::string error = boost::str(
        boost::format(
            "Index %i greater than number of stations in hmdf object (%i)") %
        index % this->m_station.size());
    adcircmodules_throw_exception(error);
    return nullptr;
  }
}

void Hmdf::setStation(size_t index, HmdfStation &station) {
  assert(index < this->m_station.size());
  this->m_station[index] = station;
}

void Hmdf::addStation(const HmdfStation &station) {
  if (station.isVector() != this->m_isVector) {
    adcircmodules_throw_exception(
        "Cannot mix vector and scalar station types.");
  }
  this->m_station.push_back(station);
}

bool Hmdf::success() const { return this->m_success; }

void Hmdf::setSuccess(bool success) { this->m_success = success; }

bool Hmdf::null() const { return this->m_null; }

void Hmdf::setNull(bool null) { this->m_null = null; }

int Hmdf::readImeds(const std::string &filename) {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "imeds format files cannot contain vector data.");
  }
  std::ifstream fid(filename.c_str());
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
    HmdfStation station(false);

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
        Adcirc::CDate d(year, month, day, hour, minute, second);

        //...Append to the station data
        station.setNext(d, value);
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

int Hmdf::readNetcdf(const std::string &filename, bool stationsOnly) {
  if (this->m_isVector) {
    adcircmodules_throw_exception(
        "generic netcdf format files cannot contain vector data.");
  }
  NetcdfTimeseries ncts;
  ncts.setFilename(filename);
  int ierr = ncts.read(stationsOnly);
  if (ierr != 0) {
    return 1;
  }

  ierr = ncts.toHmdf(this);

  if (ierr != 0) return 1;

  this->setNull(false);

  return 0;
}

void Hmdf::copyStationList(Adcirc::Output::Hmdf &templateStations) {
  this->m_station.clear();
  for (size_t i = 0; i < templateStations.nstations(); ++i) {
    HmdfStation s;
    s.setLatitude(templateStations.station(i)->latitude());
    s.setLongitude(templateStations.station(i)->longitude());
    s.setStationIndex(templateStations.station(i)->stationIndex());
    s.setName(templateStations.station(i)->name());
    s.setId(templateStations.station(i)->id());
    this->addStation(s);
  }
}

int Hmdf::writeCsv(const std::string &filename) {
  std::string value;
  std::ofstream out(filename);

  for (size_t s = 0; s < this->nstations(); ++s) {
    out << boost::str(boost::format("Station %4.4i\n") % (s + 1));
    out << boost::str(boost::format("Datum: %s\n") % this->datum());
    out << boost::str(boost::format("Units: %s\n") % this->units());

    for (size_t i = 0; i < this->station(s)->numSnaps(); i++) {
      if (this->isVector()) {
        double value1 = this->station(s)->data_u(i);
        double value2 = this->station(s)->data_v(i);
        out << boost::str(boost::format("%s,%10.4e,%10.4e\n") %
                          this->station(s)->date(i).toString() % value1 %
                          value2);
      } else {
        double value = this->station(s)->data(i);
        out << boost::str(boost::format("%s,%10.4e\n") %
                          this->station(s)->date(i).toString() % value);
      }
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
      double value = this->station(s)->data(i);
      out << boost::str(
          boost::format("%04.4i %02.2i %02.2i %02.2i %02.2i %02.2i %10.6e\n") %
          this->station(s)->date(i).year() % this->station(s)->date(i).month() %
          this->station(s)->date(i).day() % this->station(s)->date(i).hour() %
          this->station(s)->date(i).minute() %
          this->station(s)->date(i).second() % value);
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
#if defined(__unix__) || defined(__APPLE__)
  char hostname[256];
  gethostname(hostname, 256);
  std::string host(hostname);
#elif _WIN32
  std::string host = std::string(std::getenv("COMPUTERNAME"));
#else
  std::string host = "unknown";
#endif

  char *usr1 = std::getenv("USER");
  char *usr2 = std::getenv("USERNAME");
  std::string name;
  if (usr1 != NULL) {
    name = std::string(usr1);
  } else if (usr2 != NULL) {
    name = std::string(usr2);
  } else {
    name = "none";
  }

  std::string createTime = Adcirc::CDate::now().toString();
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
    size_t index[] = {i, 0};
    size_t stindex[] = {i};
    size_t count[] = {1, this->station(i)->name().size()};
    double lat[] = {this->station(i)->latitude()};
    double lon[] = {this->station(i)->longitude()};

    std::vector<long long> time(this->station(i)->numSnaps());
    for (size_t j = 0; j < this->station(i)->numSnaps(); j++) {
      time[j] = this->station(i)->date(j).toSeconds();
    }

    NCCHECK(nc_put_var1_double(ncid, varid_stationx, stindex, lon))
    NCCHECK(nc_put_var1_double(ncid, varid_stationy, stindex, lat))
    NCCHECK(nc_put_var_longlong(ncid, varid_stationDate[i], time.data()))
    NCCHECK(nc_put_var_double(ncid, varid_stationData[i],
                              this->station(i)->allData().data()))
    NCCHECK(nc_put_vara_text(ncid, varid_stationName, index, count,
                             &this->station(i)->name()[0]))
    NCCHECK(nc_put_vara_text(ncid, varid_stationId, index, count,
                             &this->station(i)->id()[0]))
  }

  nc_close(ncid);

  return 0;
}

int Hmdf::writeAdcirc(const std::string &filename) {
  if (this->nstations() > 1) {
    for (size_t i = 1; i < this->nstations(); ++i) {
      if (this->m_station[0].numSnaps() != this->m_station[i].numSnaps()) {
        adcircmodules_throw_exception(
            "To write adcirc format, all stations must have the same number of "
            "time snaps");
      }
      for (size_t j = 0; j < this->m_station[0].numSnaps(); ++j) {
        if (this->m_station[i].date(j) != this->m_station[0].date(j)) {
          adcircmodules_throw_exception(
              "To write adcirc format, all stations must have the same set of "
              "time snaps");
        }
      }
    }
  }
  std::ofstream out(filename);

  if (this->header1() == std::string()) {
    out << "ADCIRCModules" << std::endl;
  } else {
    out << this->header1() << std::endl;
  }

  double dt = this->station(0)->date(1).toSeconds() -
              this->station(0)->date(0).toSeconds();
  int dit = static_cast<int>(std::floor(dt));

  int nv = this->isVector() ? 2 : 1;
  out << Adcirc::Output::Formatting::adcircFileHeader(
      this->station(0)->numSnaps(), this->nstations(), dt, dit, nv);
  for (size_t i = 0; i < this->station(0)->numSnaps(); ++i) {
    double t = this->station(0)->date(i).toSeconds() -
               this->station(0)->date(0).toSeconds() + dt;
    int it = static_cast<int>(std::floor(t));
    out << Adcirc::Output::Formatting::adcircFullFormatRecordHeader(t, it);
    for (size_t j = 0; j < this->nstations(); ++j) {
      if (this->isVector()) {
        out << Adcirc::Output::Formatting::adcircVectorLineFormat(
            j + 1, this->station(j)->data_u(i), this->station(j)->data_v(i));
      } else {
        out << Adcirc::Output::Formatting::adcircScalarLineFormat(
            j + 1, this->station(j)->data(i));
      }
    }
  }
  out.close();
  return 0;
}

int Hmdf::write(const std::string &filename, HmdfFileType fileType) {
  if (fileType == HmdfImeds) {
    return this->writeImeds(filename);
  } else if (fileType == HmdfCsv) {
    return this->writeCsv(filename);
  } else if (fileType == HmdfNetCdf) {
    return this->writeNetcdf(filename);
  } else if (fileType == HmdfAdcirc) {
    return this->writeAdcirc(filename);
  }
  return 1;
}

Hmdf::HmdfFileType Hmdf::getFiletype(const std::string &filename) {
  std::string extension = Adcirc::FileIO::Generic::getFileExtension(filename);
  boost::algorithm::to_lower(extension);
  if (extension == ".imeds") {
    return HmdfImeds;
  } else if (extension == ".csv") {
    return HmdfCsv;
  } else if (extension == ".nc") {
    return HmdfNetCdf;
  } else if (extension == ".61" || extension == ".62" || extension == ".71" ||
             extension == ".72") {
    return HmdfAdcirc;
  } else {
    adcircmodules_throw_exception("No valid HMDF filetype found");
    return Hmdf::HmdfFileType();
  }
}

int Hmdf::write(const std::string &filename) {
  HmdfFileType ft = this->getFiletype(filename);
  return this->write(filename, ft);
}

void Hmdf::dataBounds(CDate &dateMin, CDate &dateMax, double &minValue,
                      double &maxValue) {
  dateMax = CDate::maxDate();
  dateMin = CDate::minDate();
  maxValue = -std::numeric_limits<double>::max();
  minValue = std::numeric_limits<double>::max();

  for (size_t i = 0; i < this->nstations(); i++) {
    CDate tempDateMin, tempDateMax;
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

void Hmdf::setVector(bool vector) {
  this->m_isVector = vector;
  for (auto &s : this->m_station) {
    s.setVector(vector);
  }
}
