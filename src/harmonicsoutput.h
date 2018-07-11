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
#ifndef HARMONICSOUTPUT_H
#define HARMONICSOUTPUT_H

#include <string>
#include <unordered_map>
#include <vector>
#include "harmonicsrecord.h"

namespace Adcirc {

namespace Output {

class HarmonicsOutput {
 public:
  HarmonicsOutput(std::string filename = std::string(), bool velocity = false);

  void read();

  void write(const std::string& filename);

  std::string filename() const;
  void setFilename(const std::string& filename);

  size_t index(std::string name);

  std::string name(size_t index);

  //...fort.53 type parameters
  Adcirc::Output::HarmonicsRecord* amplitude(std::string name);
  Adcirc::Output::HarmonicsRecord* amplitude(size_t index);

  Adcirc::Output::HarmonicsRecord* phase(std::string name);
  Adcirc::Output::HarmonicsRecord* phase(size_t index);

  //...fort.54 type parameters
  Adcirc::Output::HarmonicsRecord* u_magnitude(std::string name);
  Adcirc::Output::HarmonicsRecord* u_magnitude(size_t index);

  Adcirc::Output::HarmonicsRecord* u_phase(std::string name);
  Adcirc::Output::HarmonicsRecord* u_phase(size_t index);

  Adcirc::Output::HarmonicsRecord* v_magnitude(std::string name);
  Adcirc::Output::HarmonicsRecord* v_magnitude(size_t index);

  Adcirc::Output::HarmonicsRecord* v_phase(std::string name);
  Adcirc::Output::HarmonicsRecord* v_phase(size_t index);

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
  std::vector<Adcirc::Output::HarmonicsRecord> m_amplitude;
  std::vector<Adcirc::Output::HarmonicsRecord> m_phase;
  std::vector<Adcirc::Output::HarmonicsRecord> m_umagnitude;
  std::vector<Adcirc::Output::HarmonicsRecord> m_vmagnitude;
  std::vector<Adcirc::Output::HarmonicsRecord> m_uphase;
  std::vector<Adcirc::Output::HarmonicsRecord> m_vphase;
  std::unordered_map<size_t, size_t> m_nodeIndex;
  std::unordered_map<std::string, size_t> m_index;
  std::unordered_map<size_t, std::string> m_reverseIndex;

  void getFiletype();
  void readAsciiFormat();
  void readNetcdfFormat();
  void readNetcdfFormatHeader(int ncid, std::vector<int>& varids);
  void readNetcdfElevationData(int ncid, std::vector<int>& varids);
  void readNetcdfVelocityData(int ncid, std::vector<int>& varids);
};
}  // namespace Output
}  // namespace Adcirc

#endif  // HARMONICSOUTPUT_H
