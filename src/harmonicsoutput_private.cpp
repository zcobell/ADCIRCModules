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
#include "harmonicsoutput_private.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"
#include "default_values.h"
#include "fileio.h"
#include "filetypes.h"
#include "harmonicsoutput.h"
#include "logging.h"
#include "netcdf.h"
#include "stringconversion.h"

using namespace Adcirc::Harmonics;
using namespace Adcirc::Private;

HarmonicsOutput::~HarmonicsOutput() = default;

HarmonicsOutputPrivate::HarmonicsOutputPrivate(const std::string& filename)
    : m_filename(filename),
      m_numNodes(0),
      m_numConstituents(0),
      m_isVelocity(false),
      m_filetype(Adcirc::Harmonics::HarmonicsUnknown) {}

std::string HarmonicsOutputPrivate::filename() const {
  return this->m_filename;
}

void HarmonicsOutputPrivate::setFilename(const std::string& filename) {
  this->m_filename = filename;
}

size_t HarmonicsOutputPrivate::index(const std::string& name) {
  std::string n = boost::to_upper_copy<std::string>(name);
  auto i = this->m_index.find(n);
  if (i == this->m_index.end()) {
    adcircmodules_throw_exception("Harmonic consituent not found in data");
    return adcircmodules_default_value<size_t>();
  } else {
    return i->second;
  }
}

std::string HarmonicsOutputPrivate::name(size_t index) {
  assert(index < this->m_consituentNames.size());
  if (index < this->m_consituentNames.size()) {
    return this->m_consituentNames[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return std::string();
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::amplitude(const std::string& name) {
  return this->amplitude(this->index(name));
}

HarmonicsRecord* HarmonicsOutputPrivate::amplitude(size_t index) {
  assert(index < this->m_amplitude.size());
  if (index < this->m_amplitude.size()) {
    return &this->m_amplitude[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::phase(const std::string& name) {
  return this->phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutputPrivate::phase(size_t index) {
  assert(index < this->m_phase.size());
  if (index < this->m_phase.size()) {
    return &this->m_phase[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::u_amplitude(const std::string& name) {
  return this->u_amplitude(this->index(name));
}
HarmonicsRecord* HarmonicsOutputPrivate::u_amplitude(size_t index) {
  assert(index < this->m_uamplitude.size());
  if (index < this->m_uamplitude.size()) {
    return &this->m_uamplitude[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::u_phase(const std::string& name) {
  return this->u_phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutputPrivate::u_phase(size_t index) {
  assert(index < this->m_uphase.size());
  if (index < this->m_uphase.size()) {
    return &this->m_uphase[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::v_amplitude(const std::string& name) {
  return this->v_amplitude(this->index(name));
}

HarmonicsRecord* HarmonicsOutputPrivate::v_amplitude(size_t index) {
  assert(index < this->m_vamplitude.size());
  if (index < this->m_vamplitude.size()) {
    return &this->m_vamplitude[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutputPrivate::v_phase(const std::string& name) {
  return this->v_phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutputPrivate::v_phase(size_t index) {
  assert(index < this->m_vphase.size());
  if (index < this->m_vphase.size()) {
    return &this->m_vphase[index];
  } else {
    adcircmodules_throw_exception("Index out of bounds");
    return nullptr;
  }
}

size_t HarmonicsOutputPrivate::numConstituents() const {
  return this->m_numConstituents;
}

void HarmonicsOutputPrivate::setNumConstituents(const size_t& numConstituents) {
  this->m_numConstituents = numConstituents;
  if (this->isVelocity()) {
    this->m_uamplitude.resize(this->numConstituents());
    this->m_vamplitude.resize(this->numConstituents());
    this->m_uphase.resize(this->numConstituents());
    this->m_vphase.resize(this->numConstituents());
  } else {
    this->m_amplitude.resize(this->numConstituents());
    this->m_phase.resize(this->numConstituents());
  }
  return;
}

size_t HarmonicsOutputPrivate::numNodes() const { return this->m_numNodes; }

void HarmonicsOutputPrivate::setNumNodes(const size_t& numNodes) {
  this->m_numNodes = numNodes;
}

bool HarmonicsOutputPrivate::isVelocity() const { return this->m_isVelocity; }

size_t HarmonicsOutputPrivate::nodeIdToArrayIndex(size_t id) {
  auto i = this->m_nodeIndex.find(id);
  if (i == this->m_nodeIndex.end()) {
    adcircmodules_throw_exception("Node not found in data");
    return adcircmodules_default_value<size_t>();
  } else {
    return i->second;
  }
}

int HarmonicsOutputPrivate::filetype() const { return this->m_filetype; }

void HarmonicsOutputPrivate::read() {
  this->getFiletype();

  if (this->m_filetype == Adcirc::Harmonics::HarmonicsAscii) {
    this->readAsciiFormat();
  } else if (this->m_filetype == Adcirc::Harmonics::HarmonicsNetcdf) {
    this->readNetcdfFormat();
  } else {
    adcircmodules_throw_exception("Invalid file type");
  }
  return;
}

void HarmonicsOutputPrivate::write(const std::string& filename,
                                   const HarmonicsFormat& filetype) {
  HarmonicsFormat filetype2;
  if (filetype == Adcirc::Harmonics::HarmonicsUnknown) {
    filetype2 = getHarmonicsFormatFromExtension(filename);
  } else {
    filetype2 = filetype;
  }

  if (filetype2 == Adcirc::Harmonics::HarmonicsAscii) {
    this->writeAsciiFormat(filename);
  } else if (filetype2 == Adcirc::Harmonics::HarmonicsNetcdf) {
    this->writeNetcdfFormat(filename);
  } else {
    adcircmodules_throw_exception("Unknown harmonics file format specified.");
  }
  return;
}

Adcirc::Harmonics::HarmonicsFormat
HarmonicsOutputPrivate::getHarmonicsFormatFromExtension(
    const std::string& filename) {
  std::string extension = FileIO::Generic::getFileExtension(filename);
  if (extension == ".nc") {
    return Adcirc::Harmonics::HarmonicsNetcdf;
  } else {
    return Adcirc::Harmonics::HarmonicsAscii;
  }
}

void HarmonicsOutputPrivate::writeAsciiFormat(const std::string& filename) {
  std::ofstream fid;
  fid.open(filename);
  this->writeAsciiHeader(fid);
  if (this->isVelocity()) {
    this->writeAsciiFormatVelocity(fid);
  } else {
    this->writeAsciiFormatElevation(fid);
  }
  return;
}

void HarmonicsOutputPrivate::writeAsciiHeader(std::ofstream& fid) {
  fid << boost::str(boost::format("%11i\n") % this->numConstituents());

  std::vector<Adcirc::Harmonics::HarmonicsRecord>* ptr;
  if (this->isVelocity()) {
    ptr = &this->m_uamplitude;
  } else {
    ptr = &this->m_amplitude;
  }

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    fid << boost::str(boost::format("%16.10e %9.7f %9.7f %s \n") %
                      ptr->at(i).frequency() % ptr->at(i).nodalFactor() %
                      ptr->at(i).equilibriumArg() % ptr->at(i).name());
  }
  fid << boost::str(boost::format("%11i\n") % this->numNodes());
  return;
}

void HarmonicsOutputPrivate::writeAsciiFormatElevation(std::ofstream& fid) {
  for (size_t i = 0; i < this->numNodes(); ++i) {
    fid << boost::str(boost::format("%11i\n") % (i + 1));
    for (size_t j = 0; j < this->numConstituents(); ++j) {
      fid << boost::str(boost ::format("%16.10e %9.4f\n") %
                        this->amplitude(j)->value(i) %
                        this->phase(j)->value(i));
    }
  }
  return;
}

void HarmonicsOutputPrivate::writeAsciiFormatVelocity(std::ofstream& fid) {
  for (size_t i = 0; i < this->numNodes(); ++i) {
    fid << boost::str(boost::format("%11i\n") % (i + 1));
    for (size_t j = 0; j < this->numConstituents(); ++j) {
      fid << boost::str(
          boost ::format("%16.10e %9.4f %16.10e %9.4f\n") %
          this->u_amplitude(j)->value(i) % this->u_phase(j)->value(i) %
          this->v_amplitude(j)->value(i) % this->v_phase(j)->value(i));
    }
  }
  return;
}

void HarmonicsOutputPrivate::writeNetcdfFormat(const std::string& filename) {
  int ncid;
  int ierr = nc_create(filename.c_str(),
                       NC_NETCDF4 | NC_CLASSIC_MODEL | NC_CLOBBER, &ncid);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception(
        "Error creating netcdf harmonics output file.");
  }
  this->writeNetcdfHeader(ncid);
  if (this->isVelocity()) {
    this->writeNetcdfFormatVelocity(ncid);
  } else {
    this->writeNetcdfFormatElevation(ncid);
  }
  nc_close(ncid);
  return;
}

void HarmonicsOutputPrivate::writeNetcdfHeader(const int& ncid) {
  //...Dimensions
  int dimid_time, dimid_nnode, dimid_constlen, dimid_numconst;
  int ierr = nc_def_dim(ncid, "time", 1, &dimid_time);
  nc_def_dim(ncid, "node", this->numNodes(), &dimid_nnode);
  nc_def_dim(ncid, "constlen", 10, &dimid_constlen);
  nc_def_dim(ncid, "num_const", this->numConstituents(), &dimid_numconst);

  //...Variables
  int dims_constnames[2], dims_data[2];
  int varid_freq, varid_eq, varid_na;
  int varid_constnames, varid_amp, varid_pha;
  int varid_ua, varid_up, varid_va, varid_vp;

  dims_constnames[0] = dimid_numconst;
  dims_constnames[1] = dimid_constlen;
  dims_data[0] = dimid_nnode;
  dims_data[1] = dimid_numconst;
  nc_def_var(ncid, "const", NC_CHAR, 2, dims_constnames, &varid_constnames);
  nc_def_var(ncid, "frequency", NC_DOUBLE, 1, &dimid_numconst, &varid_freq);
  nc_def_var(ncid, "equilibrium_argument", NC_DOUBLE, 1, &dimid_numconst,
             &varid_eq);
  nc_def_var(ncid, "nodal_factor", NC_DOUBLE, 1, &dimid_numconst, &varid_na);

  Adcirc::Harmonics::HarmonicsRecord* ptr;

  if (this->isVelocity()) {
    nc_def_var(ncid, "u_amp", NC_DOUBLE, 2, dims_data, &varid_ua);
    nc_def_var(ncid, "u_phs", NC_DOUBLE, 2, dims_data, &varid_up);
    nc_def_var(ncid, "v_amp", NC_DOUBLE, 2, dims_data, &varid_va);
    nc_def_var(ncid, "v_phs", NC_DOUBLE, 2, dims_data, &varid_vp);

    nc_def_var_deflate(ncid, varid_ua, 1, 1, 2);
    nc_def_var_deflate(ncid, varid_up, 1, 1, 2);
    nc_def_var_deflate(ncid, varid_va, 1, 1, 2);
    nc_def_var_deflate(ncid, varid_vp, 1, 1, 2);

  } else {
    nc_def_var(ncid, "amp", NC_DOUBLE, 2, dims_data, &varid_amp);
    nc_def_var(ncid, "phs", NC_DOUBLE, 2, dims_data, &varid_pha);
  }

  ierr = nc_enddef(ncid);

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    std::unique_ptr<char> c(new char[11]);
    double f[1], e[1], n[1];
    size_t start1[1], start2[2];
    size_t count2[2];

    if (this->isVelocity()) {
      ptr = &this->m_uamplitude[i];
    } else {
      ptr = &this->m_amplitude[i];
    }

    start1[0] = i;

    start2[0] = i;
    start2[1] = 0;
    count2[0] = 1;
    count2[1] = 10;

    f[0] = ptr->frequency();
    e[0] = ptr->equilibriumArg();
    n[0] = ptr->nodalFactor();

    memset(c.get(), ' ', 10);
    memcpy(c.get(), ptr->name().c_str(), ptr->name().size());

    nc_put_vara_text(ncid, varid_constnames, start2, count2, c.get());
    nc_put_var1_double(ncid, varid_freq, start1, f);
    nc_put_var1_double(ncid, varid_eq, start1, e);
    nc_put_var1_double(ncid, varid_na, start1, n);
  }

  return;
}

void HarmonicsOutputPrivate::writeNetcdfFormatElevation(const int& ncid) {
  int varid[2];
  nc_inq_varid(ncid, "amp", &varid[0]);
  nc_inq_varid(ncid, "phs", &varid[1]);

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    size_t start[2], count[2];
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;

    std::vector<Adcirc::Harmonics::HarmonicsRecord*> ptr(4);
    ptr[0] = &this->m_amplitude[i];
    ptr[1] = &this->m_phase[i];

    for (size_t j = 0; j < 2; ++j) {
      std::vector<double> vec = ptr[j]->values();
      std::unique_ptr<double> u(new double[this->numNodes()]);
      std::copy(vec.begin(), vec.end(), u.get());
      vec.clear();
      nc_put_vara(ncid, varid[j], start, count, u.get());
    }
  }
}

void HarmonicsOutputPrivate::writeNetcdfFormatVelocity(const int& ncid) {
  int varid[4];
  nc_inq_varid(ncid, "u_amp", &varid[0]);
  nc_inq_varid(ncid, "u_phs", &varid[1]);
  nc_inq_varid(ncid, "v_amp", &varid[2]);
  nc_inq_varid(ncid, "v_phs", &varid[3]);

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    size_t start[2], count[2];
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;

    std::vector<Adcirc::Harmonics::HarmonicsRecord*> ptr(4);
    ptr[0] = &this->m_uamplitude[i];
    ptr[1] = &this->m_uphase[i];
    ptr[2] = &this->m_vamplitude[i];
    ptr[3] = &this->m_vphase[i];

    for (size_t j = 0; j < 4; ++j) {
      std::vector<double> vec = ptr[j]->values();
      std::unique_ptr<double> u(new double[this->numNodes()]);
      std::copy(vec.begin(), vec.end(), u.get());
      vec.clear();
      nc_put_vara(ncid, varid[j], start, count, u.get());
    }
  }
}

void HarmonicsOutputPrivate::getFiletype() {
  if (Adcirc::Harmonics::checkFiletypeNetcdfHarmonics(this->filename())) {
    this->m_filetype = Adcirc::Harmonics::HarmonicsNetcdf;
    return;
  }
  if (Adcirc::Harmonics::checkFiletypeAsciiHarmonics(this->filename())) {
    this->m_filetype = Adcirc::Harmonics::HarmonicsAscii;
    return;
  }
  this->m_filetype = Adcirc::Output::OutputUnknown;
  return;
}

void HarmonicsOutputPrivate::readAsciiHeader(std::fstream& fid) {
  std::string line;
  bool ok;

  std::getline(fid, line);
  size_t n = StringConversion::stringToSizet(line, ok);
  if (!ok) {
    fid.close();
    adcircmodules_throw_exception("Error reading file data");
  }

  std::vector<double> frequency;
  std::vector<double> nodalFactor;
  std::vector<double> equilibriumArg;
  std::vector<std::string> names;

  frequency.resize(n);
  nodalFactor.resize(n);
  equilibriumArg.resize(n);
  names.resize(n);

  for (size_t i = 0; i < n; ++i) {
    std::vector<std::string> list;
    std::getline(fid, line);
    FileIO::Generic::splitString(line, list);
    frequency[i] = StringConversion::stringToDouble(list[0], ok);
    nodalFactor[i] = StringConversion::stringToDouble(list[1], ok);
    equilibriumArg[i] = StringConversion::stringToDouble(list[2], ok);
    names[i] = boost::to_upper_copy<std::string>(list[3]);
  }

  std::getline(fid, line);
  int nn = StringConversion::stringToSizet(line, ok);
  if (ok) {
    this->setNumNodes(nn);
  } else {
    fid.close();
    adcircmodules_throw_exception("Error reading file data");
  }

  this->m_isVelocity = this->checkFormatAsciiVelocity(fid);
  this->setNumConstituents(n);

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    this->m_index[names[i]] = i;
    this->m_reverseIndex[i] = names[i];
    this->m_consituentNames.push_back(names[i]);
    if (this->isVelocity()) {
      this->u_amplitude(i)->setName(names[i]);
      this->u_amplitude(i)->setFrequency(frequency[i]);
      this->u_amplitude(i)->setNodalFactor(nodalFactor[i]);
      this->u_amplitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_amplitude(i)->setName(names[i]);
      this->v_amplitude(i)->setFrequency(frequency[i]);
      this->v_amplitude(i)->setNodalFactor(nodalFactor[i]);
      this->v_amplitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_phase(i)->setName(names[i]);
      this->u_phase(i)->setFrequency(frequency[i]);
      this->u_phase(i)->setNodalFactor(nodalFactor[i]);
      this->u_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_phase(i)->setName(names[i]);
      this->v_phase(i)->setFrequency(frequency[i]);
      this->v_phase(i)->setNodalFactor(nodalFactor[i]);
      this->v_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_amplitude(i)->resize(this->numNodes());
      this->v_amplitude(i)->resize(this->numNodes());
      this->u_phase(i)->resize(this->numNodes());
      this->v_phase(i)->resize(this->numNodes());
    } else {
      this->amplitude(i)->setName(names[i]);
      this->amplitude(i)->setFrequency(frequency[i]);
      this->amplitude(i)->setNodalFactor(nodalFactor[i]);
      this->amplitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->phase(i)->setName(names[i]);
      this->phase(i)->setFrequency(frequency[i]);
      this->phase(i)->setNodalFactor(nodalFactor[i]);
      this->phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->amplitude(i)->resize(this->numNodes());
      this->phase(i)->resize(this->numNodes());
    }
  }

  return;
}

bool HarmonicsOutputPrivate::checkFormatAsciiVelocity(std::fstream& fid) {
  std::string line;
  std::streampos endHeader = fid.tellg();
  std::getline(fid, line);
  std::getline(fid, line);
  fid.seekg(endHeader, std::ios::beg);
  std::vector<std::string> list;
  FileIO::Generic::splitString(line, list);
  if (list.size() == 2) {
    return false;
  } else if (list.size() == 4) {
    return true;
  } else {
    adcircmodules_throw_exception("Unknown harmonics file format.");
  }
  return false;
}

bool HarmonicsOutputPrivate::checkFormatNetcdfVelocity(const int& ncid) {
  int varid_upha;
  int ierr = nc_inq_varid(ncid, "u_phs", &varid_upha);
  if (ierr != NC_NOERR) {
    return false;
  }
  return true;
}

void HarmonicsOutputPrivate::readAsciiFormat() {
  std::fstream fid;
  fid.open(this->m_filename);
  if (!fid.is_open()) {
    adcircmodules_throw_exception("File is not open");
  }

  this->readAsciiHeader(fid);

  for (size_t i = 0; i < this->numNodes(); ++i) {
    std::string line;
    std::getline(fid, line);

    size_t node;
    FileIO::AdcircIO::splitStringBoundary0Format(line, node);

    this->m_nodeIndex[node] = i;

    for (size_t j = 0; j < this->numConstituents(); ++j) {
      std::getline(fid, line);
      if (this->isVelocity()) {
        double um, up, vm, vp;
        if (!FileIO::AdcircIO::splitStringHarmonicsVelocityFormat(line, um, up,
                                                                  vm, vp)) {
          adcircmodules_throw_exception(
              "Error reading harmonics velocity string");
        }

        this->u_amplitude(j)->set(i, um);
        this->u_phase(j)->set(i, up);
        this->v_amplitude(j)->set(i, vm);
        this->v_phase(j)->set(i, vp);
      } else {
        double a, p;
        if (!FileIO::AdcircIO::splitStringHarmonicsElevationFormat(line, a,
                                                                   p)) {
          adcircmodules_throw_exception(
              "Error reading harmonics elevation string");
        }

        this->amplitude(j)->set(i, a);
        this->phase(j)->set(i, p);
      }
    }
  }

  fid.close();

  return;
}

void HarmonicsOutputPrivate::readNetcdfFormat() {
  int ncid;
  int ierr = nc_open(this->filename().c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) {
    adcircmodules_throw_exception("Could not open netcdf file");
  }

  this->m_isVelocity = this->checkFormatNetcdfVelocity(ncid);

  std::vector<int> varids;
  this->readNetcdfFormatHeader(ncid, varids);

  if (this->isVelocity()) {
    this->readNetcdfVelocityData(ncid, varids);
  } else {
    this->readNetcdfElevationData(ncid, varids);
  }
  nc_close(ncid);
  return;
}

void HarmonicsOutputPrivate::readNetcdfFormatHeader(int ncid,
                                                    std::vector<int>& varids) {
  int ierr, dimid_nconst, dimid_constlen, dimid_node;
  ierr = nc_inq_dimid(ncid, "num_const", &dimid_nconst);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not locate constituent dimension");
  }

  ierr = nc_inq_dimid(ncid, "constlen", &dimid_constlen);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception(
        "Could not locate constituent length dimension");
  }

  ierr = nc_inq_dimid(ncid, "node", &dimid_node);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not locate node dimension");
  }

  size_t charlen;
  ierr = nc_inq_dimlen(ncid, dimid_nconst, &this->m_numConstituents);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not read constituent dimension");
  }

  ierr = nc_inq_dimlen(ncid, dimid_node, &this->m_numNodes);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not read node dimension");
  }

  ierr = nc_inq_dimlen(ncid, dimid_constlen, &charlen);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not read character dimension");
  }

  int varid_connames;
  ierr = nc_inq_varid(ncid, "const", &varid_connames);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    adcircmodules_throw_exception("Could not find constituent names");
  }

  //...Note: Frequency, Equilibrium Arg, and Nodal factor aren't in
  //   the netcdf harmonics output files in adcirc <= 53.04
  //   in that case, we just set the values to a ridiculous number
  //   and wish the user luck
  bool hasFrequency;
  int varid_freq, varid_equ, varid_nodefactor;
  nc_inq_varid(ncid, "frequency", &varid_freq);
  nc_inq_varid(ncid, "nodal_factor", &varid_nodefactor);
  ierr = nc_inq_varid(ncid, "equilibrium_argument", &varid_equ);
  if (ierr != NC_NOERR) {
    Adcirc::Logging::warning(
        "ADCIRC <= 53.04 does not place frequency, "
        "nodal factor, and equilibrium argument in netcdf output files. "
        "Invalid data has been inserted as a placeholder.");
    hasFrequency = false;
  } else {
    hasFrequency = true;
  }

  std::unique_ptr<char> constituents(new char[charlen + 1]);
  std::fill(constituents.get(), constituents.get() + charlen + 1, 0);
  for (size_t i = 0; i < this->numConstituents(); ++i) {
    size_t start[2];
    size_t count[2];
    start[0] = i;
    start[1] = 0;
    count[0] = 1;
    count[1] = charlen;
    ierr = nc_get_vara_text(ncid, varid_connames, start, count,
                            constituents.get());
    this->m_consituentNames.push_back(constituents.get());
    this->m_consituentNames.back() =
        StringConversion::sanitizeString(this->m_consituentNames.back());
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not read constituent names");
    }
  }

  std::unique_ptr<double> frequency(new double[this->numConstituents()]);
  std::unique_ptr<double> nodeFactor(new double[this->numConstituents()]);
  std::unique_ptr<double> equilibriumArg(new double[this->numConstituents()]);
  if (hasFrequency) {
    ierr = nc_get_var(ncid, varid_freq, frequency.get());
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not read frequency");
    }
    ierr = nc_get_var(ncid, varid_equ, equilibriumArg.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Could not read equilibrium argument");
    }
    ierr = nc_get_var(ncid, varid_freq, nodeFactor.get());
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not node factor");
    }
  } else {
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      nodeFactor.get()[i] = std::numeric_limits<double>::min();
      equilibriumArg.get()[i] = std::numeric_limits<double>::min();
      frequency.get()[i] = std::numeric_limits<double>::min();
    }
  }

  if (this->isVelocity()) {
    this->m_uamplitude.resize(this->numConstituents());
    this->m_vamplitude.resize(this->numConstituents());
    this->m_uphase.resize(this->numConstituents());
    this->m_vphase.resize(this->numConstituents());
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      this->m_index[this->m_consituentNames[i]] = i;
      this->m_reverseIndex[i] = this->m_consituentNames[i];
      this->u_amplitude(i)->resize(this->numNodes());
      this->u_amplitude(i)->setName(this->m_consituentNames[i]);
      this->u_amplitude(i)->setFrequency(frequency.get()[i]);
      this->u_amplitude(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->u_amplitude(i)->setNodalFactor(nodeFactor.get()[i]);
      this->v_amplitude(i)->resize(this->numNodes());
      this->v_amplitude(i)->setName(this->m_consituentNames[i]);
      this->v_amplitude(i)->setFrequency(frequency.get()[i]);
      this->v_amplitude(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->v_amplitude(i)->setNodalFactor(nodeFactor.get()[i]);
      this->u_phase(i)->resize(this->numNodes());
      this->u_phase(i)->setName(this->m_consituentNames[i]);
      this->u_phase(i)->setFrequency(frequency.get()[i]);
      this->u_phase(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->u_phase(i)->setNodalFactor(nodeFactor.get()[i]);
      this->v_phase(i)->resize(this->numNodes());
      this->v_phase(i)->setName(this->m_consituentNames[i]);
      this->v_phase(i)->setFrequency(frequency.get()[i]);
      this->v_phase(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->v_phase(i)->setNodalFactor(nodeFactor.get()[i]);
    }
  } else {
    this->m_amplitude.resize(this->numConstituents());
    this->m_phase.resize(this->numConstituents());
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      this->m_index[this->m_consituentNames[i]] = i;
      this->m_reverseIndex[i] = this->m_consituentNames[i];
      this->amplitude(i)->resize(this->numNodes());
      this->amplitude(i)->setName(this->m_consituentNames[i]);
      this->amplitude(i)->setFrequency(frequency.get()[i]);
      this->amplitude(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->amplitude(i)->setNodalFactor(nodeFactor.get()[i]);
      this->phase(i)->resize(this->numNodes());
      this->phase(i)->setName(this->m_consituentNames[i]);
      this->phase(i)->setFrequency(frequency.get()[i]);
      this->phase(i)->setEquilibriumArg(equilibriumArg.get()[i]);
      this->phase(i)->setNodalFactor(nodeFactor.get()[i]);
    }
  }

  int vid;
  if (this->isVelocity()) {
    ierr = nc_inq_varid(ncid, "u_amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      std::cout << nc_strerror(ierr) << std::endl;
      std::cout.flush();
      adcircmodules_throw_exception("Could not find u_amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "u_phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not find u_phs");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "v_amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not find v_amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "v_phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not find v_phs");
    }
    varids.push_back(vid);
  } else {
    ierr = nc_inq_varid(ncid, "amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not find amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      adcircmodules_throw_exception("Could not find phs");
    }
    varids.push_back(vid);
  }

  return;
}

void HarmonicsOutputPrivate::readNetcdfElevationData(int ncid,
                                                     std::vector<int>& varids) {
  assert(varids.size() == 2);

  std::unique_ptr<double> v(new double[this->numNodes()]);
  size_t start[2], count[2];
  for (size_t i = 0; i < this->numConstituents(); ++i) {
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;
    int ierr = nc_get_vara(ncid, varids[0], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic elevation data");
    }
    std::vector<double> amp(v.get(), v.get() + this->numNodes());
    this->amplitude(i)->set(amp);
    ierr = nc_get_vara(ncid, varids[1], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic elevation data");
    }
    std::vector<double> phs(v.get(), v.get() + this->numNodes());
    this->phase(i)->set(phs);
  }

  return;
}

void HarmonicsOutputPrivate::readNetcdfVelocityData(int ncid,
                                                    std::vector<int>& varids) {
  assert(varids.size() == 4);

  std::unique_ptr<double> v(new double[this->numNodes()]);
  size_t start[2], count[2];

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;

    int ierr = nc_get_vara(ncid, varids[0], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic velocity data");
    }
    std::vector<double> ua(v.get(), v.get() + this->numNodes());
    this->u_amplitude(i)->set(ua);

    ierr = nc_get_vara(ncid, varids[1], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic velocity data");
    }
    std::vector<double> up(v.get(), v.get() + this->numNodes());
    this->u_phase(i)->set(up);

    ierr = nc_get_vara(ncid, varids[2], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic velocity data");
    }
    std::vector<double> va(v.get(), v.get() + this->numNodes());
    this->v_amplitude(i)->set(va);

    ierr = nc_get_vara(ncid, varids[3], start, count, v.get());
    if (ierr != NC_NOERR) {
      adcircmodules_throw_exception("Error reading harmonic velocity data");
    }
    std::vector<double> vp(v.get(), v.get() + this->numNodes());
    this->v_phase(i)->set(vp);
  }

  return;
}
