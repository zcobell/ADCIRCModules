#ifndef BOOSTRTREE_H
#define BOOSTRTREE_H

#include <cstddef>
#include <vector>

class BoostRTree_impl;

class BoostRTree {
 public:
  BoostRTree();
  ~BoostRTree();

  enum _errors { NoError, SizeMismatch };

  int build(std::vector<double> &x, std::vector<double> &y);
  size_t findNearest(double x, double y);
  std::vector<size_t> findXNearest(double x, double y, size_t n);
  bool initialized();

 private:
  BoostRTree_impl *m_ptr;
};

#endif  // BOOSTRTREE_H
