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
 * \class Projection
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
#ifndef PROJECTION_H
#define PROJECTION_H

#include <string>
#include <tuple>
#include <vector>
#include "point.h"
#include <unordered_map>

class Projection {
 public:
  enum _errors { NoError, NoSuchProjection, NoData, Proj4InternalError };

  explicit Projection();

  std::string projVersion();

  int transform(int inputEPSG, int outputEPSG, double x, double y, double &outx,
                double &outy, bool &isLatLon);

  int transform(int inputEPSG, int outputEPSG, Point &input, Point &output,
                bool &isLatLon);

  int transform(int inputEPSG, int outputEPSG, std::vector<Point> &input,
                std::vector<Point> &output, bool &isLatLon);

  std::string description(int epsg);
  std::string projInitializationString(int epsg);

  static int cpp(double lambda0, double phi0, double x, double y, double &outx,
                 double &outy);
  static int cpp(double lambda0, double phi0, Point &input, Point &output);
  static int cpp(double lambda0, double phi0, std::vector<Point> &input,
                 std::vector<Point> &output);

  static int inverseCpp(double lambda0, double phi0, double x, double y,
                        double &outx, double &outy);
  static int inverseCpp(double lambda0, double phi0, Point &input,
                        Point &output);
  static int inverseCpp(double lambda0, double phi0, std::vector<Point> &input,
                        std::vector<Point> &output);

 private:
  void _initialize();

  size_t position(int epsg);

  std::vector<std::string> m_epsgDescriptions;
  std::vector<std::string> m_epsgInit;
  std::unordered_map<int, size_t> m_epsgMapping;

};

#endif  // Projection_H
