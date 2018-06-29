#ifndef HARMONICSOUTPUT_H
#define HARMONICSOUTPUT_H

#include <string>
#include <unordered_map>
#include <vector>
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
#include "harmonicsrecord.h"

class HarmonicsOutput {
 public:
  HarmonicsOutput(bool isVector);

  int read();

  int write(std::string filename);

  std::string filename() const;
  void setFilename(const std::string& filename);

  size_t index(std::string name);

  //...fort.53 type parameters
  HarmonicsRecord* amplitude(std::string name);
  HarmonicsRecord* amplitude(size_t index);

  HarmonicsRecord* phase(std::string name);
  HarmonicsRecord* phase(size_t index);

  //...fort.54 type parameters
  HarmonicsRecord* u_magnitude(std::string name);
  HarmonicsRecord* u_magnitude(size_t index);

  HarmonicsRecord* u_phase(std::string name);
  HarmonicsRecord* u_phase(size_t index);

  HarmonicsRecord* v_magnitude(std::string name);
  HarmonicsRecord* v_magnitude(size_t index);

  HarmonicsRecord* v_phase(std::string name);
  HarmonicsRecord* v_phase(size_t index);

  size_t numConstituents() const;
  void setNumConstituents(const size_t& numConstituents);

  size_t numNodes() const;
  void setNumNodes(const size_t& numNodes);

  bool isVector() const;

 private:
  bool m_isVector;
  std::string m_filename;
  size_t m_numConstituents;
  size_t m_numNodes;
  std::vector<HarmonicsRecord> m_amplitude;
  std::vector<HarmonicsRecord> m_phase;
  std::vector<HarmonicsRecord> m_umagnitude;
  std::vector<HarmonicsRecord> m_vmagnitude;
  std::vector<HarmonicsRecord> m_uphase;
  std::vector<HarmonicsRecord> m_vphase;
  std::unordered_map<std::string, size_t> m_index;
  std::unordered_map<size_t, std::string> m_reverseIndex;
};

#endif  // HARMONICSOUTPUT_H
