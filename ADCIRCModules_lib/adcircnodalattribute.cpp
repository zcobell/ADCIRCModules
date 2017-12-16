#include "adcircnodalattribute.h"

AdcircNodalAttribute::AdcircNodalAttribute() {}

AdcircNodalAttributeMetadata *AdcircNodalAttribute::metadata() const {
  return this->m_metadata;
}

void AdcircNodalAttribute::setMetadata(AdcircNodalAttributeMetadata *metadata) {
  m_metadata = metadata;
}

double AdcircNodalAttribute::value(int index) const {
  if (index >= 0 && index < this->m_values.size())
    return m_values[index];
  else
    return -9999.0;
}

void AdcircNodalAttribute::setValues(const std::vector<double> &values) {
  this->m_values = values;
}
