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
#include "locations.h"

#include <cassert>
#include <fstream>
#include "adcircmodules.h"
#include "shapefil.h"

Locations::Locations(const std::string &filename, const std::string &fieldname) {
  this->m_filename = filename;
  this->m_fieldname = fieldname;
  this->m_dbfFilename =
      Adcirc::FileIO::Generic::getFileWithoutExtension(this->m_filename) +
      ".dbf";
}

size_t Locations::size() { return this->m_locations.size(); }

Location *Locations::location(size_t index) {
  assert(index < this->m_locations.size());
  return &this->m_locations[index];
}

void Locations::read() {
  std::ifstream f(this->m_filename);
  if (!f.good()) {
    Adcirc::Logging::logError("Location file does not exist.");
  }

  std::string ext = Adcirc::FileIO::Generic::getFileExtension(this->m_filename);
  if (ext == ".shp") {
    this->readShapefile();
  } else if (ext == ".csv") {
    this->readCsv();
  }

  return;
}

void Locations::readShapefile() {
  SHPHandle s = SHPOpen(this->m_filename.c_str(), "rb");
  DBFHandle d = DBFOpen(this->m_dbfFilename.c_str(), "rb");
  int nObject, shapeType;

  SHPGetInfo(s, &nObject, &shapeType, nullptr, nullptr);

  int dbfField = DBFGetFieldIndex(d, this->m_fieldname.c_str());
  if (dbfField == -1) {
    Adcirc::Logging::logError("Field \"" + this->m_fieldname +
                              "\" not found in dbf file.");
    exit(1);
  }

  for (int i = 0; i < nObject; ++i) {
    Location l;
    SHPObject *p = SHPReadObject(s, i);
    int pType = p->nSHPType;
    if (pType == SHPT_POINT || pType == SHPT_POINTZ) {
      l.setX(p->padfX[0]);
      l.setY(p->padfY[0]);
      l.setMeasured(DBFReadDoubleAttribute(d, i, dbfField));
      this->m_locations.push_back(l);
    } else {
      Adcirc::Logging::logError("Invalid shapefile object type");
      SHPDestroyObject(p);
    }
    SHPDestroyObject(p);
  }

  SHPClose(s);
}

void Locations::readCsv() { return; }
