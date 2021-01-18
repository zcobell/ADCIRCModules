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
#include "Pixel.h"

#include <limits>

#include "DefaultValues.h"

using namespace Adcirc::Raster;

/**
 * @brief Default constructor
 *
 * Pixel is set as invalid by default
 */
Pixel::Pixel() { this->setInvalid(); }

/**
 * @brief Constructor that takes i,j index
 * @param i i-index
 * @param j j-index
 */
Pixel::Pixel(size_t i, size_t j) : m_i(i), m_j(j) {}

/**
 * @brief Sets the pixel indicies
 * @param i i-index
 * @param j j-index
 */
void Pixel::set(const size_t &i, const size_t &j) {
  this->m_i = i;
  this->m_j = j;
  return;
}

/**
 * @brief Returns the i-index
 * @return i-index
 */
size_t Pixel::i() const { return this->m_i; }

/**
 * @brief Returns the j-index
 * @return j-index
 */
size_t Pixel::j() const { return this->m_j; }

/**
 * @brief Returns a boolean describing if the pixel is valid
 * @return true if pixel is valid, false otherwise
 */
bool Pixel::isValid() {
  return this->m_i < adcircmodules_default_value<size_t>() &&
         this->m_j < adcircmodules_default_value<size_t>();
}

/**
 * @brief Sets a pixel to an invalid value for detection later
 */
void Pixel::setInvalid() {
  this->m_i = adcircmodules_default_value<size_t>();
  this->m_j = adcircmodules_default_value<size_t>();
}
