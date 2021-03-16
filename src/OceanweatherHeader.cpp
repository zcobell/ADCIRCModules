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
#include "OceanweatherHeader.h"

#include <fstream>

using namespace Adcirc;

OceanweatherHeader::OceanweatherHeader() = default;

void OceanweatherHeader::read(std::ifstream *fid) {
  std::string line;
  std::getline(*(fid), line);
  auto sdate1 = std::string(line.begin() + 55, line.begin() + 65);
  auto sdate2 = std::string(line.begin() + 70, line.begin() + 80);

  this->m_startDate =
      CDate(stoi(sdate1.substr(0, 4)), stoi(sdate1.substr(4, 2)),
            stoi(sdate1.substr(6, 2)), stoi(sdate1.substr(8, 2)), 0, 0);
  this->m_endDate =
      CDate(stoi(sdate2.substr(0, 4)), stoi(sdate2.substr(4, 2)),
            stoi(sdate2.substr(6, 2)), stoi(sdate2.substr(8, 2)), 0, 0);
}

CDate OceanweatherHeader::startDate() const { return m_startDate; }

void OceanweatherHeader::setStartDate(const CDate &startDate) {
  m_startDate = startDate;
}

CDate OceanweatherHeader::endDate() const { return m_endDate; }

void OceanweatherHeader::setEndDate(const CDate &endDate) {
  m_endDate = endDate;
}

bool OceanweatherHeader::operator==(const OceanweatherHeader &b) const {
  return m_startDate == b.startDate() && m_endDate == b.endDate();
}

bool OceanweatherHeader::operator!=(const OceanweatherHeader &b) const {
  return !(*(this) == b);
}
