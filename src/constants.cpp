//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "constants.h"
#include <cmath>

/**
 * @brief Returns the radius of Earth at a given latitude
 * @param[in] latitude location to generate a radius for
 * @return radius of Earth at given latitude
 */
double Constants::radiusEarth(double latitude) {
  double l = Constants::toRadians(latitude);
  return std::sqrt(
      (std::pow(Constants::equitoralRadius(), 4.0) * std::cos(l) * std::cos(l) +
       std::pow(Constants::polarRadius(), 4.0) * std::sin(l) * std::sin(l)) /
      (std::pow(Constants::equitoralRadius(), 2.0) * std::cos(l) * std::cos(l) +
       std::pow(Constants::polarRadius(), 2.0) * std::sin(l) * std::sin(l)));
}

/**
 * @brief Calculates the distance between two points
 * @param[in] x1 x-location 1
 * @param[in] y1 y-location 1
 * @param[in] x2 x-location 2
 * @param[in] y2 y-location 2
 * @param[in] geodesic if true, use the Haversine formula to calculate a geodesic
 * distance, otherwise use a simple cartesian distance (default)
 * @return distance between two points
 */
double Constants::distance(double x1, double y1, double x2, double y2,
                           bool geodesic) {
  if (geodesic) {
    return Constants::geodesic_distance(x1, y1, x2, y2);
  } else {
    return Constants::cartesian_distance(x1, y1, x2, y2);
  }
}

/**
 * @brief Calculates the distance between two points
 * @param[in] p1 location 1
 * @param[in] p2 location 2
 * @param[in] geodesic if true, use the Haversine formula to calculate a geodesic
 * distance, otherwise use a simple cartesian distance (default)
 * @return distance between two points
 */
double Constants::distance(Point &p1, Point &p2, bool geodesic) {
  if (geodesic) {
    return Constants::geodesic_distance(p1.first, p1.second, p2.first,
                                        p2.second);
  } else {
    return Constants::cartesian_distance(p1.first, p1.second, p2.first,
                                         p2.second);
  }
}

/**
 * @brief Calculates the distance between two points
 * @param[in] p1 location 1
 * @param[in] x2 x-location 2
 * @param[in] y2 y-location 2
 * @param[in] geodesic if true, use the Haversine formula to calculate a geodesic
 * distance, otherwise use a simple cartesian distance (default)
 * @return distance between two points
 */
double Constants::distance(Point &p1, double x2, double y2, bool geodesic) {
  if (geodesic) {
    return Constants::geodesic_distance(p1.first, p1.second, x2, y2);
  } else {
    return Constants::cartesian_distance(p1.first, p1.second, x2, y2);
  }
}

/**
 * @brief Calculates the distance on a sphere using the Haversine formula
 * @param[in] x1 x-location 1
 * @param[in] y1 y-location 1
 * @param[in] x2 x-location 2
 * @param[in] y2 y-location 2
 * @return distance on sphere in meters
 */
double Constants::geodesic_distance(double x1, double y1, double x2,
                                    double y2) {
  double lat1 = Constants::toRadians(y1);
  double lon1 = Constants::toRadians(x1);
  double lat2 = Constants::toRadians(y2);
  double lon2 = Constants::toRadians(x2);
  double r = Constants::radiusEarth((y1 + y2) / 2.0);
  return 2.0 * r *
         std::asin(std::sqrt(std::pow(std::sin((lat2 - lat1) / 2.0), 2.0) +
                             std::cos(lat1) * std::cos(lat2) *
                                 std::pow(std::sin((lon2 - lon1) / 2.0), 2.0)));
}

/**
 * @brief Calculates a simple cartesian distance where the output is in the same
 * units as the input
 * @param[in] x1 x-location 1
 * @param[in] y1 y-location 1
 * @param[in] x2 x-location 2
 * @param[in] y2 y-location 2
 * @return cartesian distance between two points
 */
double Constants::cartesian_distance(double x1, double y1, double x2,
                                     double y2) {
  return std::sqrt(std::pow(x2 - x1, 2.0) + std::pow(y2 - y1, 2.0));
}

/**
 * @brief Converts a value from radians to degrees
 * @param[in] radians value in radians
 * @return value in degrees
 */
double Constants::toDegrees(double radians) {
  return radians * Constants::rad2deg();
}

/**
 * @brief Converts a value from degrees to radians
 * @param[in] degrees value in degrees
 * @return value in radians
 */
double Constants::toRadians(double degrees) {
  return degrees * Constants::deg2rad();
}
