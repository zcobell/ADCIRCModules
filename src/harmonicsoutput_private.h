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
#ifndef ADCMOD_HARMONICSOUTPUTPRIVATE_H
#define ADCMOD_HARMONICSOUTPUTPRIVATE_H

#include <string>
#include <vector>
#include "adcmap.h"
#include "filetypes.h"
#include "harmonicsrecord.h"

namespace Adcirc {
namespace Private {
class HarmonicsOutputPrivate {
 public:
  HarmonicsOutputPrivate(const std::string& filename = std::string());

  void read();

  void write(const std::string& filename,
             const Adcirc::Harmonics::HarmonicsFormat& filetype =
                 Adcirc::Harmonics::HarmonicsUnknown);

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
  Adcirc::Harmonics::HarmonicsRecord* u_amplitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_amplitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* u_phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_phase(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* v_amplitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* v_amplitude(size_t index);

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
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_uamplitude;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_vamplitude;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_uphase;
  std::vector<Adcirc::Harmonics::HarmonicsRecord> m_vphase;

  adcmap<size_t, size_t> m_nodeIndex;
  adcmap<std::string, size_t> m_index;
  adcmap<size_t, std::string> m_reverseIndex;

  void getFiletype();
  Adcirc::Harmonics::HarmonicsFormat getHarmonicsFormatFromExtension(
      const std::string& filename);
  void readAsciiFormat();
  void readNetcdfFormat();
  void readAsciiHeader(std::fstream& fid);

  bool checkFormatAsciiVelocity(std::fstream& fid);
  bool checkFormatNetcdfVelocity(const int& ncid);

  void writeAsciiFormat(const std::string& filename);
  void writeAsciiFormatElevation(std::ofstream& fid);
  void writeAsciiFormatVelocity(std::ofstream& fid);
  void writeAsciiHeader(std::ofstream& fid);

  void writeNetcdfFormat(const std::string& filename);
  void writeNetcdfFormatElevation(const int& ncid);
  void writeNetcdfFormatVelocity(const int& ncid);
  void writeNetcdfHeader(const int& ncid);

  void readNetcdfFormatHeader(int ncid, std::vector<int>& varids);
  void readNetcdfElevationData(int ncid, std::vector<int>& varids);
  void readNetcdfVelocityData(int ncid, std::vector<int>& varids);
};
}  // namespace Private
}  // namespace Adcirc

#endif  // ADCMOD_HARMONICSOUTPUTPRIVATE_H
