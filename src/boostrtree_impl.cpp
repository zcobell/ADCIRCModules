#include "boostrtree_impl.h"
#include "boost/function_output_iterator.hpp"

// used with Boost.Geometry R-tree
struct SearchCallback {
  template <typename Value>
  void operator()(Value const &v) {
    this->m_vec->push_back(v.second);
  }
  void setPtr(std::vector<size_t> *vec) { this->m_vec = vec; }
  std::vector<size_t> *m_vec;
};

BoostRTree_impl::BoostRTree_impl() : m_initialized(false) {}

bool BoostRTree_impl::initialized() { return this->m_initialized; }

int BoostRTree_impl::build(std::vector<double> &x, std::vector<double> &y) {
  if (x.size() != y.size()) return 1;
  for (size_t i = 0; i < x.size(); ++i) {
    point p(x[i], y[i]);
    this->m_tree.insert(std::make_pair(p, i));
  }
  this->m_initialized = true;
  return 0;
}

size_t BoostRTree_impl::findNearest(double x, double y) {
  return this->findXNearest(x, y, 1)[0];
}

std::vector<size_t> BoostRTree_impl::findXNearest(double x, double y,
                                                  size_t n) {
  SearchCallback callback;
  std::vector<size_t> result;
  result.reserve(n);
  callback.setPtr(&result);
  this->m_tree.query(bgi::nearest(point(x, y), n),
                     boost::make_function_output_iterator(callback));
  return result;
}
