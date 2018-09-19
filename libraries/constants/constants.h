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

#include <cmath>

class Constants {
 public:
  Constants() = default;

  static constexpr double equitoralRadius() { return 6378137.0; }
  static constexpr double polarRadius() { return 6356752.3; }
  static constexpr double g() { return 9.80665; }
  static constexpr double e() { return exp(1.0); }
  static constexpr double pi() { return 4.0 * atan2(1.0, 1.0); }
  static constexpr double deg2rad() { return Constants::pi() / 180.0; }
  static constexpr double rad2deg() { return 180.0 / Constants::pi(); }
  static constexpr double twoPi() { return 2.0 * Constants::pi(); }
  static constexpr double halfPi() { return 0.5 * Constants::halfPi(); }
  static constexpr double rhoAir() { return 1.15; }
  static constexpr double root2() { return sqrt(2.0); }
  static constexpr double root3() { return sqrt(3.0); }

  static double toRadians(double degrees);
  static double toDegrees(double radians);
  static double radiusEarth(double latitude);
  static double radiusEarth() { return 6378206.4; }

  static double distance(double x1, double y1, double x2, double y2,
                         bool geodesic = false);
  // static double azimuth(double x1, double y1, double  x2, double y2);

 private:
  static double geodesic_distance(double x1, double y1, double x2, double y2);
  static double cartesian_distance(double x1, double y1, double x2, double y2);
};

#endif  // CONSTANTS_H
