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
#ifndef OCEANWEATHERHEADER_H
#define OCEANWEATHERHEADER_H

#include "CDate.h"

namespace Adcirc {

class OceanweatherHeader {
 public:
  OceanweatherHeader();

  void read(std::fstream *fid);

  Adcirc::CDate startDate() const;

  void setStartDate(const Adcirc::CDate &startDate);

  Adcirc::CDate endDate() const;

  void setEndDate(const Adcirc::CDate &endDate);

  bool operator==(const OceanweatherHeader &b) const;

  bool operator!=(const OceanweatherHeader &b) const;

 private:
  Adcirc::CDate m_startDate;
  Adcirc::CDate m_endDate;
};

}  // namespace Adcirc

#endif  // OCEANWEATHERHEADER_H
