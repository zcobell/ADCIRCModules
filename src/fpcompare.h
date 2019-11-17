#ifndef ADCMOD_FPCOMPARE_H
#define ADCMOD_FPCOMPARE_H

#include <vector>
#include "boost/math/special_functions/relative_difference.hpp"

namespace FpCompare {

template <typename T>
bool ADCIRCMODULES_EXPORT equalTo(const T &a, const T &b) {
  return !(boost::math::relative_difference(a, b) > 0);
}

template <typename T>
bool equalTo(const std::vector<T> &a, const std::vector<T> &b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (!equalTo(a[i], b[i])) return false;
  }
  return true;
}

};  // namespace FpCompare

#endif  // ADCMOD_FPCOMPARE_H
