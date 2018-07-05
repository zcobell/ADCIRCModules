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
#ifndef IO_H
#define IO_H

#include <string>
#include <vector>

namespace FileIO {
enum _fileio {
  NoError,
  GenericFileReadError,
  FileReadErrorNode,
  FileReadErrorElement,
  FileReadErrorBoundary
};
}

class IO {
 public:
  explicit IO();

  static int readFileData(std::string filename, std::vector<std::string> &data);

  static int splitString(std::string &data, std::vector<std::string> &result);

  static int splitStringNodeFormat(std::string &data, size_t &id, double &x, double &y,
                                   double &z);

  static int splitStringElemFormat(std::string &data, size_t &id, size_t &n1, size_t &n2,
                                   size_t &n3);

  static int splitStringBoundary0Format(std::string &data, size_t &node1);

  static int splitStringBoundary23Format(std::string &data, size_t &node1,
                                         double &crest, double &supercritical);

  static int splitStringBoundary24Format(std::string &data, size_t &node1, size_t &node2,
                                         double &crest, double &subcritical,
                                         double &supercritical);

  static int splitStringBoundary25Format(std::string &data, size_t &node1, size_t &node2,
                                         double &crest, double &subcritical,
                                         double &supercritical,
                                         double &pipeheight, double &pipecoef,
                                         double &pipediam);

  static int splitStringAttribute1Format(std::string &data, size_t &node,
                                         double &value);

  static int splitStringAttribute2Format(std::string &data, size_t &node,
                                         double &value1, double &value2);

  static int splitStringAttribute12Format(std::string &data, size_t &node,
                                          std::vector<double> &values);

  static int splitStringHarmonicsElevationFormat(std::string &data, double &amplitude, double &phase);

  static int splitStringHarmonicsVelocityFormat(std::string &data, double &u_magnitude, double &u_phase, double &v_magnitude, double &v_phase);
};

#endif  // IO_H
