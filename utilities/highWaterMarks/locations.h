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
#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <string>
#include <vector>
#include "location.h"

class Locations {
 public:
  Locations(const std::string &filename, const std::string &fieldname,
            bool unitFlag = false);
  void read();

  Location *location(size_t index);
  size_t size();

 private:
  void readShapefile();
  void readCsv();

  bool m_unitFlag;
  double m_conversion;

  std::string m_filename;
  std::string m_dbfFilename;
  std::string m_fieldname;

  std::vector<Location> m_locations;
};

#endif  // LOCATIONS_H
