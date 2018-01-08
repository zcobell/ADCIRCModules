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
};

class IO {
public:
  explicit IO();

  static int readFileData(std::string filename, std::vector<std::string> &data);

  static int splitString(std::string &data, std::vector<std::string> &result);

  static int splitStringNodeFormat(std::string &data, int &id, double &x,
                                   double &y, double &z);

  static int splitStringElemFormat(std::string &data, int &id, int &n1, int &n2,
                                   int &n3);

  static int splitStringBoundary0Format(std::string &data, int &node1);

  static int splitStringBoundary23Format(std::string &data, int &node1,
                                         double &crest, double &supercritical);

  static int splitStringBoundary24Format(std::string &data, int &node1,
                                         int &node2, double &crest,
                                         double &subcritical,
                                         double &supercritical);

  static int splitStringBoundary25Format(std::string &data, int &node1,
                                         int &node2, double &crest,
                                         double &subcritical,
                                         double &supercritical,
                                         double &pipeheight, double &pipecoef,
                                         double &pipediam);

  static int splitStringNodalAttribute1Format(std::string &data, int &node,
                                              double &value);

  static int splitStringNodalAttribute12Format(std::string &data, int &node,
                                               std::vector<double> &values);
};

#endif // IO_H
