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
#ifndef ADCMOD_KDTREE_IMPL_H
#define ADCMOD_KDTREE_IMPL_H

#include <cstdlib>
#include <memory>
#include <vector>
#include "nanoflann.hpp"

class Kdtree_impl {
 public:
  Kdtree_impl();

  int build(std::vector<double> &x, std::vector<double> &y);
  bool initialized();

  size_t findNearest(double x, double y);
  std::vector<size_t> findXNearest(double x, double y, size_t n);
  std::vector<size_t> findWithinRadius(double x, double y, const double radius);

 private:
  bool m_initialized;

  template <typename T>
  struct PointCloud {
    struct Point {
      T x, y;
    };

    std::vector<Point> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate
    // value, the
    //  "if/else's" are actually solved at compile time.
    inline T kdtree_get_pt(const size_t idx, const size_t dim) const {
      if (dim == 0)
        return pts[idx].x;
      else
        return pts[idx].y;
    }

    // Optional bounding-box computation: return false to default to a standard
    // bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned
    //   in "bb" so it can be avoided to redo it again. Look at bb.size() to
    //   find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX & /* bb */) const {
      return false;
    }
  };

  // construct a kd-tree index:
  typedef nanoflann::KDTreeSingleIndexAdaptor<
      nanoflann::L2_Simple_Adaptor<double, PointCloud<double> >,
      PointCloud<double>, 2>
      kd_tree_t;

  PointCloud<double> m_cloud;
  std::unique_ptr<kd_tree_t> m_tree;
};

#endif  // ADCMOD_KDTREE_IMPL_H
