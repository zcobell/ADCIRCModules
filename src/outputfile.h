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
#include <memory>
#include <unordered_map>
#include <vector>
#include "node.h"
#include "outputrecord.h"

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

  int read(size_t snap = Adcirc::Output::NextOutputSnap);

  int write(size_t snap = Adcirc::Output::NextOutputSnap);

  int filetype() const;

  std::string getHeader() const;

  std::string filename() const;
  void setFilename(const std::string &filename);

  Adcirc::Output::OutputRecord *data(size_t snap);
  Adcirc::Output::OutputRecord *data(size_t snap, bool &ok);

  Adcirc::Output::OutputRecord *dataAt(size_t position);
  Adcirc::Output::OutputRecord *dataAt(size_t position, bool &ok);

  size_t getNumSnaps() const;
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
  bool m_isVector;
  bool m_isMax;
  bool m_open;
  int m_filetype;
  size_t m_currentSnap;
  size_t m_numSnaps;
  size_t m_numNodes;
  double m_dt;
  double m_defaultValue;
  int m_dit;
  std::fstream m_fid;
  std::string m_filename;
  std::vector<std::unique_ptr<Adcirc::Output::OutputRecord>> m_records;
  std::unordered_map<size_t, Adcirc::Output::OutputRecord *> m_recordMap;
  std::string m_header;

  // netcdf specific variables
  int m_ncid;
  int m_dimid_time, m_dimid_node;
  int m_varid_time;
  std::vector<double> m_time;
  std::vector<int> m_varid_data;

  // functions
  int getFiletype();
  int findNetcdfVarId();
  int rebuildMap();

  static bool checkFiletypeAsciiFull(const std::string &filename);
  static bool checkFiletypeAsciiSparse(const std::string &filename);
  static bool checkFiletypeNetcdf3(std::string filename);
  static bool checkFiletypeNetcdf4(std::string filename);
  static bool checkFiletypeXdmf(const std::string &filename);
  static bool inquireNetcdfFormat(const std::string &filename, int &format);

  int openAscii();
  int openNetcdf();
  int openXdmf();

  int closeAscii();
  int closeNetcdf();
  int closeXdmf();

  int readAsciiHeader();
  int readNetcdfHeader();

  int readAsciiRecord(std::unique_ptr<OutputRecord> &record);
  int readNetcdfRecord(size_t snap, std::unique_ptr<OutputRecord> &record);
};
}  // namespace Output
}  // namespace Adcirc

#endif  // OUTPUTFILE_H
