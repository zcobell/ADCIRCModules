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
#include "readoutput.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include "adcirc_outputfiles.h"
#include "fileio.h"
#include "filetypes.h"
#include "logging.h"
#include "netcdf.h"
#include "stringconversion.h"

using namespace Adcirc::Output;

const std::vector<OutputMetadata>* ReadOutput::adcircFileMetadata() {
  return &c_outputMetadata;
}

ReadOutput::ReadOutput(const std::string& filename)
    : m_filename(filename),
      m_currentSnap(0),
      m_numNodes(0),
      m_numSnaps(0),
      m_dt(0),
      m_modelDt(0),
      m_dit(0),
      m_open(false),
      m_defaultValue(Adcirc::Output::defaultOutputValue()),
      m_filetype(Adcirc::Output::OutputUnknown),
      m_units("n/a"),
      m_description("n/a"),
      m_name("n/a"),
      m_varid_time(0),
      m_metadata(OutputMetadata()) {}

ReadOutput::~ReadOutput() { this->clear(); }

std::string ReadOutput::filename() const { return this->m_filename; }

void ReadOutput::setFilename(const std::string& filename) {
  if (!this->isOpen()) {
    this->m_filename = filename;
  } else {
    adcircmodules_throw_exception(
        "ReadOutput: Cannot change filename since file currently open");
  }
}

bool ReadOutput::isOpen() { return this->m_open; }

bool ReadOutput::exists() {
  std::ifstream f(this->m_filename.c_str());
  return f.good();
}

size_t ReadOutput::numSnaps() const { return this->m_numSnaps; }

void ReadOutput::setNumSnaps(size_t numSnaps) { this->m_numSnaps = numSnaps; }

size_t ReadOutput::numNodes() const { return this->m_numNodes; }

void ReadOutput::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

double ReadOutput::dt() const { return this->m_dt; }

void ReadOutput::setDt(double dt) { this->m_dt = dt; }

int ReadOutput::dIteration() const { return this->m_dit; }

void ReadOutput::setDiteration(int dit) { this->m_dit = dit; }

OutputFormat ReadOutput::filetype() const { return this->m_filetype; }

std::string ReadOutput::header() const { return this->m_header; }

void ReadOutput::setHeader(const std::string& header) {
  this->m_header = header;
}

void ReadOutput::setFiletype(OutputFormat filetype) {
  this->m_filetype = filetype;
}

size_t ReadOutput::currentSnap() const { return this->m_currentSnap; }

void ReadOutput::setCurrentSnap(const size_t& currentSnap) {
  this->m_currentSnap = currentSnap;
}

void ReadOutput::setOpen(bool open) { this->m_open = open; }

double ReadOutput::modelDt() const { return this->m_modelDt; }

void ReadOutput::setModelDt(double modelDt) { this->m_modelDt = modelDt; }

OutputMetadata* ReadOutput::metadata() { return &this->m_metadata; }

void ReadOutput::setMetadata(const OutputMetadata& metadata) {
  this->m_metadata = metadata;
}

double ReadOutput::defaultValue() const { return this->m_defaultValue; }

void ReadOutput::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}

void ReadOutput::clear() {
  for (auto& m_record : this->m_records) {
    m_record.reset(nullptr);
  }
  this->m_records.clear();
  this->m_recordMap.clear();
}

void ReadOutput::clearAt(size_t position) {
  assert(position < this->m_records.size());
  if (position < this->m_records.size()) {
    this->m_records[position].reset(nullptr);
    this->m_records.erase(this->m_records.begin() + position);
    this->rebuildMap();
  } else {
    adcircmodules_throw_exception("ReadOutput: Index exceeds dimension");
  }
}

void ReadOutput::open() {
  if (this->isOpen()) {
    adcircmodules_throw_exception("ReadOutput: File already open");
  }

  if (!this->exists()) {
    adcircmodules_throw_exception("ReadOutput: File does not exist");
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
    adcircmodules_throw_exception("ReadOutput: No valid file type detected");
  }

  if (this->isOpen()) {
    return;
  } else {
    adcircmodules_throw_exception("ReadOutput: Error opening file");
  }
  return;
}

void ReadOutput::close() {
  assert(this->isOpen());

  if (!this->isOpen()) {
    adcircmodules_throw_exception("ReadOutput: File not open");
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

void ReadOutput::read(size_t snap) {
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
          "ReadOutput: Attempt to read past last record in file");
    }
    this->readAsciiRecord(record);
  } else if (this->filetype() == Adcirc::Output::OutputNetcdf3 ||
             this->filetype() == Adcirc::Output::OutputNetcdf4) {
    this->readNetcdfRecord(snap, record);
  } else {
    adcircmodules_throw_exception("ReadOutput: Unknown filetype");
  }

  this->m_records.push_back(std::move(record));

  return;
}

void ReadOutput::openAscii() {
  if (this->isOpen()) {
    adcircmodules_throw_exception("ReadOutput: File already open");
  }
  this->m_fid.open(this->filename());
  if (this->m_fid.is_open()) {
    this->setOpen(true);
    return;
  } else {
    adcircmodules_throw_exception("ReadOutput: File could not be opened");
  }
}

void ReadOutput::openNetcdf() {
  if (!this->isOpen()) {
    int ierr = nc_open(this->filename().c_str(), NC_NOWRITE, &this->m_ncid);
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("ReadOutput: Error opening netcdf file");
    }
    this->setOpen(true);
  } else {
    adcircmodules_throw_exception("ReadOutput: Error opening netcdf file");
    return;
  }
  return;
}

void ReadOutput::closeAscii() {
  if (this->isOpen()) {
    this->m_fid.close();
    this->setOpen(false);
    return;
  }
  adcircmodules_throw_exception("ReadOutput: Error closing ascii file");
  return;
}

void ReadOutput::closeNetcdf() {
  if (this->isOpen()) {
    nc_close(this->m_ncid);
    this->setOpen(false);
    return;
  }
  adcircmodules_throw_exception("ReadOutput: Error closing netcdf file");
  return;
}

OutputRecord* ReadOutput::data(size_t snap) {
  bool ok;
  return this->data(snap, ok);
}

OutputRecord* ReadOutput::data(size_t snap, bool& ok) {
  assert(this->m_recordMap.find(snap) != this->m_recordMap.end());

  if (this->m_recordMap.find(snap) == this->m_recordMap.end()) {
    ok = false;
    adcircmodules_throw_exception(
        "ReadOutput: Data requested is out of bounds");
    return nullptr;
  } else {
    ok = true;
    return this->m_recordMap[snap];
  }
}

OutputRecord* ReadOutput::dataAt(size_t position) {
  bool ok;
  return this->dataAt(position, ok);
}

OutputRecord* ReadOutput::dataAt(size_t position, bool& ok) {
  assert(position < this->m_records.size());

  if (position >= this->m_records.size()) {
    ok = false;
    adcircmodules_throw_exception(
        "ReadOutput: Data requested is out of bounds");
    return nullptr;
  } else {
    ok = true;
    return this->m_records[position].get();
  }
}

Adcirc::Output::OutputFormat ReadOutput::getFiletype() {
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

int ReadOutput::netcdfVariableSearch(size_t variableIndex,
                                     OutputMetadata& filetypeFound) {
  if (filetypeFound == OutputMetadata()) {
    const std::vector<OutputMetadata>* meta = this->adcircFileMetadata();
    for (size_t i = 0; i < meta->size(); ++i) {
      int varid;
      int ierr = nc_inq_varid(
          this->m_ncid, meta->at(i).variable(variableIndex).c_str(), &varid);
      if (ierr == NC_NOERR) {
        filetypeFound = meta->at(i);
        return varid;
      }
    }
    return -1;
  } else {
    int varid;
    int ierr = nc_inq_varid(
        this->m_ncid, filetypeFound.variable(variableIndex).c_str(), &varid);
    if (ierr == NC_NOERR) {
      return varid;
    } else {
      return -1;
    }
  }
}

void ReadOutput::findNetcdfVarId() {
  assert(this->isOpen());
  assert(this->getFiletype() == Adcirc::Output::OutputNetcdf3 ||
         this->getFiletype() == Adcirc::Output::OutputNetcdf4);

  if (!this->isOpen()) {
    adcircmodules_throw_exception("ReadOutput: Netcdf file not open");
  }

  if (!(this->getFiletype() != Adcirc::Output::OutputNetcdf3) ||
      !(this->getFiletype() == Adcirc::Output::OutputNetcdf4)) {
    adcircmodules_throw_exception("ReadOutput: Filetype is not netcdf");
  }

  OutputMetadata meta;
  int varid1 = this->netcdfVariableSearch(0, meta);
  if (varid1 == -1) {
    adcircmodules_throw_exception(
        "ReadReadOutput: Could not find valid netCDF variable.");
  }
  this->m_varid_data.push_back(varid1);
  this->setMetadata(meta);

  if (this->metadata()->dimension() >= 2) {
    int varid2 = this->netcdfVariableSearch(1, meta);
    if (varid2 == -1) {
      Adcirc::Logging::warning(
          "ReadReadOutput: Expected second value in output file. Continuing "
          "without.");
      this->metadata()->setDimension(1);
    } else {
      this->m_varid_data.push_back(varid2);
    }
  }

  if (this->metadata()->dimension() == 3) {
    int varid3 = this->netcdfVariableSearch(2, meta);
    if (varid3 == -1) {
      Adcirc::Logging::warning(
          "ReadReadOutput: Expected third value in output file. Continuing "
          "without.");
    } else {
      this->m_varid_data.push_back(varid3);
    }
  }

  return;
}

void ReadOutput::readAsciiHeader() {
  assert(!this->filename().empty());

  if (this->filename().empty()) {
    adcircmodules_throw_exception("ReadOutput: No filename specified");
  }

  std::string line;
  std::getline(this->m_fid, line);  // header
  this->setHeader(line);

  std::getline(this->m_fid, line);  // file info header

  std::vector<std::string> list;
  FileIO::Generic::splitString(line, list);

  bool ok;
  this->setNumSnaps(StringConversion::stringToSizet(list.at(0), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("ReadOutput: Error reading ascii header");
  }

  this->setNumNodes(StringConversion::stringToSizet(list.at(1), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("ReadOutput: Error reading ascii header");
  }

  this->setDt(StringConversion::stringToDouble(list.at(2), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("ReadOutput: Error reading ascii header");
  }

  this->setDiteration(StringConversion::stringToInt(list.at(3), ok));
  if (!ok) {
    this->m_fid.close();
    adcircmodules_throw_exception("ReadOutput: Error reading ascii header");
  }

  this->setModelDt(this->dt() / static_cast<double>(this->dIteration()));

  int numCols = StringConversion::stringToInt(list.at(4), ok);
  if (numCols == 1) {
    this->metadata()->setIsVector(false);
    this->metadata()->setDimension(1);
  } else if (numCols == 2) {
    this->metadata()->setIsVector(true);
    this->metadata()->setDimension(2);
  } else {
    this->m_fid.close();
    adcircmodules_throw_exception(
        "ReadOutput: Invalid number of columns in file");
  }

  return;
}

void ReadOutput::readNetcdfHeader() {
  assert(this->isOpen());

  int ierr = nc_inq_dimid(this->m_ncid, "time", &this->m_dimid_time);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Time dimension not found");
  }

  ierr = nc_inq_varid(this->m_ncid, "time", &this->m_varid_time);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Time variable not found");
  }

  size_t ns;
  ierr = nc_inq_dimlen(this->m_ncid, this->m_dimid_time, &ns);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Error reading time dimension");
  }
  this->setNumSnaps(ns);

  ierr = nc_inq_dimid(this->m_ncid, "node", &this->m_dimid_node);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Node dimension not found");
  }

  size_t nn;
  ierr = nc_inq_dimlen(this->m_ncid, this->m_dimid_node, &nn);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Error reading node dimension");
  }
  this->setNumNodes(nn);

  double dt;
  ierr = nc_get_att_double(this->m_ncid, NC_GLOBAL, "dt", &dt);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Error reading model dt");
  }
  this->setModelDt(dt);

  std::unique_ptr<double> t(new double[this->numSnaps()]);

  ierr = nc_get_var_double(this->m_ncid, this->m_varid_time, t.get());
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("Error getting time variable");
    return;
  }

  if (this->numSnaps() > 1) {
    this->setDt(t.get()[1] - t.get()[0]);
  } else {
    this->setDt(t.get()[0]);
  }
  this->setDiteration(this->dt() / dt);
  this->m_time = std::vector<double>(t.get(), t.get() + this->numSnaps());

  this->findNetcdfVarId();

  int nofill;
  double dv;
  ierr = nc_inq_var_fill(this->m_ncid, this->m_varid_data[0], &nofill, &dv);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("ReadOutput: Error reading default value");
  }
  this->setDefaultValue(dv);

  return;
}

void ReadOutput::readAsciiRecord(std::unique_ptr<OutputRecord>& record) {
  std::string line;

  record = std::unique_ptr<OutputRecord>(new OutputRecord(
      this->currentSnap(), this->numNodes(), *(this->metadata())));

  //...Record header
  std::getline(this->m_fid, line);
  std::vector<std::string> list;
  FileIO::Generic::splitString(line, list);
  bool ok;

  double t = StringConversion::stringToDouble(list[0], ok);
  if (ok) {
    record->setTime(t);
  } else {
    record.reset(nullptr);
    adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
  }

  int it = StringConversion::stringToInt(list[1], ok);
  if (ok) {
    record->setIteration(it);
  } else {
    record.reset(nullptr);
    adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
  }

  size_t numNonDefault = this->numNodes();
  double dflt = Adcirc::Output::defaultOutputValue();

  if (list.size() > 2) {
    numNonDefault = StringConversion::stringToSizet(list[2], ok);
    if (!ok) {
      record.reset(nullptr);
      adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
    }

    dflt = StringConversion::stringToDouble(list[3], ok);
    if (!ok) {
      record.reset(nullptr);
      adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
    }
  }
  record->setDefaultValue(dflt);
  record->fill(dflt);

  //...Record loop
  for (size_t i = 0; i < numNonDefault; ++i) {
    std::getline(this->m_fid, line);

    if (this->metadata()->isVector()) {
      size_t id;
      double v1, v2;
      if (FileIO::AdcircIO::splitStringAttribute2Format(line, id, v1, v2)) {
        record->set(id - 1, v1, v2);
      } else {
        record.reset(nullptr);
        adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
      }
    } else {
      size_t id;
      double v1;
      if (FileIO::AdcircIO::splitStringAttribute1Format(line, id, v1)) {
        record->set(id - 1, v1);
      } else {
        record.reset(nullptr);
        adcircmodules_throw_exception("ReadOutput: Error reading ascii record");
      }
    }
  }

  //...Setup the map for record indicies
  this->m_recordMap[record->record()] = record.get();
  this->setCurrentSnap(this->currentSnap() + 1);

  return;
}

void ReadOutput::readNetcdfRecord(size_t snap,
                                  std::unique_ptr<OutputRecord>& record) {
  if (snap == Output::nextOutputSnap()) {
    snap = this->currentSnap();
  }

  assert(snap < this->numSnaps());
  assert(this->isOpen());

  if (snap > this->numSnaps()) {
    adcircmodules_throw_exception(
        "ReadOutput: Record requested > number of records in file");
  }
  record = std::unique_ptr<OutputRecord>(new OutputRecord(
      snap, this->numNodes(), this->metadata()->isVector(),
      this->metadata()->isMax(), this->metadata()->dimension()));

  record->setTime(this->m_time[snap]);
  record->setIteration(std::floor(this->m_time[snap] / this->dt()));

  //..Read the data record. If it is a max record, there is
  //  no time dimension
  if (this->metadata()->isMax()) {
    if (this->metadata()->dimension() == 1) {
      std::unique_ptr<double> u(new double[this->numNodes()]);
      int ierr = nc_get_var(this->m_ncid, this->m_varid_data[0], u.get());

      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
    } else if (this->metadata()->dimension() == 2) {
      std::unique_ptr<double> u(new double[this->numNodes()]);
      std::unique_ptr<double> v(new double[this->numNodes()]);
      int ierr = nc_get_var(this->m_ncid, this->m_varid_data[0], u.get());

      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      ierr = nc_get_var(this->m_ncid, this->m_varid_data[1], v.get());

      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      record->setAll(this->numNodes(), u.get(), v.get());
    }
  } else {
    size_t start[2], count[2];
    start[0] = snap;
    start[1] = 0;
    count[0] = 1;
    count[1] = this->numNodes();

    if (this->metadata()->dimension() == 1) {
      std::unique_ptr<double> u(new double[this->numNodes()]);
      int ierr = nc_get_vara(this->m_ncid, this->m_varid_data[0], start, count,
                             u.get());
      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      record->setAll(this->numNodes(), u.get());
    } else if (this->metadata()->dimension() == 2) {
      std::unique_ptr<double> u(new double[this->numNodes()]);
      std::unique_ptr<double> v(new double[this->numNodes()]);
      int ierr = nc_get_vara(this->m_ncid, this->m_varid_data[0], start, count,
                             u.get());
      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }

      ierr = nc_get_vara(this->m_ncid, this->m_varid_data[1], start, count,
                         v.get());
      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      record->setAll(this->numNodes(), u.get(), v.get());
    } else if (this->metadata()->dimension() == 3) {
      std::unique_ptr<double> u(new double[this->numNodes()]);
      std::unique_ptr<double> v(new double[this->numNodes()]);
      std::unique_ptr<double> w(new double[this->numNodes()]);

      int ierr = nc_get_vara(this->m_ncid, this->m_varid_data[1], start, count,
                             v.get());
      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      ierr = nc_get_vara(this->m_ncid, this->m_varid_data[2], start, count,
                         w.get());
      if (ierr != NC_NOERR) {
        adcircmodules_throw_exception(
            "ReadOutput: Error reading netcdf record");
        return;
      }
      record->setAll(this->numNodes(), u.get(), v.get(), w.get());
    }
  }

  this->m_recordMap[record->record()] = record.get();
  this->setCurrentSnap(this->currentSnap() + 1);
}

void ReadOutput::rebuildMap() {
  this->m_recordMap.clear();
  for (auto& m_record : this->m_records) {
    this->m_recordMap[m_record->record()] = m_record.get();
  }
  return;
}
