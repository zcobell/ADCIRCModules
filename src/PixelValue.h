//
// Created by Zach Cobell on 1/16/21.
//

#ifndef ADCIRCMODULES_SRC_PIXELVALUE_H_
#define ADCIRCMODULES_SRC_PIXELVALUE_H_

#include <Point.h>

namespace Adcirc {

template <typename T>
class PixelValue {
 public:
  ADCIRCMODULES_EXPORT PixelValue()
      : m_location(0.0, 0.0), m_valid(false), m_value(0) {}
  ADCIRCMODULES_EXPORT PixelValue(Point t_location, bool t_valid, T t_value)
      : m_location(t_location), m_valid(t_valid), m_value(t_value) {}

  Point ADCIRCMODULES_EXPORT location() const { return m_location; }

  const Point ADCIRCMODULES_EXPORT &ref_location() const { return m_location; }

  void ADCIRCMODULES_EXPORT setLocation(const Point &t_location) {
    m_location = t_location;
  }

  bool ADCIRCMODULES_EXPORT valid() const { return m_valid; }

  void ADCIRCMODULES_EXPORT setValid(bool t_valid) { m_valid = t_valid; }

  T ADCIRCMODULES_EXPORT value() const { return m_value; }

  void ADCIRCMODULES_EXPORT setValue(T t_value) { m_value = t_value; }

 private:
  Point m_location;
  bool m_valid;
  T m_value;
};
}  // namespace Adcirc

#endif  // ADCIRCMODULES_SRC_PIXELVALUE_H_
