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
#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include <fstream>
#include <unordered_map>
#include <vector>
#include "adcirc/geometry/node.h"
#include "adcirc/output/outputrecord.h"

namespace Adcirc {

namespace Output {

enum _fileTypes { ASCIIFull, ASCIISparse, Netcdf3, Netcdf4, Xdmf, Unknown };

class OutputFile {
 public:
  explicit OutputFile(std::string filename);

  ~OutputFile();

  void setHeader(const std::string &header);

  int open();

  int close();

  bool exists();

  bool isOpen();

  int read(int snap = Adcirc::Output::NextOutputSnap);

  int write(int snap = Adcirc::Output::NextOutputSnap);

  int filetype() const;

  std::string getHeader() const;

  std::string filename() const;
  void setFilename(const std::string &filename);

  Adcirc::Output::OutputRecord *data(size_t snap);
  Adcirc::Output::OutputRecord *data(size_t snap, bool &ok);

  Adcirc::Output::OutputRecord *dataAt(size_t position);
  Adcirc::Output::OutputRecord *dataAt(size_t position, bool &ok);

  int getNumSnaps() const;
  void setNumSnaps(int numSnaps);

  size_t getNumNodes() const;
  void setNumNodes(size_t numNodes);

  double getDt() const;
  void setDt(double dt);

  int getDiteration() const;
  void setDiteration(int dit);

  void clear();
  void clearAt(size_t position);

 private:
  // variables
  bool m_open;
  int m_filetype;
  size_t m_currentSnap;
  size_t m_numSnaps;
  size_t m_numNodes;
  double m_dt;
  int m_dit;
  std::fstream m_fid;
  std::string m_filename;
  std::vector<Adcirc::Output::OutputRecord *> m_records;
  std::unordered_map<size_t, Adcirc::Output::OutputRecord *> m_recordMap;
  std::string m_header;

  // netcdf specific variables
  bool m_isVector;
  int m_ncid;
  int m_dimid_time;
  int m_varid_time;
  std::vector<int> m_varid_data;

  // functions
  int getFiletype();
  int findNetcdfVarId();
  int rebuildMap();

  static bool checkFiletypeAsciiFull(std::string filename);
  static bool checkFiletypeAsciiSparse(std::string filename);
  static bool checkFiletypeNetcdf3(std::string filename);
  static bool checkFiletypeNetcdf4(std::string filename);
  static bool checkFiletypeXdmf(std::string filename);
  static int inquireNetcdfFormat(std::string filename, int &format);

  int openAscii();
  int openNetcdf();
  int openXdmf();

  int closeAscii();
  int closeNetcdf();
  int closeXdmf();

  int readAsciiHeader();
  int readNetcdfHeader();

  int readAsciiRecord(Adcirc::Output::OutputRecord *record);
};
}  // namespace Output
}  // namespace Adcirc

#endif  // OUTPUTFILE_H
