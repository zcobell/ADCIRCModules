//
// Created by Zach Cobell on 1/16/21.
//

#ifndef ADCIRCMODULES_SRC_POINT_H_
#define ADCIRCMODULES_SRC_POINT_H_

#include "AdcircModules_Global.h"

namespace Adcirc {
class Point {
 public:
  ADCIRCMODULES_EXPORT Point() : m_x(0.0), m_y(0.0) {}
  ADCIRCMODULES_EXPORT Point(double x, double y) : m_x(x), m_y(y) {}

  void ADCIRCMODULES_EXPORT set(double x, double y) {
    m_x = x;
    m_y = y;
  }

  void ADCIRCMODULES_EXPORT setX(double x) { m_x = x; }
  double ADCIRCMODULES_EXPORT x() const { return m_x; }

  void ADCIRCMODULES_EXPORT setY(double y) { m_y = y; }
  double ADCIRCMODULES_EXPORT y() const { return m_y; }

 private:
  double m_x;
  double m_y;
};
}  // namespace Adcirc

#endif  // ADCIRCMODULES_SRC_POINT_H_
