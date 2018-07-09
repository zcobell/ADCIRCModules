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
#include "point.h"

Point::Point() {
  this->m_x = -9999.0;
  this->m_y = -9999.0;
}

Point::Point(double x, double y) {
  this->setX(x);
  this->setY(y);
}

double Point::x() const { return this->m_x; }

void Point::setX(double x) { this->m_x = x; }

double Point::y() const { return this->m_y; }

void Point::setY(double y) { this->m_y = y; }
