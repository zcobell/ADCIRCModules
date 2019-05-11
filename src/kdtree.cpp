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
#include "kdtree.h"
#include "kdtree_impl.h"

/**
 * @brief Default constructor for a new Kdtree object
 */
Kdtree::Kdtree() : m_ptr(new Kdtree_impl()) {}

/**
 * @brief Builds the Kdtree using an x and y vector of doubles
 * @param x vector of doubles for x-position
 * @param y vector of doubles for y-position
 * @return error code
 */
int Kdtree::build(std::vector<double> &x, std::vector<double> &y) {
  int ierr = this->m_ptr->build(x, y);
  if (ierr != 0) return Kdtree::SizeMismatch;
  return Kdtree::NoError;
}

/**
 * @brief Finds the nearest position in the x, y 2d pointcloud
 * @param x x-location for search
 * @param y y-location for search
 * @return index in x,y array
 */
size_t Kdtree::findNearest(double x, double y) {
  return this->m_ptr->findNearest(x, y);
}

/**
 * @brief Finds the nearest 'x' number of locations, sorted
 * @param x x-location for search
 * @param y y-location for search
 * @param n number of points to return
 * @return vector of indicies in the x,y array
 */
std::vector<size_t> Kdtree::findXNearest(double x, double y, size_t n) {
  return this->m_ptr->findXNearest(x, y, n);
}

/**
 * @brief Checks if the Kdtree has been initialized
 * @return true if the Kdtree has been initialized
 */
bool Kdtree::initialized() { return this->m_ptr->initialized(); }

/**
 * @brief Finds all points within a given radius
 * @param x x-location for search
 * @param y y-location for search
 * @param radius search radius in native coordinates
 * @return vector with indicies of found points
 */
std::vector<size_t> Kdtree::findWithinRadius(double x, double y,
                                             const double radius) {
  return this->m_ptr->findWithinRadius(x, y, radius);
}
