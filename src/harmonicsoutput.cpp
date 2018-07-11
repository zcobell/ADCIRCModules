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
#include "harmonicsoutput.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include "boost/algorithm/string.hpp"
#include "error.h"
#include "filetypes.h"
#include "io.h"
#include "netcdf.h"
#include "outputfile.h"
#include "stringconversion.h"

using namespace std;
using namespace Adcirc::Output;

HarmonicsOutput::HarmonicsOutput(string filename, bool velocity)
    : m_filename(filename) {
  this->m_numNodes = 0;
  this->m_numConstituents = 0;
  this->m_isVelocity = velocity;
  this->m_filetype = Adcirc::Output::Unknown;
}

string HarmonicsOutput::filename() const { return this->m_filename; }

void HarmonicsOutput::setFilename(const string& filename) {
  this->m_filename = filename;
}

size_t HarmonicsOutput::index(string name) {
  boost::to_upper(name);
  auto i = this->m_index.find(name);
  if (i == this->m_index.end()) {
    Adcirc::Error::throwError("Harmonic consituent not found in data");
    return std::numeric_limits<size_t>::max();
  } else {
    return i->second;
  }
}

string HarmonicsOutput::name(size_t index) {
  assert(index < this->m_consituentNames.size());
  if (index < this->m_consituentNames.size()) {
    return this->m_consituentNames[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return string();
  }
}

HarmonicsRecord* HarmonicsOutput::amplitude(string name) {
  return this->amplitude(this->index(std::move(name)));
}

HarmonicsRecord* HarmonicsOutput::amplitude(size_t index) {
  assert(index < this->m_amplitude.size());
  if (index < this->m_amplitude.size()) {
    return &this->m_amplitude[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutput::phase(string name) {
  return this->phase(this->index(std::move(name)));
}

HarmonicsRecord* HarmonicsOutput::phase(size_t index) {
  assert(index < this->m_phase.size());
  if (index < this->m_phase.size()) {
    return &this->m_phase[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutput::u_magnitude(string name) {
  return this->u_magnitude(this->index(std::move(name)));
}
HarmonicsRecord* HarmonicsOutput::u_magnitude(size_t index) {
  assert(index < this->m_umagnitude.size());
  if (index < this->m_umagnitude.size()) {
    return &this->m_umagnitude[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutput::u_phase(string name) {
  return this->u_phase(this->index(std::move(name)));
}

HarmonicsRecord* HarmonicsOutput::u_phase(size_t index) {
  assert(index < this->m_uphase.size());
  if (index < this->m_uphase.size()) {
    return &this->m_uphase[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutput::v_magnitude(string name) {
  return this->v_magnitude(this->index(std::move(name)));
}

HarmonicsRecord* HarmonicsOutput::v_magnitude(size_t index) {
  assert(index < this->m_vmagnitude.size());
  if (index < this->m_vmagnitude.size()) {
    return &this->m_vmagnitude[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

HarmonicsRecord* HarmonicsOutput::v_phase(string name) {
  return this->v_phase(this->index(std::move(name)));
}

HarmonicsRecord* HarmonicsOutput::v_phase(size_t index) {
  assert(index < this->m_vphase.size());
  if (index < this->m_vphase.size()) {
    return &this->m_vphase[index];
  } else {
    Adcirc::Error::throwError("Index out of bounds");
    return nullptr;
  }
}

size_t HarmonicsOutput::numConstituents() const {
  return this->m_numConstituents;
}

void HarmonicsOutput::setNumConstituents(const size_t& numConstituents) {
  this->m_numConstituents = numConstituents;
  if (this->isVelocity()) {
    this->m_umagnitude.resize(this->numConstituents());
    this->m_vmagnitude.resize(this->numConstituents());
    this->m_uphase.resize(this->numConstituents());
    this->m_vphase.resize(this->numConstituents());
  } else {
    this->m_amplitude.resize(this->numConstituents());
    this->m_phase.resize(this->numConstituents());
  }
  return;
}

size_t HarmonicsOutput::numNodes() const { return this->m_numNodes; }

void HarmonicsOutput::setNumNodes(const size_t& numNodes) {
  this->m_numNodes = numNodes;
}

bool HarmonicsOutput::isVelocity() const { return this->m_isVelocity; }

size_t HarmonicsOutput::nodeIdToArrayIndex(size_t id) {
  auto i = this->m_nodeIndex.find(id);
  if (i == this->m_nodeIndex.end()) {
    Adcirc::Error::throwError("Harmonic consituent not found in data");
    return std::numeric_limits<size_t>::max();
  } else {
    return i->second;
  }
}

int HarmonicsOutput::filetype() const { return this->m_filetype; }

void HarmonicsOutput::read() {
  this->getFiletype();

  if (this->m_filetype == ASCIIHarmonics) {
    this->readAsciiFormat();
  } else if (this->m_filetype == Netcdf4) {
    this->readNetcdfFormat();
  } else {
    Adcirc::Error::throwError("Invalid file type");
  }
  return;
}

void HarmonicsOutput::getFiletype() {
  if (Filetypes::checkFiletypeNetcdfHarmonics(this->filename())) {
    this->m_filetype = Adcirc::Output::Netcdf4;
    return;
  }
  if (Filetypes::checkFiletypeAsciiHarmonics(this->filename())) {
    this->m_filetype = Adcirc::Output::ASCIIHarmonics;
    return;
  }
  this->m_filetype = Adcirc::Output::Unknown;
  return;
}

void HarmonicsOutput::readAsciiFormat() {
  fstream fid;
  fid.open(this->m_filename);
  if (!fid.is_open()) {
    Adcirc::Error::throwError("File is not open");
  }

  string line;
  bool ok;

  getline(fid, line);
  size_t n = StringConversion::stringToSizet(line, ok);
  if (ok) {
    this->setNumConstituents(n);
  } else {
    fid.close();
    Adcirc::Error::throwError("Error reading file data");
  }

  std::vector<double> frequency;
  std::vector<double> nodalFactor;
  std::vector<double> equilibriumArg;
  std::vector<string> names;

  frequency.resize(this->numConstituents());
  nodalFactor.resize(this->numConstituents());
  equilibriumArg.resize(this->numConstituents());
  names.resize(this->numConstituents());

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    std::vector<string> list;
    getline(fid, line);
    IO::splitString(line, list);
    frequency[i] = StringConversion::stringToDouble(list[0], ok);
    nodalFactor[i] = StringConversion::stringToDouble(list[1], ok);
    equilibriumArg[i] = StringConversion::stringToDouble(list[2], ok);
    names[i] = boost::to_upper_copy<std::string>(list[3]);
  }

  getline(fid, line);
  n = StringConversion::stringToSizet(line, ok);
  if (ok) {
    this->setNumNodes(n);
  } else {
    fid.close();
    Adcirc::Error::throwError("Error reading file data");
  }

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    this->m_index[names[i]] = i;
    this->m_reverseIndex[i] = names[i];
    this->m_consituentNames.push_back(names[i]);
    if (this->isVelocity()) {
      this->u_magnitude(i)->setName(names[i]);
      this->u_magnitude(i)->setFrequency(frequency[i]);
      this->u_magnitude(i)->setNodalFactor(nodalFactor[i]);
      this->u_magnitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_magnitude(i)->setName(names[i]);
      this->v_magnitude(i)->setFrequency(frequency[i]);
      this->v_magnitude(i)->setNodalFactor(nodalFactor[i]);
      this->v_magnitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_phase(i)->setName(names[i]);
      this->u_phase(i)->setFrequency(frequency[i]);
      this->u_phase(i)->setNodalFactor(nodalFactor[i]);
      this->u_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_phase(i)->setName(names[i]);
      this->v_phase(i)->setFrequency(frequency[i]);
      this->v_phase(i)->setNodalFactor(nodalFactor[i]);
      this->v_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_magnitude(i)->resize(this->numNodes());
      this->v_magnitude(i)->resize(this->numNodes());
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

  for (size_t i = 0; i < this->numNodes(); ++i) {
    getline(fid, line);

    size_t node;
    int ierr = IO::splitStringBoundary0Format(line, node);
    if (ierr != Adcirc::NoError) {
    }

    this->m_nodeIndex[node] = i;

    for (size_t j = 0; j < this->numConstituents(); ++j) {
      getline(fid, line);
      if (this->isVelocity()) {
        double um, up, vm, vp;
        ierr = IO::splitStringHarmonicsVelocityFormat(line, um, up, vm, vp);
        if (ierr != Adcirc::NoError) {
          Adcirc::Error::throwError("Error reading harmonics velocity string");
        }

        this->u_magnitude(j)->set(i, um);
        this->u_phase(j)->set(i, up);
        this->v_magnitude(j)->set(i, vm);
        this->v_phase(j)->set(i, vp);
      } else {
        double a, p;
        ierr = IO::splitStringHarmonicsElevationFormat(line, a, p);
        if (ierr != Adcirc::NoError) {
          Adcirc::Error::throwError("Error reading harmonics elevation string");
        }

        this->amplitude(j)->set(i, a);
        this->phase(j)->set(i, p);
      }
    }
  }

  fid.close();

  return;
}

void HarmonicsOutput::readNetcdfFormat() {
  int ncid;
  int ierr = nc_open(this->filename().c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) {
    Adcirc::Error::throwError("Could not open netcdf file");
  }

  vector<int> varids;
  this->readNetcdfFormatHeader(ncid, varids);

  if (this->isVelocity()) {
    this->readNetcdfVelocityData(ncid, varids);
  } else {
    this->readNetcdfElevationData(ncid, varids);
  }
  nc_close(ncid);
  return;
}

void HarmonicsOutput::readNetcdfFormatHeader(int ncid,
                                             std::vector<int>& varids) {
  int ierr, dimid_nconst, dimid_constlen, dimid_node;
  ierr = nc_inq_dimid(ncid, "num_const", &dimid_nconst);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not locate constituent dimension");
  }

  ierr = nc_inq_dimid(ncid, "constlen", &dimid_constlen);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not locate constituent length dimension");
  }

  ierr = nc_inq_dimid(ncid, "node", &dimid_node);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not locate node dimension");
  }

  size_t charlen;
  ierr = nc_inq_dimlen(ncid, dimid_nconst, &this->m_numConstituents);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not read constituent dimension");
  }

  ierr = nc_inq_dimlen(ncid, dimid_node, &this->m_numNodes);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not read node dimension");
  }

  ierr = nc_inq_dimlen(ncid, dimid_constlen, &charlen);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not read character dimension");
  }

  int varid_connames;
  ierr = nc_inq_varid(ncid, "const", &varid_connames);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    Adcirc::Error::throwError("Could not find constituent names");
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
    Error::warning(
        "ADCIRC <= 53.04 does not place frequency, "
        "nodal factor, and equilibrium argument in netcdf output files. "
        "Invalid data has been inserted as a placeholder.");
    hasFrequency = false;
  } else {
    hasFrequency = true;
  }

  char* constituents = new char[charlen + 1]();
  std::fill(constituents, constituents + charlen + 1, 0);
  for (size_t i = 0; i < this->numConstituents(); ++i) {
    size_t start[2];
    size_t count[2];
    start[0] = i;
    start[1] = 0;
    count[0] = 1;
    count[1] = charlen;
    ierr = nc_get_vara_text(ncid, varid_connames, start, count, constituents);
    this->m_consituentNames.push_back(constituents);
    this->m_consituentNames.back() =
        StringConversion::sanitizeString(this->m_consituentNames.back());
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      delete[] constituents;
      Adcirc::Error::throwError("Could not read constituent names");
    }
  }

  delete[] constituents;

  double* frequency = new double[this->numConstituents()];
  double* nodeFactor = new double[this->numConstituents()];
  double* equilibriumArg = new double[this->numConstituents()];
  if (hasFrequency) {
    ierr = nc_get_var(ncid, varid_freq, frequency);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      delete[] nodeFactor;
      delete[] equilibriumArg;
      delete[] frequency;
      Adcirc::Error::throwError("Could not read frequency");
    }
    ierr = nc_get_var(ncid, varid_equ, equilibriumArg);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      delete[] nodeFactor;
      delete[] equilibriumArg;
      delete[] frequency;
      Adcirc::Error::throwError("Could not read equilibrium argument");
    }
    ierr = nc_get_var(ncid, varid_freq, nodeFactor);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      delete[] nodeFactor;
      delete[] equilibriumArg;
      delete[] frequency;
      Adcirc::Error::throwError("Could not node factor");
    }
  } else {
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      nodeFactor[i] = std::numeric_limits<double>::min();
      equilibriumArg[i] = std::numeric_limits<double>::min();
      frequency[i] = std::numeric_limits<double>::min();
    }
  }

  //...Check file type
  int v;
  ierr = nc_inq_varid(ncid, "amp", &v);
  if (ierr == NC_NOERR) {
    this->m_isVelocity = false;
  } else {
    this->m_isVelocity = true;
  }

  if (this->isVelocity()) {
    this->m_umagnitude.resize(this->numConstituents());
    this->m_vmagnitude.resize(this->numConstituents());
    this->m_uphase.resize(this->numConstituents());
    this->m_vphase.resize(this->numConstituents());
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      this->m_index[this->m_consituentNames[i]] = i;
      this->m_reverseIndex[i] = this->m_consituentNames[i];
      this->u_magnitude(i)->resize(this->numNodes());
      this->u_magnitude(i)->setName(this->m_consituentNames[i]);
      this->u_magnitude(i)->setFrequency(frequency[i]);
      this->u_magnitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_magnitude(i)->setNodalFactor(nodeFactor[i]);
      this->v_magnitude(i)->resize(this->numNodes());
      this->v_magnitude(i)->setName(this->m_consituentNames[i]);
      this->v_magnitude(i)->setFrequency(frequency[i]);
      this->v_magnitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_magnitude(i)->setNodalFactor(nodeFactor[i]);
      this->u_phase(i)->resize(this->numNodes());
      this->u_phase(i)->setName(this->m_consituentNames[i]);
      this->u_phase(i)->setFrequency(frequency[i]);
      this->u_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->u_phase(i)->setNodalFactor(nodeFactor[i]);
      this->v_phase(i)->resize(this->numNodes());
      this->v_phase(i)->setName(this->m_consituentNames[i]);
      this->v_phase(i)->setFrequency(frequency[i]);
      this->v_phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->v_phase(i)->setNodalFactor(nodeFactor[i]);
    }
  } else {
    this->m_amplitude.resize(this->numConstituents());
    this->m_phase.resize(this->numConstituents());
    for (size_t i = 0; i < this->numConstituents(); ++i) {
      this->m_index[this->m_consituentNames[i]] = i;
      this->m_reverseIndex[i] = this->m_consituentNames[i];
      this->amplitude(i)->resize(this->numNodes());
      this->amplitude(i)->setName(this->m_consituentNames[i]);
      this->amplitude(i)->setFrequency(frequency[i]);
      this->amplitude(i)->setEquilibriumArg(equilibriumArg[i]);
      this->amplitude(i)->setNodalFactor(nodeFactor[i]);
      this->phase(i)->resize(this->numNodes());
      this->phase(i)->setName(this->m_consituentNames[i]);
      this->phase(i)->setFrequency(frequency[i]);
      this->phase(i)->setEquilibriumArg(equilibriumArg[i]);
      this->phase(i)->setNodalFactor(nodeFactor[i]);
    }
  }

  delete[] nodeFactor;
  delete[] equilibriumArg;
  delete[] frequency;

  int vid;
  if (this->isVelocity()) {
    ierr = nc_inq_varid(ncid, "u_amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find u_amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "u_phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find u_phs");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "v_amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find v_amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "v_phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find v_phs");
    }
    varids.push_back(vid);
  } else {
    ierr = nc_inq_varid(ncid, "amp", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find amp");
    }
    varids.push_back(vid);

    ierr = nc_inq_varid(ncid, "phs", &vid);
    if (ierr != NC_NOERR) {
      nc_close(ncid);
      Adcirc::Error::throwError("Could not find phs");
    }
    varids.push_back(vid);
  }

  return;
}

void HarmonicsOutput::readNetcdfElevationData(int ncid,
                                              std::vector<int>& varids) {
  assert(varids.size() == 2);

  double* v = new double[this->numNodes()];
  size_t start[2], count[2];
  for (size_t i = 0; i < this->numConstituents(); ++i) {
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;
    int ierr = nc_get_vara(ncid, varids[0], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic elevation data");
    }
    vector<double> amp(v, v + this->numNodes());
    this->amplitude(i)->set(amp);
    ierr = nc_get_vara(ncid, varids[1], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic elevation data");
    }
    vector<double> phs(v, v + this->numNodes());
    this->phase(i)->set(phs);
  }

  delete[] v;

  return;
}

void HarmonicsOutput::readNetcdfVelocityData(int ncid,
                                             std::vector<int>& varids) {
  assert(varids.size() == 4);

  double* v = new double[this->numNodes()];
  size_t start[2], count[2];

  for (size_t i = 0; i < this->numConstituents(); ++i) {
    start[0] = 0;
    start[1] = i;
    count[0] = this->numNodes();
    count[1] = 1;

    int ierr = nc_get_vara(ncid, varids[0], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic velocity data");
    }
    vector<double> ua(v, v + this->numNodes());
    this->u_magnitude(i)->set(ua);

    ierr = nc_get_vara(ncid, varids[1], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic velocity data");
    }
    vector<double> up(v, v + this->numNodes());
    this->u_phase(i)->set(up);

    ierr = nc_get_vara(ncid, varids[2], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic velocity data");
    }
    vector<double> va(v, v + this->numNodes());
    this->v_magnitude(i)->set(va);

    ierr = nc_get_vara(ncid, varids[3], start, count, v);
    if (ierr != NC_NOERR) {
      delete[] v;
      Adcirc::Error::throwError("Error reading harmonic velocity data");
    }
    vector<double> vp(v, v + this->numNodes());
    this->v_phase(i)->set(vp);
  }

  delete[] v;
  return;
}

void HarmonicsOutput::write(const string& filename) { return; }
