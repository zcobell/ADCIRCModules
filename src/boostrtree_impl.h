#ifndef BOOSTRTREE_IMPL_H
#define BOOSTRTREE_IMPL_H

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<double, 2, bg::cs::cartesian> point;
typedef std::pair<point, size_t> value;

class BoostRTree_impl {
 public:
  BoostRTree_impl();

  int build(std::vector<double> &x, std::vector<double> &y);
  bool initialized();

  size_t findNearest(double x, double y);
  std::vector<size_t> findXNearest(double x, double y, size_t n);

 private:
  bool m_initialized;
  bgi::rtree<value, bgi::quadratic<16>> m_tree;
};

#endif  // BOOSTRTREE_IMPL_H
