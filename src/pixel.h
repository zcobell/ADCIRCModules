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
#ifndef ADCMOD_PIXEL_H
#define ADCMOD_PIXEL_H

#include <cstddef>
#include "adcircmodules_global.h"

namespace Adcirc {
namespace Raster {

/**
 * @class Pixel
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief Describes a single pixel i,j used when working with raster data
 *
 */
class Pixel {
 public:
  ADCIRCMODULES_EXPORT Pixel();
  ADCIRCMODULES_EXPORT Pixel(size_t i, size_t j);

  ADCIRCMODULES_EXPORT size_t i() const;
  ADCIRCMODULES_EXPORT size_t j() const;

  ADCIRCMODULES_EXPORT bool isValid();
  ADCIRCMODULES_EXPORT void setInvalid();

  ADCIRCMODULES_EXPORT void set(const size_t &i, const size_t &j);

 private:
  size_t m_i;
  size_t m_j;
};
}  // namespace Raster
}  // namespace Adcirc

#endif  // ADCMOD_PIXEL_H
