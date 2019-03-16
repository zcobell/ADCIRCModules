#include "boostrtree.h"
#include "boostrtree_impl.h"

BoostRTree::BoostRTree() { this->m_ptr = new BoostRTree_impl(); }

BoostRTree::~BoostRTree() { delete this->m_ptr; }

int BoostRTree::build(std::vector<double> &x, std::vector<double> &y) {
  int ierr = this->m_ptr->build(x, y);
  if (ierr != 0) return BoostRTree::SizeMismatch;
  return BoostRTree::NoError;
}

size_t BoostRTree::findNearest(double x, double y) {
  return this->m_ptr->findNearest(x, y);
}

std::vector<size_t> BoostRTree::findXNearest(double x, double y, size_t n) {
  return this->m_ptr->findXNearest(x, y, n);
}

bool BoostRTree::initialized() { return this->m_ptr->initialized(); }
