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
#include "constants.h"

double Constants::radiusEarth(double latitude) {
  double l = Constants::toRadians(latitude);
  return sqrt((pow(Constants::equitoralRadius(), 4.0) * cos(l) * cos(l) +
               pow(Constants::polarRadius(), 4.0) * sin(l) * sin(l)) /
              (pow(Constants::equitoralRadius(), 2.0) * cos(l) * cos(l) +
               pow(Constants::polarRadius(), 2.0) * sin(l) * sin(l)));
}

double Constants::distance(double x1, double y1, double x2, double y2,
                           bool geodesic) {
  if (geodesic) {
    return Constants::geodesic_distance(x1, y1, x2, y2);
  } else {
    return Constants::cartesian_distance(x1, y1, x2, y2);
  }
}

double Constants::geodesic_distance(double x1, double y1, double x2,
                                    double y2) {
  double lat1 = Constants::toRadians(y1);
  double lon1 = Constants::toRadians(x1);
  double lat2 = Constants::toRadians(y2);
  double lon2 = Constants::toRadians(x2);
  double r = Constants::radiusEarth((y1 + y2) / 2.0);
  return 2.0 * r *
         asin(sqrt(pow(sin((lat2 - lat1) / 2.0), 2.0) +
                   cos(lat1) * cos(lat2) * pow(sin((lon2 - lon1) / 2.0), 2.0)));
}

double Constants::cartesian_distance(double x1, double y1, double x2,
                                     double y2) {
  return sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
}

double Constants::toDegrees(double radians) {
  return radians * Constants::rad2deg();
}

double Constants::toRadians(double degrees) {
  return degrees * Constants::deg2rad();
}
