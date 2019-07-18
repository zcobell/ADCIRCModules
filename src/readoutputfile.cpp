/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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
#include "readoutputfile.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include "fileio.h"
#include "filetypes.h"
#include "logging.h"
#include "netcdf.h"
#include "stringconversion.h"

using namespace Adcirc::Output;

//...netcdf Variable names currently in ADCIRC source code
static const std::vector<std::string> c_netcdfVarNames = {"sigmat",
                                                          "salinity",
                                                          "temperature",
                                                          "u-vel3D",
                                                          "v-vel3D",
                                                          "w-vel3D",
                                                          "q20",
                                                          "l",
                                                          "ev",
                                                          "qsurfkp1",
                                                          "zeta",
                                                          "zeta_max",
                                                          "u-vel",
                                                          "v-vel",
                                                          "vel_max",
                                                          "pressure",
                                                          "pressure_min",
                                                          "windx",
                                                          "windy",
                                                          "wind_max",
                                                          "radstress_x",
                                                          "radstress_y",
                                                          "radstress_max",
                                                          "swan_HS",
                                                          "swan_HS_max",
                                                          "swan_DIR",
                                                          "swan_DIR_max",
                                                          "swan_TM01",
                                                          "swan_TM01_max",
                                                          "swan_TPS",
                                                          "swan_TPS_max",
                                                          "swan_windx",
                                                          "swan_windy",
                                                          "swan_wind_max",
                                                          "swan_TM02",
                                                          "swan_TM02_max",
                                                          "swan_TMM10",
                                                          "swan_TMM10_max"};

ReadOutputFile::ReadOutputFile(const std::string& filename)
    : OutputFile(filename) {}

ReadOutputFile::~ReadOutputFile() { this->clear(); }

void ReadOutputFile::clear() {
  for (auto& m_record : this->m_records) {
    m_record.reset(nullptr);
  }
  this->m_records.clear();
  this->m_recordMap.clear();
}

void ReadOutputFile::clearAt(size_t position) {
  assert(position < this->m_records.size());
  if (position < this->m_records.size()) {
    this->m_records[position].reset(nullptr);
    this->m_records.erase(this->m_records.begin() + position);
    this->rebuildMap();
  } else {
    adcircmodules_throw_exception("OutputFile: Index exceeds dimension");
  }
}

void ReadOutputFile::open() {
  if (this->isOpen()) {
    adcircmodules_throw_exception("OutputFile: File already open");
  }

  if (!this->exists()) {
    adcircmodules_throw_exception("OutputFile: File does not exist");
  }

  this->setFiletype(this->getFiletype());

  if (this->filetype() == Adcirc::Output::OutputAsciiFull ||
      this->filetype() == Adcirc::Output::OutputAsciiSparse) {
    this->openAscii();
    this->readAsciiHeader();
  } else if (this->filetype() == Adcirc::Output::OutputNetcdf3 ||
             this->filetype() == Adcirc::Output::OutputNetcdf4) {
    this->openNetcdf();
    this->readNetcdfHeader();
  } else if (this->filetype() == Adcirc::Output::OutputHdf5) {
    // this->openHdf5();
  } else {
    adcircmodules_throw_exception("OutputFile: No valid file type detected");
  }

  if (this->isOpen()) {
    return;
  } else {
    adcircmodules_throw_exception("OutputFile: Error opening file");
  }
  return;
}

void ReadOutputFile::close() {
  assert(this->isOpen());

  if (!this->isOpen()) {
    adcircmodules_throw_exception("OutputFile: File not open");
  }

  if (this->filetype() == Adcirc::Output::OutputAsciiFull ||
      this->filetype() == Adcirc::Output::OutputAsciiSparse) {
    return this->closeAscii();
  }

  if (this->filetype() == Adcirc::Output::OutputNetcdf3 ||
      this->filetype() == Adcirc::Output::OutputNetcdf4) {
    return this->closeNetcdf();
  }

  if (this->filetype() == Adcirc::Output::OutputHdf5) {
    // return this->closeHdf5();
  }

  return;
}

void ReadOutputFile::read(size_t snap) {
  std::unique_ptr<OutputRecord> record;

  if (this->filetype() == Adcirc::Output::OutputAsciiFull ||
      this->filetype() == Adcirc::Output::OutputAsciiSparse) {
    if (snap != Adcirc::Output::nextOutputSnap()) {
      Logging::warning(
          "ASCII Output must be read record by "
          "record. Specified snap number ignored.");
    }
    if (this->currentSnap() > this->numSnaps()) {
      adcircmodules_throw_exception(
          "OutputFile: Attempt to read past last record in file");
    }
    this->readAsciiRecord(record);
  } else if (this->filetype() == Adcirc::Output::OutputNetcdf3 ||
             this->filetype() == Adcirc::Output::OutputNetcdf4) {
    this->readNetcdfRecord(snap, record);
  } else {
    adcircmodules_throw_exception("OutputFile: Unknown filetype");
  }

  this->m_records.push_back(std::move(record));

  return;
}

void ReadOutputFile::openAscii() {
  if (this->isOpen()) {
    adcircmodules_throw_exception("OutputFile: File already open");
  }
  this->m_fid.open(this->filename());
  if (this->m_fid.is_open()) {
    this->setOpen(true);
    return;
  } else {
    adcircmodules_throw_exception("OutputFile: File could not be opened");
  }
}

void ReadOutputFile::openNetcdf() {
  if (!this->isOpen()) {
    int ierr = nc_open(this->filename().c_str(), NC_NOWRITE, &this->m_ncid);
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("OutputFile: Error opening netcdf file");
    }
    this->setOpen(true);
  } else {
    adcircmodules_throw_exception("OutputFile: Error opening netcdf file");
    return;
  }
  return;
}

void ReadOutputFile::closeAscii() {
  if (this->isOpen()) {
    this->m_fid.close();
    this->setOpen(false);
    return;
  }
  adcircmodules_throw_exception("OutputFile: Error closing ascii file");
  return;
}

void ReadOutputFile::closeNetcdf() {
  if (this->isOpen()) {
    nc_close(this->m_ncid);
    this->setOpen(false);
    return;
  }
  adcircmodules_throw_exception("OutputFile: Error closing netcdf file");
  return;
}

OutputRecord* ReadOutputFile::data(size_t snap) {
  bool ok;
  return this->data(snap, ok);
}

OutputRecord* ReadOutputFile::data(size_t snap, bool& ok) {
  assert(this->m_recordMap.find(snap) != this->m_recordMap.end());

  if (this->m_recordMap.find(snap) == this->m_recordMap.end()) {
    ok = false;
    adcircmodules_throw_exception(
        "OutputFile: Data requested is out of bounds");
    return nullptr;
  } else {
    ok = true;
    return this->m_recordMap[snap];
  }
}

OutputRecord* ReadOutputFile::dataAt(size_t position) {
  bool ok;
  return this->dataAt(position, ok);
}

OutputRecord* ReadOutputFile::dataAt(size_t position, bool& ok) {
  assert(position < this->m_records.size());

  if (position >= this->m_records.size()) {
    ok = false;
    adcircmodules_throw_exception(
        "OutputFile: Data requested is out of bounds");
    return nullptr;
  } else {
    ok = true;
    return this->m_records[position].get();
  }
}

Adcirc::Output::OutputFormat ReadOutputFile::getFiletype() {
  if (Adcirc::Output::checkFiletypeNetcdf3(this->filename())) {
    return Adcirc::Output::OutputNetcdf3;
  }
  if (Adcirc::Output::checkFiletypeNetcdf4(this->filename())) {
    return Adcirc::Output::OutputNetcdf4;
  }
  if (Adcirc::Output::checkFiletypeAsciiFull(this->filename())) {
    return Adcirc::Output::OutputAsciiFull;
  }
  if (Adcirc::Output::checkFiletypeAsciiSparse(this->filename())) {
    return Adcirc::Output::OutputAsciiSparse;
  }
  return Adcirc::Output::OutputUnknown;
}

void ReadOutputFile::findNetcdfVarId() {
  assert(this->isOpen());
  assert(this->getFiletype() == Adcirc::Output::OutputNetcdf3 ||
         this->getFiletype() == Adcirc::Output::OutputNetcdf4);

  if (!this->isOpen()) {
    adcircmodules_throw_exception("OutputFile: Netcdf file not open");
  }

  if (!(this->getFiletype() != Adcirc::Output::OutputNetcdf3) ||
      !(this->getFiletype() == Adcirc::Output::OutputNetcdf4)) {
    adcircmodules_throw_exception("OutputFile: Filetype is not netcdf");
  }

  this->setName(std::string());

  for (const auto& varname : c_netcdfVarNames) {
    int varid;
    int ierr = nc_inq_varid(this->m_ncid, varname.c_str(), &varid);
    if (ierr == NC_NOERR) {
      this->m_varid_data.push_back(varid);

      if (this->m_varid_data.size() == 1) {
        this->setName(varname);
      } else {
        this->setName(this->name() + "," + varname);
      }

      if (varname.substr(varname.size() - 3, varname.size()) == "max" ||
          varname.substr(varname.size() - 3, varname.size()) == "min") {
        this->setIsMax(true);
      }
    }
  }

  if (this->m_varid_data.size() == 0) {
    adcircmodules_throw_exception(
        "OutputFile: No valid netcdf variables found");
  }
  if (this->m_varid_data.size() > 2) {
    adcircmodules_throw_exception(
        "OutputFile: Too many netcdf variables found");
  }

  if (this->m_varid_data.size() == 1) {
    this->setIsVector(false);
  }
  if (this->m_varid_data.size() == 2) {
    this->setIsVector(true);
  }

  size_t attlen_longname1;
  int ierr = nc_inq_attlen(this->m_ncid, this->m_varid_data[0], "long_name",
                           &attlen_longname1);
  char* longname1_char = new char[attlen_longname1 + 1];
  ierr = nc_get_att_text(this->m_ncid, this->m_varid_data[0], "long_name",
                         longname1_char);

  if (this->isVector()) {
    size_t attlen_longname2;
    int ierr = nc_inq_attlen(this->m_ncid, this->m_varid_data[1], "long_name",
                             &attlen_longname2);
    char* longname2_char = new char[attlen_longname1 + 1];
    ierr = nc_get_att_text(this->m_ncid, this->m_varid_data[1], "long_name",
                           longname2_char);
    std::string s1, s2;
    s1 = std::string(longname1_char, attlen_longname1);
    s2 = std::string(longname2_char, attlen_longname2);
    this->setDescription(s1 + "," + s2);
    delete[] longname1_char;
    delete[] longname2_char;
  } else {
    this->setDescription(std::string(longname1_char, attlen_longname1));
    delete[] longname1_char;
  }

  size_t attlen_units;
  ierr = nc_inq_attlen(this->m_ncid, this->m_varid_data[0], "units",
                       &attlen_units);
  char* units_char = new char[attlen_units + 1];
  ierr =
      nc_get_att_text(this->m_ncid, this->m_varid_data[0], "units", units_char);
  this->setUnits(std::string(units_char, attlen_units));
  delete[] units_char;

  return;
}

void ReadOutputFile::readAsciiHeader() {
  assert(!this->filename().empty());

  if (this->filename().empty()) {
    adcircmodules_throw_exception("OutputFile: No filename specified");
  }

  std::string line;
  std::getline(this->m_fid, line);  // header
  this->setHeader(line);

  std::getline(this->m_fid, line);  // file info header

  std::vector<std::string> list;
  FileIO::Generic::splitString(line, list);

  bool ok;
  this->setNumSnaps(StringConversion::stringToInt(list.at(0), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("OutputFile: Error reading ascii header");
  }

  this->setNumNodes(StringConversion::stringToSizet(list.at(1), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("OutputFile: Error reading ascii header");
  }

  this->setDt(StringConversion::stringToDouble(list.at(2), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("OutputFile: Error reading ascii header");
  }

  this->setDiteration(StringConversion::stringToInt(list.at(3), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("OutputFile: Error reading ascii header");
  }

  int numCols = StringConversion::stringToInt(list.at(4), ok);
  if (numCols == 1) {
    this->setIsVector(false);
  } else if (numCols == 2) {
    this->setIsVector(true);
  } else {
    this->m_fid.close();
    adcircmodules_throw_exception(
        "OutputFile: Invalid number of columns in file");
  }

  return;
}

void ReadOutputFile::readNetcdfHeader() {
  assert(this->isOpen());

  int ierr = nc_inq_dimid(this->m_ncid, "time", &this->m_dimid_time);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Time dimension not found");
  }

  ierr = nc_inq_varid(this->m_ncid, "time", &this->m_varid_time);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Time variable not found");
  }

  size_t ns;
  ierr = nc_inq_dimlen(this->m_ncid, this->m_dimid_time, &ns);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Error reading time dimension");
  }
  this->setNumSnaps(ns);

  ierr = nc_inq_dimid(this->m_ncid, "node", &this->m_dimid_node);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Node dimension not found");
  }

  size_t nn;
  ierr = nc_inq_dimlen(this->m_ncid, this->m_dimid_node, &nn);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Error reading node dimension");
  }
  this->setNumNodes(nn);

  double dt;
  ierr = nc_get_att_double(this->m_ncid, NC_GLOBAL, "dt", &dt);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Error reading model dt");
  }

  double* t = new double[this->numSnaps()];

  ierr = nc_get_var_double(this->m_ncid, this->m_varid_time, t);
  if (ierr != NC_NOERR) {
    delete[] t;
    adcircmodules_throw_exception("Error getting time variable");
    return;
  }

  if (this->numSnaps() > 1) {
    this->setDt(t[1] - t[0]);
  } else {
    this->setDt(t[0]);
  }
  this->setDiteration(this->dt() / dt);
  this->m_time = std::vector<double>(t, t + this->numSnaps());
  delete[] t;

  this->findNetcdfVarId();

  int nofill;
  double dv;
  ierr = nc_inq_var_fill(this->m_ncid, this->m_varid_data[0], &nofill, &dv);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("OutputFile: Error reading default value");
  }
  this->setDefaultValue(dv);

  return;
}

void ReadOutputFile::readAsciiRecord(std::unique_ptr<OutputRecord>& record) {
  std::string line;

  record = std::unique_ptr<OutputRecord>(new OutputRecord(
      this->currentSnap(), this->numNodes(), this->isVector()));

  //...Record header
  std::getline(this->m_fid, line);
  std::vector<std::string> list;
  FileIO::Generic::splitString(line, list);
  bool ok;

  double t = StringConversion::stringToDouble(list[0], ok);
  if (ok) {
    record.get()->setTime(t);
  } else {
    record.reset(nullptr);
    adcircmodules_throw_exception("OutputFile: Error reading ascii record");
  }

  int it = StringConversion::stringToInt(list[1], ok);
  if (ok) {
    record.get()->setIteration(it);
  } else {
    record.reset(nullptr);
    adcircmodules_throw_exception("OutputFile: Error reading ascii record");
  }

  size_t numNonDefault = this->numNodes();
  double dflt = Adcirc::Output::defaultOutputValue();

  if (list.size() > 2) {
    numNonDefault = StringConversion::stringToSizet(list[2], ok);
    if (!ok) {
      record.reset(nullptr);
      adcircmodules_throw_exception("OutputFile: Error reading ascii record");
    }

    dflt = StringConversion::stringToDouble(list[3], ok);
    if (!ok) {
      record.reset(nullptr);
      adcircmodules_throw_exception("OutputFile: Error reading ascii record");
    }
  }
  record.get()->setDefaultValue(dflt);
  record.get()->fill(dflt);

  //...Record loop
  for (size_t i = 0; i < numNonDefault; ++i) {
    std::getline(this->m_fid, line);

    if (this->isVector()) {
      size_t id;
      double v1, v2;
      if (FileIO::AdcircIO::splitStringAttribute2Format(line, id, v1, v2)) {
        record.get()->set(id - 1, v1, v2);
      } else {
        record.reset(nullptr);
        adcircmodules_throw_exception("OutputFile: Error reading ascii record");
      }
    } else {
      size_t id;
      double v1;
      if (FileIO::AdcircIO::splitStringAttribute1Format(line, id, v1)) {
        record.get()->set(id - 1, v1);
      } else {
        record.reset(nullptr);
        adcircmodules_throw_exception("OutputFile: Error reading ascii record");
      };
    }
  }

  //...Setup the map for record indicies
  this->m_recordMap[record.get()->record()] = record.get();
  this->setCurrentSnap(this->currentSnap() + 1);

  return;
}

void ReadOutputFile::readNetcdfRecord(size_t snap,
                                      std::unique_ptr<OutputRecord>& record) {
  if (snap == Output::nextOutputSnap()) {
    snap = this->currentSnap();
  }

  assert(snap < this->numSnaps());
  assert(this->isOpen());

  if (snap > this->numSnaps()) {
    adcircmodules_throw_exception(
        "OutputFile: Record requested > number of records in file");
  }
  record = std::unique_ptr<OutputRecord>(
      new OutputRecord(snap, this->numNodes(), this->isVector()));

  record.get()->setTime(this->m_time[snap]);
  record.get()->setIteration(std::floor(this->m_time[snap] / this->dt()));

  //..Read the data record. If it is a max record, there is
  //  no time dimension
  if (this->isMax()) {
    double* u = new double[this->numNodes()];
    int ierr = nc_get_var(this->m_ncid, this->m_varid_data[0], u);

    if (ierr != NC_NOERR) {
      delete[] u;
      adcircmodules_throw_exception("OutputFile: Error reading netcdf record");
      return;
    }
    record.get()->setAll(this->numNodes(), u);
    delete[] u;

  } else {
    size_t start[2], count[2];
    start[0] = snap;
    start[1] = 0;
    count[0] = 1;
    count[1] = this->numNodes();
    double* u = new double[this->numNodes()];

    int ierr =
        nc_get_vara(this->m_ncid, this->m_varid_data[0], start, count, u);
    if (ierr != NC_NOERR) {
      delete[] u;
      adcircmodules_throw_exception("OutputFile: Error reading netcdf record");
      return;
    }

    if (this->isVector()) {
      double* v = new double[this->numNodes()];
      ierr = nc_get_vara(this->m_ncid, this->m_varid_data[1], start, count, v);
      if (ierr != NC_NOERR) {
        delete[] u;
        delete[] v;
        adcircmodules_throw_exception(
            "OutputFile: Error reading netcdf record");
        return;
      }
      record.get()->setAll(this->numNodes(), u, v);
      delete[] v;
    } else {
      record.get()->setAll(this->numNodes(), u);
    }

    delete[] u;
  }

  this->m_recordMap[record.get()->record()] = record.get();
  this->setCurrentSnap(this->currentSnap() + 1);
}

void ReadOutputFile::rebuildMap() {
  this->m_recordMap.clear();
  for (auto& m_record : this->m_records) {
    this->m_recordMap[m_record->record()] = m_record.get();
  }
  return;
}
