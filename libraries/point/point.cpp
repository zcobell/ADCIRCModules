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
#include <limits>

/**
 * @brief Default constructor. Point is initialized to double min.
 */
Point::Point() {
  this->m_x = std::numeric_limits<double>::min();
  this->m_y = std::numeric_limits<double>::min();
}

/**
 * @brief Constructor that assigns the location
 * @param x x-location
 * @param y y-location
 */
Point::Point(double x, double y) : m_x(x), m_y(y) {}

/**
 * @brief Returns x-location
 * @return x-location
 */
double Point::x() const { return this->m_x; }

/**
 * @brief Sets the x-location
 * @param x new x-location
 */
void Point::setX(double x) { this->m_x = x; }

/**
 * @brief Returns y-location
 * @return y-location
 */
double Point::y() const { return this->m_y; }

/**
 * @brief Sets the y-location
 * @param y new y-location
 */
void Point::setY(double y) { this->m_y = y; }

/**
 * @brief Sets the x and y location of a point
 * @param x x-location
 * @param y y-location
 */
void Point::set(double x, double y) {
  this->m_x = x;
  this->m_y = y;
}
