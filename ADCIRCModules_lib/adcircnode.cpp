#include "adcircnode.h"
#include "boost/format.hpp"

AdcircNode::AdcircNode() {}

AdcircNode::AdcircNode(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
}

void AdcircNode::setNode(int id, double x, double y, double z) {
  this->m_id = id;
  this->m_x = x;
  this->m_y = y;
  this->m_z = z;
  return;
}

double AdcircNode::x() const { return this->m_x; }

void AdcircNode::setX(double x) { this->m_x = x; }

double AdcircNode::y() const { return this->m_y; }

void AdcircNode::setY(double y) { this->m_y = y; }

double AdcircNode::z() const { return this->m_z; }

void AdcircNode::setZ(double z) { this->m_z = z; }

int AdcircNode::id() const { return this->m_id; }

void AdcircNode::setId(int id) { this->m_id = id; }

std::string AdcircNode::toString(bool geographicCoordinates) {
  if (geographicCoordinates)
    return boost::str(boost::format("%11i   %14.10f   %14.10f  %14.10f") %
                      this->id() % this->x() % this->y() % this->z());
  else
    return boost::str(boost::format("%11i   %14.4f   %14.4f  %14.4f") %
                      this->id() % this->x() % this->y() % this->z());
}
