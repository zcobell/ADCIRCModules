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
#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include <fstream>
#include <unordered_map>
#include <vector>
#include "adcirc/adcirc_errors.h"
#include "adcirc/geometry/node.h"
#include "adcirc/output/outputrecord.h"

namespace Adcirc {

namespace Output {

class OutputFile {
 public:
  explicit OutputFile(std::string filename);

  explicit OutputFile(const char *filename);

  ~OutputFile();

  void setHeader(const std::string &header);

  bool open();

  bool close();

  bool isOpen();

  bool read(OutputRecord &data, int snap = Adcirc::Output::NextOutputSnap);

  bool write(OutputRecord &data, int snap = Adcirc::Output::NextOutputSnap);

  int filetype() const;

  std::string getHeader() const;

  std::string filename() const;

  OutputRecord *data(int snap, bool &ok);

 private:
  // variables
  bool m_open;
  int m_filetype;
  int m_currentSnap;
  std::fstream m_fid;
  std::string m_filename;
  std::vector<OutputRecord> m_records;
  std::unordered_map<int, OutputRecord *> m_recordMap;
  std::string m_header;

  // netcdf specific variables
  bool m_isVector;
  int m_ncid;
  int m_dimid_time;
  int m_varid_time;
  std::vector<int> m_varid_data;

  // functions
  int getFiletype();
  bool findNetcdfVarId();
  bool checkFiletypeAsciiFull();
  bool checkFiletypeAsciiSparse();
  bool checkFiletypeNetcdf3();
  bool checkFiletypeNetcdf4();
  bool checkFiletypeXdmf();
  bool inquireNetcdfFormat(int &format);

  bool openAscii();
  bool openNetcdf();
  bool openXdmf();

  bool closeAscii();
  bool closeNetcdf();
  bool closeXdmf();

  bool readAsciiHeader();
  bool readNetcdfHeader();
};
}
}

#endif  // OUTPUTFILE_H
