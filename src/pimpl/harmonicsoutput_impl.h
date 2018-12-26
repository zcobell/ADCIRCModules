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
#ifndef HARMONICSOUTPUTIMPL_H
#define HARMONICSOUTPUTIMPL_H

#include <string>
#include <vector>
#include "harmonicsrecord.h"

#ifdef USE_GOOGLE_FLAT_MAP
#include "absl/container/flat_hash_map.h"
#else
#include <unordered_map>
#endif

class HarmonicsOutputImpl {
 public:
  HarmonicsOutputImpl(const std::string& filename = std::string(),
                      bool velocity = false);

  void read();

  void write(const std::string& filename);

  std::string filename() const;
  void setFilename(const std::string& filename);

  size_t index(const std::string& name);

  std::string name(size_t index);

  //...fort.53 type parameters
  Adcirc::Harmonics::HarmonicsRecord* amplitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* amplitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* phase(size_t index);

  //...fort.54 type parameters
  Adcirc::Harmonics::HarmonicsRecord* u_magnitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_magnitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* u_phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_phase(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* v_magnitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* v_magnitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* v_phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* v_phase(size_t index);

  size_t numConstituents() const;
  void setNumConstituents(const size_t& numConstituents);

  size_t numNodes() const;
  void setNumNodes(const size_t& numNodes);

  bool isVelocity() const;

  size_t nodeIdToArrayIndex(size_t id);

  int filetype() const;

 private:
  int m_filetype;
  bool m_isVelocity;
  std::string m_filename;
  std::vector<std::string> m_consituentNames;
  size_t m_numConstituents;
  size_t m_numNodes;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_amplitude;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_phase;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_umagnitude;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_vmagnitude;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_uphase;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_vphase;

#ifdef USE_GOOGLE_FLAT_MAP
  absl::flat_hash_map<size_t, size_t> m_nodeIndex;
  absl::flat_hash_map<std::string, size_t> m_index;
  absl::flat_hash_map<size_t, std::string> m_reverseIndex;
#else
  std::unordered_map<size_t, size_t> m_nodeIndex;
  std::unordered_map<std::string, size_t> m_index;
  std::unordered_map<size_t, std::string> m_reverseIndex;
#endif

  void getFiletype();
  void readAsciiFormat();
  void readNetcdfFormat();
  void readNetcdfFormatHeader(int ncid, std::vector<int>& varids);
  void readNetcdfElevationData(int ncid, std::vector<int>& varids);
  void readNetcdfVelocityData(int ncid, std::vector<int>& varids);
};

#endif  // HARMONICSOUTPUTIMPL_H
