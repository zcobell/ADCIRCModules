#ifndef POINT_H
#define POINT_H

class Point {
public:
  Point();

  Point(double x, double y);

  double x() const;
  void setX(double x);

  double y() const;
  void setY(double y);

private:
  double _x;
  double _y;
};

#endif // POINT_H
