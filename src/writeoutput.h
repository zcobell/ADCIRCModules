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
#ifndef ADCMOD_WRITEOUTPUT_H
#define ADCMOD_WRITEOUTPUT_H

#include <fstream>
#include "mesh.h"
#include "outputrecord.h"
#include "readoutput.h"

namespace Adcirc {
namespace Output {

class WriteOutput {
 public:
  WriteOutput(const std::string &filename, ReadOutput *dataContainer,
              Geometry::Mesh *mesh = nullptr);
  ~WriteOutput();
  void open();
  void close();
  void write(const Adcirc::Output::OutputRecord *record,
             const Adcirc::Output::OutputRecord *record2 = nullptr);
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
  void writeRecordHdf5(const Adcirc::Output::OutputRecord *recordElevation,
                       const Adcirc::Output::OutputRecord *recordVelocity);
  void writeAsciiNodeRecord(size_t i, const OutputRecord *record);

  void h5_createDataset(const std::string &name, bool isVector);
  void h5_appendRecord(const std::string &name,
                         const Adcirc::Output::OutputRecord *record,
                         bool isVector);
  void h5_defineStringAttribute(int64_t id, const std::string &name,
                                const std::string &value);
  void h5_defineIntegerAttribute(int64_t id, const std::string &name,
                                 int value);
  void h5_defineFloatAttribute(int64_t id, const std::string &name,
                               float value);
  void h5_defineDoubleAttribute(int64_t id, const std::string &name,
                                double value);
  void h5_createPropertiesGroup(int64_t gid, bool isVector);
  void h5_defineFiletype();

  Adcirc::Output::ReadOutput *m_dataContainer;
  Adcirc::Geometry::Mesh *m_mesh;
  Adcirc::Output::OutputFormat m_format;
  std::ofstream m_fid;
  std::string m_filename;
  int m_ncid;
  size_t m_recordsWritten;
  bool m_isOpen;
  int m_varid_time;
  int64_t m_h5fid;
  std::vector<int> m_varid;
};

}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_WRITEOUTPUT_H
