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
/**
 * \class QProj4
 *
 * \brief Class that serves as an interface to the standard C PROJ4 library
 *
 * This function serves as the C++ interface to the C library PROJ4 (which is
 * compiled into this code)
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef QPROJ4_H
#define QPROJ4_H

#include <string>
#include <vector>
#include "point.h"
#include <unordered_map>
#include <string>

using namespace std;

class QProj4 {

public:

  enum _errors {
    NoError,
    NoSuchProjection,
    Proj4InternalError
  };

  explicit QProj4();

  int transform(int inputEPSG, int outputEPSG, Point &input, Point &output,
                bool &isLatLon);

  int transform(int inputEPSG, int outputEPSG, vector<Point> &input,
                vector<Point> &output, bool &isLatLon);

private:
  void _initialize();
  unordered_map<int,string> m_epsgMapping;
};

#endif // QPROJ4_H
