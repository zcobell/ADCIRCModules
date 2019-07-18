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
#ifndef ADCMOD_READOUTPUTFILE_H
#define ADCMOD_READOUTPUTFILE_H

#include <fstream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "adcircmodules_global.h"
#include "node.h"
#include "outputfile.h"
#include "outputrecord.h"

namespace Adcirc {

namespace Output {

class ReadOutputFile : public OutputFile {
 public:
  ReadOutputFile(const std::string &filename);

  ~ReadOutputFile();

  void open();

  void close();

  void read(size_t snap = Adcirc::Output::nextOutputSnap());

  Adcirc::Output::OutputRecord *data(size_t snap);
  Adcirc::Output::OutputRecord *data(size_t snap, bool &ok);

  Adcirc::Output::OutputRecord *dataAt(size_t position);
  Adcirc::Output::OutputRecord *dataAt(size_t position, bool &ok);

  void clear();
  void clearAt(size_t position);

 private:
  // variables
  std::fstream m_fid;
  std::vector<std::unique_ptr<Adcirc::Output::OutputRecord>> m_records;
  std::unordered_map<size_t, Adcirc::Output::OutputRecord *> m_recordMap;

  // netcdf specific variables
  int m_ncid;
  int m_dimid_time, m_dimid_node;
  int m_varid_time;
  std::vector<double> m_time;
  std::vector<int> m_varid_data;

  // functions
  Adcirc::Output::OutputFormat getFiletype();
  void findNetcdfVarId();
  void rebuildMap();

  void openAscii();
  void openNetcdf();

  void closeAscii();
  void closeNetcdf();

  void readAsciiHeader();
  void readNetcdfHeader();

  void readAsciiRecord(std::unique_ptr<OutputRecord> &record);
  void readNetcdfRecord(size_t snap, std::unique_ptr<OutputRecord> &record);
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_READOUTPUTFILE_H
