/*-------------------------------GPL-------------------------------------//
//
// adcircmodules - A library for working with ADCIRC models
// Copyright (C) 2016  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------*/
#include "kdtree2lib.h"
#include "boost/array.hpp"
#include "boost/multi_array.hpp"
#include "kdtree2.hpp"
#include "point.h"

/** \brief Constructor function for QKdtree2 class
 *
 * @param[in] parent Parent QObject used for memory management
 *
 * Constructor function for QKdtree2 class
 *
 */
QKdtree2::QKdtree2() {
  this->m_initialized = false;
  this->m_numDataPoints = 0;
  this->m_tree.reset(nullptr);
}

QKdtree2::~QKdtree2() {}

/**
 * \brief Generates a kdtree2 search tree
 *
 * @param[in] pointCloud Vector of Point used to generate the search tree
 *
 * This function uses the kdtree2 library to generate a search tree
 *
 **/
int QKdtree2::build(std::vector<Point> &pointCloud) {
  int i;
  typedef boost::multi_array<float, 2> array2d;

  this->m_numDataPoints = pointCloud.size();

  array2d data(boost::extents[pointCloud.size()][2]);

  for (i = 0; i < pointCloud.size(); i++) {
    data[i][0] = static_cast<float>(pointCloud[i].x());
    data[i][1] = static_cast<float>(pointCloud[i].y());
  }

  this->m_tree = std::unique_ptr<kdtree2>(new kdtree2(data, true));
  this->m_initialized = true;

  return NoError;
}

/**
 * \brief Generates a kdtree2 search tree
 *
 * @param[in] x Vector of x-coordinates
 * @param[in] y Vector of y-coordinates
 *
 * This function uses the kdtree2 library to generate a search tree
 *
 **/
int QKdtree2::build(std::vector<double> &x, std::vector<double> &y) {
  int i;
  typedef boost::multi_array<float, 2> array2d;

  if (x.size() != y.size()) {
    return SizeMismatch;
  }

  this->m_numDataPoints = x.size();

  array2d data(boost::extents[x.size()][2]);

  for (i = 0; i < x.size(); i++) {
    data[i][0] = static_cast<float>(x[i]);
    data[i][1] = static_cast<float>(y[i]);
  }

  this->m_tree = std::unique_ptr<kdtree2>(new kdtree2(data, true));
  this->m_initialized = true;

  return NoError;
}

/**
 * \brief Function that uses a kd-tree to find the nearest point to a given set
 *of coordinates
 *
 * @param[in]  pointLocation  location of point to search for
 * @param[out] index          location in array that created kd-tree of nearest
 *point
 *
 * Function that uses a kd-tree to find the nearest point to a given set of
 *coordinates
 *
 **/
size_t QKdtree2::findNearest(Point pointLocation) {
  kdtree2_result_vector result_vector;
  kdtree2_result result;
  std::vector<float> query(2);

  query[0] = static_cast<float>(pointLocation.x());
  query[1] = static_cast<float>(pointLocation.y());

  this->m_tree->n_nearest(query, 1, result_vector);

  result = result_vector.at(0);
  return result.idx;
}

/**
 * \brief Function that uses a kd-tree to find the nearest point to a given set
 *of coordinates
 *
 * @param[in]  x      x-coordinate of point to search for
 * @param[in]  y      y-coordinate of point to search for
 * @param[out] index  location in array that created kd-tree of nearest point
 *
 * Function that uses a kd-tree to find the nearest point to a given set of
 *coordinates
 *
 **/
size_t QKdtree2::findNearest(double x, double y) {
  return this->findNearest(Point(x, y));
}

/**
 * \brief Function that uses a kd-tree to find the N-nearest point to a given
 *set of coordinates
 *
 * @param[in]  pointLocation  location of point to search for
 * @param[in]  nn             number of locations to find
 * @param[out] indicies       vector of locations in array that created kd-tree
 *of nearest points
 *
 * Function that uses a kd-tree to find the N-nearest point to a given set of
 *coordinates
 *
 **/
std::vector<size_t> QKdtree2::findXNearest(Point pointLocation, int nn) {
  int i;
  kdtree2_result_vector result_vector;
  kdtree2_result result;
  std::vector<float> query(2);
  std::vector<size_t> indicies;

  if (nn > this->m_numDataPoints) {
    nn = static_cast<int>(this->m_numDataPoints);
  }

  query[0] = static_cast<float>(pointLocation.x());
  query[1] = static_cast<float>(pointLocation.y());

  this->m_tree->n_nearest(query, nn, result_vector);

  indicies.resize(nn);

  for (i = 0; i < nn; i++) {
    result = result_vector.at(i);
    indicies[i] = result.idx;
  }

  return indicies;
}

/**
 * \brief Function that uses a kd-tree to find the N-nearest point to a given
 *set of coordinates
 *
 * @param[in]  x              x-coordinate of point to search for
 * @param[in]  y              y-coordinate of point to search for
 * @param[in]  nn             number of locations to find
 * @param[out] indicies       vector of locations in array that created kd-tree
 *of nearest points
 *
 * Function that uses a kd-tree to find the N-nearest point to a given set of
 *coordinates
 *
 **/
std::vector<size_t> QKdtree2::findXNearest(double x, double y, int nn) {
  return this->findXNearest(Point(x, y), nn);
}

size_t QKdtree2::size() const { return this->m_numDataPoints; }

bool QKdtree2::isInitialized() const { return this->m_initialized; }
