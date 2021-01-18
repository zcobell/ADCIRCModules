/*------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------*/
#ifndef ADCMOD_KDTREE_H
#define ADCMOD_KDTREE_H

#include <cstddef>
#include <memory>
#include <vector>

#include "AdcircModules_Global.h"

namespace Adcirc {

namespace Private {
// Forward declaration of pimpl class
class KdtreePrivate;
}  // namespace Private

/**
 * @class Kdtree
 * @author Zachary Cobell
 * @brief Class that handles neighbor searching using nanoflann
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 *
 */
class Kdtree {
 public:
  ADCIRCMODULES_EXPORT Kdtree();
  ADCIRCMODULES_EXPORT ~Kdtree();

  enum _errors { NoError, SizeMismatch };

  ADCIRCMODULES_EXPORT size_t size();
  ADCIRCMODULES_EXPORT int build(std::vector<double> &x,
                                 std::vector<double> &y);
  ADCIRCMODULES_EXPORT size_t findNearest(double x, double y);
  ADCIRCMODULES_EXPORT std::vector<size_t> findXNearest(double x, double y,
                                                        size_t n);
  ADCIRCMODULES_EXPORT std::vector<size_t> findWithinRadius(
      double x, double y, const double radius);
  ADCIRCMODULES_EXPORT bool initialized();

 private:
  std::unique_ptr<Adcirc::Private::KdtreePrivate> m_ptr;
};
}  // namespace Adcirc

#endif  // ADCMOD_KDTREE_H
