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

using namespace std;

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

  static int readFileData(string filename, vector<string> &data);

  static int splitString(string &data, vector<string> &result);

  static int splitStringNodeFormat(string &data, int &id, double &x, double &y,
                                   double &z);

  static int splitStringElemFormat(string &data, int &id, int &n1, int &n2,
                                   int &n3);

  static int splitStringBoundary0Format(string &data, int &node1);

  static int splitStringBoundary23Format(string &data, int &node1,
                                         double &crest, double &supercritical);

  static int splitStringBoundary24Format(string &data, int &node1, int &node2,
                                         double &crest, double &subcritical,
                                         double &supercritical);

  static int splitStringBoundary25Format(string &data, int &node1, int &node2,
                                         double &crest, double &subcritical,
                                         double &supercritical,
                                         double &pipeheight, double &pipecoef,
                                         double &pipediam);

  static int splitStringAttribute1Format(string &data, int &node,
                                         double &value);

  static int splitStringAttribute12Format(string &data, int &node,
                                          vector<double> &values);
};

#endif // IO_H
