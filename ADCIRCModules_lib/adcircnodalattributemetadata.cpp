#include "adcircnodalattributemetadata.h"

AdcircNodalAttributeMetadata::AdcircNodalAttributeMetadata(std::string name, std::string units,
                                           int numValues) {
  this->setName(name);
  this->setUnits(units);
  this->setNumValues(numValues);
}

std::string AdcircNodalAttributeMetadata::name() const { return this->m_name; }

void AdcircNodalAttributeMetadata::setName(const std::string &name) {
  this->m_name = name;
}

std::string AdcircNodalAttributeMetadata::units() const { return this->m_units; }

void AdcircNodalAttributeMetadata::setUnits(const std::string &units) {
  this->m_units = units;
}

int AdcircNodalAttributeMetadata::numValues() const { return this->m_numValues; }

void AdcircNodalAttributeMetadata::setNumValues(int numValues) {
  this->m_numValues = numValues;
}

std::vector<double> AdcircNodalAttributeMetadata::getDefaultValue() const {
  return this->m_defaultValue;
}

void AdcircNodalAttributeMetadata::setDefaultValue(const std::vector<double> &value) {
  this->m_defaultValue = value;
}
