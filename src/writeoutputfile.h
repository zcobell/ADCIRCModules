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
#ifndef WRITEOUTPUTFILE_H
#define WRITEOUTPUTFILE_H

#include <fstream>
#include "mesh.h"
#include "outputfile.h"
#include "outputrecord.h"
#include "readoutputfile.h"

namespace Adcirc {
namespace Output {

class WriteOutput {
 public:
  WriteOutput(const std::string &filename, ReadOutputFile *dataContainer,
              Geometry::Mesh *mesh = nullptr);
  ~WriteOutput();
  void open();
  void close();
  void write(const Adcirc::Output::OutputRecord *record);
  void writeSparseAscii(bool s);

  std::string filename() const;

  void setFilename(const std::string &filename);

 private:
  void openFileAscii();
  void openFileNetCDF();
  void openFileHdf5();
  int defineNetcdfVariable(int dimid_node, const int *dims, double fill,
                           size_t index);

  void writeRecordAsciiFull(const Adcirc::Output::OutputRecord *record);
  void writeRecordAsciiSparse(const Adcirc::Output::OutputRecord *record);
  void writeRecordNetCDF(const Adcirc::Output::OutputRecord *record);
  void writeAsciiNodeRecord(size_t i, const OutputRecord *record);

  Adcirc::Output::ReadOutputFile *m_dataContainer;
  Adcirc::Geometry::Mesh *m_mesh;
  Adcirc::Output::OutputFormat m_format;
  std::ofstream m_fid;
  std::string m_filename;
  int m_ncid;
  int m_recordsWritten;
  bool m_isOpen;
  std::vector<int> m_varid;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // WRITEOUTPUTFILE_H
