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
#ifndef CONSTANTS_H
#define CONSTANTS_H

class Constants {
 public:
  Constants();
  static double equitoralRadius();
  static double polarRadius();
  static double e();
  static double pi();
  static double twoPi();
  static double halfPi();
  static double deg2rad();
  static double rad2deg();
  static double toRadians(double degrees);
  static double toDegrees(double radians);
  static double radiusEarth(double latitude);
  static double radiusEarth();
  static double g();
  static double rhoAir();
  static double distance(double x1, double y1, double x2, double y2, bool geodesic = false);
  static double azimuth(double x1, double y1, double  x2, double y2);
  static double root2();
  static double root3();

private:
  static double geodesic_distance(double x1, double y1, double x2, double y2);
  static double cartesian_distance(double x1, double y1, double x2, double y2);
};

#endif  // CONSTANTS_H
