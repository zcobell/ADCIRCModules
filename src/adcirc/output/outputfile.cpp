//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "adcirc/output/outputfile.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include "adcirc/io/io.h"
#include "adcirc/io/stringconversion.h"
#include "netcdf.h"

using namespace Adcirc::Output;
using namespace std;

static const std::vector<string> netcdfVarNames = {"sigmat",
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

OutputFile::OutputFile(std::string filename) {
  this->m_filename = filename;
  this->m_currentSnap = 0;
  this->m_numNodes = 0;
  this->m_open = false;
  this->m_filetype = Adcirc::Output::Unknown;
}

OutputFile::OutputFile(const char* filename) {
  this->m_filename = std::string(filename);
  this->m_currentSnap = 0;
  this->m_numNodes = 0;
  this->m_open = false;
  this->m_filetype = Adcirc::Output::Unknown;
}

OutputFile::~OutputFile() { this->clear(); }

void OutputFile::clear() {
  for (size_t i = 0; i < this->m_records.size(); i++) {
    delete this->m_records[i];
  }
  this->m_records.clear();
  this->m_recordMap.clear();
}

string OutputFile::filename() const { return this->m_filename; }

bool OutputFile::isOpen() { return this->m_open; }

bool OutputFile::exists() {
  ifstream f(this->m_filename.c_str());
  return f.good();
}

bool OutputFile::open() {
  assert(!this->isOpen());
  assert(this->exists());

  if (this->isOpen()) return false;

  if (!this->exists()) {
    std::cerr << "[ADCModules ERROR]: Specified file does not exist.\n";
    return false;
  }

  this->m_filetype = this->getFiletype();

  if (this->filetype() == Adcirc::Output::ASCIIFull ||
      this->filetype() == Adcirc::Output::ASCIISparse) {
    this->openAscii();
    this->readAsciiHeader();
  } else if (this->filetype() == Adcirc::Output::Netcdf3 ||
             this->filetype() == Adcirc::Output::Netcdf4) {
    this->openNetcdf();
    this->readNetcdfHeader();
  } else if (this->filetype() == Adcirc::Output::Xdmf) {
    this->openXdmf();
  } else {
    std::cerr << "[ADCModules ERROR]: no valid file type found.";
    return false;
  }

  return this->isOpen();
}

bool OutputFile::close() {
  assert(this->isOpen());
  if (!this->isOpen()) return false;

  if (this->filetype() == Adcirc::Output::ASCIIFull ||
      this->filetype() == Adcirc::Output::ASCIISparse)
    this->closeAscii();

  if (this->filetype() == Adcirc::Output::Netcdf3 ||
      this->filetype() == Adcirc::Output::Netcdf4)
    this->closeNetcdf();

  if (this->filetype() == Adcirc::Output::Xdmf) this->closeXdmf();

  return true;
}

int OutputFile::read(int snap) {
  OutputRecord* record = nullptr;
  if (this->m_filetype == Adcirc::Output::ASCIIFull ||
      this->m_filetype == Adcirc::Output::ASCIISparse) {
    if (snap != Adcirc::Output::NextOutputSnap) {
      std::cerr << "[ADCModules WARNING]: ASCII Output must be read record by "
                   "record. "
                   "Specified snap number ignored.\n";
    }
    int ierr = this->readAsciiRecord(record);
    if (ierr == 0) {
      this->m_records.push_back(record);
    } else {
      return ierr;
    }
  }

  return 0;
}

int OutputFile::write(int snap) { return 1; }

int OutputFile::openAscii() {
  assert(!this->isOpen());
  if (this->isOpen()) return false;
  this->m_fid.open(this->filename());
  if (this->m_fid.is_open()) {
    this->m_open = true;
    return true;
  } else
    return false;
}

int OutputFile::openNetcdf() {
  assert(!this->isOpen());
  if (!this->isOpen()) {
    int ierr = nc_open(this->m_filename.c_str(), NC_NOWRITE, &this->m_ncid);
    if (ierr != NC_NOERR) return false;
    this->m_open = true;
    return false;
  } else {
    return false;
  }
}

int OutputFile::openXdmf() { return false; }

int OutputFile::closeAscii() {
  assert(this->isOpen());
  if (this->isOpen()) {
    this->m_fid.close();
    this->m_open = false;
    return true;
  }
  return false;
}

int OutputFile::closeNetcdf() {
  assert(this->isOpen());
  if (this->isOpen()) {
    nc_close(this->m_ncid);
    this->m_open = false;
    return true;
  }
  return false;
}

int OutputFile::closeXdmf() { return false; }

OutputRecord* OutputFile::data(int snap) {
  bool ok;
  return this->data(snap, ok);
}

OutputRecord* OutputFile::data(int snap, bool& ok) {
  assert(this->m_recordMap.find(snap) != this->m_recordMap.end());

  if (this->m_recordMap.find(snap) == this->m_recordMap.end()) {
    ok = false;
    return nullptr;
  } else {
    ok = true;
    return this->m_recordMap[snap];
  }
}

int OutputFile::getNumSnaps() const { return this->m_numSnaps; }

void OutputFile::setNumSnaps(int numSnaps) { this->m_numSnaps = numSnaps; }

size_t OutputFile::getNumNodes() const { return this->m_numNodes; }

void OutputFile::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

double OutputFile::getDt() const { return this->m_dt; }

void OutputFile::setDt(double dt) { this->m_dt = dt; }

int OutputFile::getDit() const { return this->m_dit; }

void OutputFile::setDit(int dit) { this->m_dit = dit; }

int OutputFile::filetype() const { return this->m_filetype; }

std::string OutputFile::getHeader() const { return this->m_header; }

void OutputFile::setHeader(const std::string& header) {
  this->m_header = header;
}

int OutputFile::getFiletype() {
  if (OutputFile::checkFiletypeNetcdf3(this->filename()))
    return Adcirc::Output::Netcdf3;
  if (OutputFile::checkFiletypeNetcdf4(this->filename()))
    return Adcirc::Output::Netcdf4;
  if (OutputFile::checkFiletypeXdmf(this->filename()))
    return Adcirc::Output::Xdmf;
  if (OutputFile::checkFiletypeAsciiFull(this->filename()))
    return Adcirc::Output::ASCIIFull;
  if (OutputFile::checkFiletypeAsciiSparse(this->filename()))
    return Adcirc::Output::ASCIISparse;
  return Adcirc::Output::Unknown;
}

bool OutputFile::checkFiletypeAsciiSparse(string filename) {
  assert(!filename.empty());

  fstream fid(filename);

  try {
    string line;
    std::getline(fid, line);  // header
    std::getline(fid, line);  // header
    std::getline(fid, line);  // first record

    vector<string> list;
    int ierr = IO::splitString(line, list);
    if (ierr != FileIO::NoError) {
      fid.close();
      return false;
    }

    if (list.size() == 5) {
      fid.close();
      return true;
    } else {
      fid.close();
      return false;
    }
  } catch (...) {
    if (fid.is_open()) fid.close();
    return false;
  }
}

bool OutputFile::checkFiletypeAsciiFull(string filename) {
  assert(!filename.empty());

  fstream fid(filename);

  try {
    string line;

    getline(fid, line);  // header
    getline(fid, line);  // header
    getline(fid, line);  // first record header

    vector<string> list;
    int ierr = IO::splitString(line, list);
    if (ierr != FileIO::NoError) {
      fid.close();
      return false;
    }

    if (list.size() == 2) {
      fid.close();
      return true;
    } else {
      fid.close();
      return false;
    }
  } catch (...) {
    if (fid.is_open()) fid.close();
    return false;
  }
}

int OutputFile::inquireNetcdfFormat(string filename, int& format) {
  int ncid;
  int ierr = nc_open(filename.c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) return false;
  ierr = nc_inq_format(ncid, &format);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    return false;
  }
  nc_close(ncid);
  return true;
}

bool OutputFile::checkFiletypeNetcdf3(string filename) {
  int format;
  OutputFile::inquireNetcdfFormat(filename, format);
  if (format == NC_FORMAT_CLASSIC) return true;
  return false;
}

bool OutputFile::checkFiletypeNetcdf4(string filename) {
  int format;
  OutputFile::inquireNetcdfFormat(filename, format);
  if (format == NC_FORMAT_NETCDF4_CLASSIC || format == NC_FORMAT_NETCDF4)
    return true;
  return false;
}

bool OutputFile::checkFiletypeXdmf(string filename) { return false; }

int OutputFile::findNetcdfVarId() {
  assert(this->isOpen());
  assert(this->filetype() == Adcirc::Output::Netcdf3 ||
         this->filetype() == Adcirc::Output::Netcdf4);

  if (!this->isOpen()) return false;

  if (!(this->filetype() != Adcirc::Output::Netcdf3) ||
      !(this->filetype() == Adcirc::Output::Netcdf4))
    return false;

  int varid;

  for (size_t i = 0; i < netcdfVarNames.size(); i++) {
    string varname = netcdfVarNames.at(i);
    int ierr = nc_inq_varid(this->m_ncid, varname.c_str(), &varid);
    if (ierr != NC_NOERR) {
      this->m_varid_data.push_back(varid);
    }
  }

  if (this->m_varid_data.size() == 0) return false;
  if (this->m_varid_data.size() > 2) return false;

  if (this->m_varid_data.size() == 1) this->m_isVector = false;
  if (this->m_varid_data.size() == 2) this->m_isVector = true;

  return true;
}

int OutputFile::readAsciiHeader() {
  assert(!this->m_filename.empty());

  if (this->m_filename.empty()) return false;

  this->m_fid = fstream(this->m_filename);

  try {
    string line;
    std::getline(this->m_fid, line);  // header
    this->setHeader(line);

    std::getline(this->m_fid, line);  // file info header

    vector<string> list;
    int ierr = IO::splitString(line, list);
    if (ierr != FileIO::NoError) {
      this->m_fid.close();
      return false;
    }

    bool ok;
    this->setNumSnaps(StringConversion::stringToInt(list.at(0), ok));
    if (!ok) {
      this->m_fid.close();
      return false;
    }

    this->setNumNodes(StringConversion::stringToSizet(list.at(1), ok));
    if (!ok) {
      this->m_fid.close();
      return false;
    }

    this->setDt(StringConversion::stringToDouble(list.at(2), ok));
    if (!ok) {
      this->m_fid.close();
      return false;
    }

    this->setDit(StringConversion::stringToInt(list.at(3), ok));
    if (!ok) {
      this->m_fid.close();
      return false;
    }

    int numCols = StringConversion::stringToInt(list.at(4), ok);
    if (numCols == 1)
      this->m_isVector = false;
    else if (numCols == 2)
      this->m_isVector = true;
    else {
      this->m_fid.close();
      return false;
    }

  } catch (...) {
    if (this->m_fid.is_open()) this->m_fid.close();
    return false;
  }

  return true;
}

int OutputFile::readNetcdfHeader() { return 0; }

int OutputFile::readAsciiRecord(OutputRecord* record) {
  string line;

  record = new OutputRecord(this->m_numNodes, this->m_isVector);

  //...Record header
  getline(this->m_fid, line);
  std::vector<string> list;
  IO::splitString(line, list);
  bool ok;

  double t = StringConversion::stringToDouble(list[0], ok);
  if (ok)
    record->setTime(t);
  else {
    delete record;
    return 1;
  }

  int it = StringConversion::stringToInt(list[1], ok);
  if (ok)
    record->setIteration(it);
  else {
    delete record;
    return 1;
  }

  size_t numNonDefault = this->m_numNodes;
  double dflt = Adcirc::Output::DefaultOutputValue;

  if (list.size() > 2) {
    numNonDefault = StringConversion::stringToSizet(list[2], ok);
    if (!ok) {
      delete record;
      return 1;
    }

    dflt = StringConversion::stringToDouble(list[3], ok);
    if (!ok) {
      delete record;
      return 1;
    }
  }
  record->setDefaultValue(dflt);
  record->fill(dflt);

  //...Record loop
  for (size_t i = 0; i < numNonDefault; i++) {
    getline(this->m_fid, line);

    if (this->m_isVector) {
      size_t id;
      double v1, v2;
      int ierr = IO::splitStringAttribute2Format(line, id, v1, v2);
      if (ierr == 0)
        record->set(id - 1, v1, v2);
      else {
        delete record;
        return 1;
      }
    } else {
      size_t id;
      double v1;
      int ierr = IO::splitStringAttribute1Format(line, id, v1);
      if (ierr == 0) {
        record->set(id - 1, v1);
      } else {
        delete record;
        return 1;
      };
    }
  }

  //...Setup the map for record indicies
  this->m_recordMap[this->m_currentSnap] = record;
  this->m_currentSnap++;

  return 0;
}
