#ifndef ADCIRCNODE_H
#define ADCIRCNODE_H

#include "adcircmodules_global.h"
#include <string>

class AdcircNode {

public:
  explicit AdcircNode();
  explicit AdcircNode(int id, double x, double y, double z);

  void setNode(int id, double x, double y, double z);

  double x() const;
  void setX(double x);

  double y() const;
  void setY(double y);

  double z() const;
  void setZ(double z);

  int id() const;
  void setId(int id);

  std::string toString(bool geographicCoordinates);

private:
  int m_id;
  double m_x;
  double m_y;
  double m_z;
};

#endif // ADCIRCNODE_H
