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
#include <assert.h>
#include <fstream>
#include "adcirc/adcirc_codes.h"
#include "adcirc/io/io.h"
#include "adcirc/io/stringconversion.h"
#include "boost/algorithm/string.hpp"

using namespace std;
using namespace Adcirc::Output;

HarmonicsOutput::HarmonicsOutput(string filename, bool velocity) {
  this->m_numNodes = 0;
  this->m_numConstituents = 0;
  this->m_isVelocity = velocity;
  this->m_filename = filename;
}

string HarmonicsOutput::filename() const { return this->m_filename; }

void HarmonicsOutput::setFilename(const string& filename) {
  this->m_filename = filename;
}

size_t HarmonicsOutput::index(string name) {
  boost::to_upper(name);
  auto i = this->m_index.find(name);
  if (i == this->m_index.end())
    return 999999999;
  else
    return i->second;
}

HarmonicsRecord* HarmonicsOutput::amplitude(string name) {
  return this->amplitude(this->index(name));
}

HarmonicsRecord* HarmonicsOutput::amplitude(size_t index) {
  assert(index < this->m_amplitude.size());
  if (index < this->m_amplitude.size())
    return &this->m_amplitude[index];
  else
    return nullptr;
}

HarmonicsRecord* HarmonicsOutput::phase(string name) {
  return this->phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutput::phase(size_t index) {
  assert(index < this->m_phase.size());
  if (index < this->m_phase.size())
    return &this->m_phase[index];
  else
    return nullptr;
}

HarmonicsRecord* HarmonicsOutput::u_magnitude(string name) {
  return this->u_magnitude(this->index(name));
}
HarmonicsRecord* HarmonicsOutput::u_magnitude(size_t index) {
  assert(index < this->m_umagnitude.size());
  if (index < this->m_umagnitude.size())
    return &this->m_umagnitude[index];
  else
    return nullptr;
}

HarmonicsRecord* HarmonicsOutput::u_phase(string name) {
  return this->u_phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutput::u_phase(size_t index) {
  assert(index < this->m_uphase.size());
  if (index < this->m_uphase.size())
    return &this->m_uphase[index];
  else
    return nullptr;
}

HarmonicsRecord* HarmonicsOutput::v_magnitude(string name) {
  return this->v_magnitude(this->index(name));
}

HarmonicsRecord* HarmonicsOutput::v_magnitude(size_t index) {
  assert(index < this->m_vmagnitude.size());
  if (index < this->m_vmagnitude.size())
    return &this->m_vmagnitude[index];
  else
    return nullptr;
}

HarmonicsRecord* HarmonicsOutput::v_phase(string name) {
  return this->v_phase(this->index(name));
}

HarmonicsRecord* HarmonicsOutput::v_phase(size_t index) {
  assert(index < this->m_vphase.size());
  if (index < this->m_vphase.size())
    return &this->m_vphase[index];
  else
    return nullptr;
}

size_t HarmonicsOutput::numConstituents() const {
  return this->m_numConstituents;
}

void HarmonicsOutput::setNumConstituents(const size_t& numConstituents) {
  this->m_numConstituents = numConstituents;
  if (this->isVector()) {
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

bool HarmonicsOutput::isVector() const { return this->m_isVelocity; }

size_t HarmonicsOutput::nodeIdToArrayIndex(size_t id) {
  auto i = this->m_nodeIndex.find(id);
  if (i == this->m_nodeIndex.end())
    return 999999999;
  else
    return i->second;
}

int HarmonicsOutput::read() {
  fstream fid;
  fid.open(this->m_filename);
  if (!fid.is_open()) return Adcirc::Output::OutputReadError;

  string line;
  bool ok;

  getline(fid, line);
  size_t n = StringConversion::stringToSizet(line, ok);
  if (ok) {
    this->setNumConstituents(n);
  } else {
    fid.close();
    return Adcirc::Output::OutputReadError;
  }

  std::vector<double> frequency;
  std::vector<double> nodalFactor;
  std::vector<double> equilibriumArg;
  std::vector<string> names;

  frequency.resize(this->numConstituents());
  nodalFactor.resize(this->numConstituents());
  equilibriumArg.resize(this->numConstituents());
  names.resize(this->numConstituents());

  for (size_t i = 0; i < this->numConstituents(); i++) {
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
    return Adcirc::Output::OutputReadError;
  }

  for (size_t i = 0; i < this->numConstituents(); i++) {
    this->m_index[names[i]] = i;
    this->m_reverseIndex[i] = names[i];
    if (this->isVector()) {
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

  for (size_t i = 0; i < this->numNodes(); i++) {
    getline(fid, line);

    size_t node;
    int ierr = IO::splitStringBoundary0Format(line, node);
    if (ierr != 0) return ierr;

    this->m_nodeIndex[node] = i;

    for (size_t j = 0; j < this->numConstituents(); j++) {
      getline(fid, line);
      if (this->isVector()) {
        double um, up, vm, vp;
        ierr = IO::splitStringHarmonicsVelocityFormat(line, um, up, vm, vp);
        if (ierr != 0) return ierr;

        this->u_magnitude(j)->set(i, um);
        this->u_phase(j)->set(i, up);
        this->v_magnitude(j)->set(i, vm);
        this->v_phase(j)->set(i, vp);
      } else {
        double a, p;
        ierr = IO::splitStringHarmonicsElevationFormat(line, a, p);
        if (ierr != 0) return ierr;

        this->amplitude(j)->set(i, a);
        this->phase(j)->set(i, p);
      }
    }
  }

  fid.close();

  return Adcirc::NoError;
}

int HarmonicsOutput::write(string filename) { return Adcirc::NoError; }
