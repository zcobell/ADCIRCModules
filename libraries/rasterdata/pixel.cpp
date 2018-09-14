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
#include "pixel.h"
#include <limits>

Pixel::Pixel() { this->setInvalid(); }

Pixel::Pixel(size_t i, size_t j) {
  this->m_i = i;
  this->m_j = j;
  return;
}

void Pixel::set(const size_t &i, const size_t &j) {
  this->m_i = i;
  this->m_j = j;
  return;
}

size_t Pixel::i() const { return this->m_i; }

size_t Pixel::j() const { return this->m_j; }

bool Pixel::isValid() {
  return this->m_i < std::numeric_limits<size_t>::max() &&
         this->m_j < std::numeric_limits<size_t>::max();
}

void Pixel::setInvalid() {
  this->m_i = std::numeric_limits<size_t>::max();
  this->m_j = this->m_i;
}
