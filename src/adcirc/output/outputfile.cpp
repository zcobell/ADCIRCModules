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
  this->m_open = false;
  this->m_filetype = Adcirc::Output::Unknown;
}

OutputFile::OutputFile(const char* filename) {
  this->m_filename = std::string(filename);
  this->m_currentSnap = 0;
  this->m_open = false;
  this->m_filetype = Adcirc::Output::Unknown;
}

OutputFile::~OutputFile() {}

string OutputFile::filename() const { return this->m_filename; }

bool OutputFile::isOpen() { return this->m_open; }

bool OutputFile::open() {
  assert(!this->isOpen());

  if (this->isOpen()) return false;

  this->m_filetype = this->getFiletype();
  std::cout << "Filetype: " << this->m_filetype << "\n";

  if (this->filetype() == Adcirc::Output::ASCIIFull ||
      this->filetype() == Adcirc::Output::ASCIISparse)
    this->openAscii();

  if (this->filetype() == Adcirc::Output::Netcdf3 ||
      this->filetype() == Adcirc::Output::Netcdf4)
    this->openNetcdf();

  if (this->filetype() == Adcirc::Output::Xdmf) this->openXdmf();

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

bool OutputFile::openAscii() {
  assert(!this->isOpen());
  if (this->isOpen()) return false;
  this->m_fid.open(this->filename());
  if (this->m_fid.is_open()) {
    this->m_open = true;
    return true;
  } else
    return false;
}

bool OutputFile::openNetcdf() {
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

bool OutputFile::openXdmf() { return false; }

bool OutputFile::closeAscii() {
  assert(this->isOpen());
  if (this->isOpen()) {
    this->m_fid.close();
    this->m_open = false;
    return true;
  }
  return false;
}

bool OutputFile::closeNetcdf() {
  assert(this->isOpen());
  if (this->isOpen()) {
    nc_close(this->m_ncid);
    this->m_open = false;
    return true;
  }
  return false;
}

bool OutputFile::closeXdmf() { return false; }

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

int OutputFile::filetype() const { return this->m_filetype; }

std::string OutputFile::getHeader() const { return this->m_header; }

void OutputFile::setHeader(const std::string& header) {
  this->m_header = header;
}

int OutputFile::getFiletype() {
  if (this->checkFiletypeNetcdf3()) return Adcirc::Output::Netcdf3;
  if (this->checkFiletypeNetcdf4()) return Adcirc::Output::Netcdf4;
  if (this->checkFiletypeXdmf()) return Adcirc::Output::Xdmf;
  if (this->checkFiletypeAsciiFull()) return Adcirc::Output::ASCIIFull;
  if (this->checkFiletypeAsciiSparse()) return Adcirc::Output::ASCIISparse;
  return Adcirc::Output::Unknown;
}

bool OutputFile::checkFiletypeAsciiFull() {
  assert(!this->filename().empty());

  fstream fid(this->filename());

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

bool OutputFile::checkFiletypeAsciiSparse() {
  assert(!this->filename().empty());

  fstream fid(this->filename());

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

bool OutputFile::inquireNetcdfFormat(int& format) {
  int ncid;
  int ierr = nc_open(this->m_filename.c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) return false;
  ierr = nc_inq_format(ncid, &format);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    return false;
  }
  nc_close(ncid);
  return true;
}

bool OutputFile::checkFiletypeNetcdf3() {
  int format;
  this->inquireNetcdfFormat(format);
  if (format == NC_FORMAT_CLASSIC)
    return true;
  return false;
}

bool OutputFile::checkFiletypeNetcdf4() {
  int format;
  this->inquireNetcdfFormat(format);
  if (format == NC_FORMAT_NETCDF4_CLASSIC || format == NC_FORMAT_NETCDF4)
    return true;
  return false;
}

bool OutputFile::checkFiletypeXdmf() { return false; }

bool OutputFile::findNetcdfVarId() {
  assert(this->isOpen());
  assert(this->filetype() == Adcirc::Output::Netcdf3 ||
         this->filetype() == Adcirc::Output::Netcdf4);

  if (!this->isOpen()) return -1;

  if (!(this->filetype() != Adcirc::Output::Netcdf3) ||
      !(this->filetype() == Adcirc::Output::Netcdf4))
    return -1;

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

bool OutputFile::readAsciiHeader() { return false; }
