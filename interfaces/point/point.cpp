#include "point.h"

Point::Point(){}

Point::Point(double x, double y) {
  this->setX(x);
  this->setY(y);
}

double Point::x() const { return _x; }

void Point::setX(double x) { _x = x; }

double Point::y() const { return _y; }

void Point::setY(double y) { _y = y; }
