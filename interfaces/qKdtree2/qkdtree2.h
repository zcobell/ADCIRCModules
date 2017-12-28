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
/**
 * \class QKdtree2
 *
 * \brief Class that serves as an interface to the kdtree2 library
 *
 * This function serves as the C++ interface to the kdtree2 library. Kdtree2
 * requires the boost library headers that are included with this code. This
 * implementatin
 * currently only supports 2-dimensional searches, though the kd-tree library
 * this interfaces with does not have such a limitation
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef QKDTREE2_H
#define QKDTREE2_H

#include "point.h"
#include <cstddef>
#include <vector>

class kdtree2;

class QKdtree2 {

public:
  enum _errors { NoError, SizeMismatch };

  explicit QKdtree2();

  ~QKdtree2();

  int build(std::vector<Point> &pointCloud);
  int build(std::vector<double> &x, std::vector<double> &y);

  size_t findNearest(Point pointLocation);
  size_t findNearest(double x, double y);

  std::vector<int> findXNearest(Point pointLocation, int nn);
  std::vector<int> findXNearest(double x, double y, int nn);

  size_t size() const;

  bool isInitialized() const;

private:
  /// Variable holding the total number of points in the search tree
  size_t m_numDataPoints;

  /// Variable that ensures the search tree is initialized
  bool m_initialized;

  /// Pointer to variable holding the kdtree search tree
  kdtree2 *m_tree;
};

#endif // QKDTREE2_H
