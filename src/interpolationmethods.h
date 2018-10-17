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
#ifndef INTERPOLATIONMETHODS_H
#define INTERPOLATIONMETHODS_H

namespace Interpolation {
enum Method {
  /// No interpolation
  NoMethod = 0,
  /// Mesh sized based averaging defined by the mesh size and any multipliers
  Average = 1,
  /// Nearest node interpolation within specified radius defined by the mesh
  /// size and any multipliers
  Nearest = 2,
  /// Greatest value within specified radius
  Highest = 3,
  /// Calculates the average of values > 2 sigma from the mean. Used to
  /// interpolate raised features withinout overstating
  PlusTwoSigma = 4,
  /// Uses the Bilskie et al radius sizing. Generally 1/4 the average element
  /// size or the nearest point for coarse regions
  BilskieEtAll = 5,
  /// Uses an inverse distance weighting approach for the points available
  /// within the radius defined by the mesh size and any multipliers
  InverseDistanceWeighted = 6
};
}

#endif  // INTERPOLATIONMETHODS_H
