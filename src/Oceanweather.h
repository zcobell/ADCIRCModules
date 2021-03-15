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
#ifndef OCEANWEATHER_H
#define OCEANWEATHER_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "OceanweatherHeader.h"
#include "OceanweatherRecord.h"

namespace Adcirc {

class Oceanweather {
 public:
  Oceanweather();

  int addDomain(std::string pressureFile, std::string windFile);

  size_t nDomains() const;

  size_t currentSnap() const;

  int open();

  int read();

  int close();

  std::string pressureFile(int domain = 0) const;
  std::string windFile(int domain = 0) const;

  Adcirc::OceanweatherRecord record() const;

 private:
  std::vector<std::string> m_pressureFiles;
  std::vector<std::string> m_windFiles;
  size_t m_currentSnap;
  bool m_isOpen;
  std::vector<std::unique_ptr<std::fstream>> m_fid_pressure;
  std::vector<std::unique_ptr<std::fstream>> m_fid_wind;
  std::vector<OceanweatherHeader> m_headers;
  Adcirc::OceanweatherRecord m_record;
};

}  // namespace Adcirc
#endif  // OCEANWEATHER_H
