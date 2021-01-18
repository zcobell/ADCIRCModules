//
// Created by Zach Cobell on 1/16/21.
//

#ifndef ADCIRCMODULES_SRC_PIXELVALUEVECTOR_H_
#define ADCIRCMODULES_SRC_PIXELVALUEVECTOR_H_

#include <vector>

#include "PixelValue.h"

namespace Adcirc {

template <typename T>
class PixelValueVector {
 public:
  ADCIRCMODULES_EXPORT PixelValueVector() : m_code(1) {}

  int ADCIRCMODULES_EXPORT code() const { return m_code; }

  void ADCIRCMODULES_EXPORT setCode(int t_code) { m_code = t_code; }

  void ADCIRCMODULES_EXPORT reserve(size_t size) { m_pixels.reserve(size); }

  void ADCIRCMODULES_EXPORT push_back(PixelValue<T> t_pixel) {
    m_pixels.push_back(t_pixel);
  }

  void ADCIRCMODULES_EXPORT erase(size_t index) {
    assert(index < m_pixels.size());
    m_pixels.erase(m_pixels.begin() + index);
  }

  PixelValue<T> ADCIRCMODULES_EXPORT &operator[](size_t n) {
    return m_pixels[n];
  }

  const PixelValue<T> ADCIRCMODULES_EXPORT &operator[](size_t n) const {
    return m_pixels[n];
  }

  std::vector<PixelValue<T>> ADCIRCMODULES_EXPORT &pixels() { return m_pixels; }

  const std::vector<PixelValue<T>> ADCIRCMODULES_EXPORT &pixels() const {
    return m_pixels;
  }

 private:
  int m_code;
  std::vector<PixelValue<T>> m_pixels;
};

}  // namespace Adcirc
#endif  // ADCIRCMODULES_SRC_PIXELVALUEVECTOR_H_
