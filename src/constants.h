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
#ifndef ADCMOD_CONSTANTS_H
#define ADCMOD_CONSTANTS_H

#include <utility>

using Point = std::pair<double, double>;

/**
 * @class Constants
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief Set of static functions to use for geometric or Earth constant
 * calculations
 *
 */
class Constants {
 public:
  Constants() = default;

  /**
   * @brief Returns the equitorial radius of earth in meters
   * @return equitorial radius of earth in meters
   */
  static constexpr double equitoralRadius() { return 6378137.0; }

  /**
   * @brief Returns the polar radius of earth in meters
   * @return
   */
  static constexpr double polarRadius() { return 6356752.3; }

  /**
   * @brief Returns the mean radius of the earth in meters
   * @return mean radius of earth in meters
   */
  static constexpr double meanRadiusEarth() { return 6378206.4; }

  /**
   * @brief Returns the constant e
   * @return e
   */
  static constexpr double e() { return 2.71828182845904523536; }

  /**
   * @brief Returns the constant pi
   * @return pi
   */
  static constexpr double pi() { return 3.14159265358979323846; }

  /**
   * @brief Returns two times the constant pi
   * @return 2*pi
   */
  static constexpr double twoPi() { return 2.0 * Constants::pi(); }

  /**
   * @brief Returns 0.5 times the constant pi
   * @return
   */
  static constexpr double halfPi() { return 0.50 * Constants::pi(); }

  /**
   * @brief Returns the factor to convert degrees to radians
   * @return degrees to radians conversion factor (pi / 180)
   */
  static constexpr double deg2rad() { return Constants::pi() / 180.0; }

  /**
   * @brief Returns the factor to convert radians to degrees
   * @return radians to degrees conversion factor (180 / pi)
   */
  static constexpr double rad2deg() { return 180.0 / Constants::pi(); }

  /**
   * @brief Returns the gravitational constant g in m/s2
   * @return g
   */
  static constexpr double g() { return 9.80665; }

  /**
   * @brief Returns the density of air at standard temperature and pressure
   * @return 1.15
   */
  static constexpr double rhoAir() { return 1.15; }

  /**
   * @brief Returns the square root of 2
   * @return sqrt(2)
   */
  static constexpr double root2() { return 1.41421356237309504880; }

  /**
   * @brief Returns the square root of 3
   * @return sqrt(3)
   */
  static constexpr double root3() { return 1.73205080756887719317; }

  static double toRadians(double degrees);
  static double toDegrees(double radians);
  static double radiusEarth(double latitude);

  static double distance(double x1, double y1, double x2, double y2,
                         bool geodesic = false);
  static double distance(Point &p1, Point &p2, bool geodesic = false);
  static double distance(Point &p1, double x2, double y2,
                         bool geodesic = false);
  // static double azimuth(double x1, double y1, double  x2, double y2);

 private:
  static double geodesic_distance(double x1, double y1, double x2, double y2);
  static double cartesian_distance(double x1, double y1, double x2, double y2);
};

#endif  // ADCMOD_CONSTANTS_H
