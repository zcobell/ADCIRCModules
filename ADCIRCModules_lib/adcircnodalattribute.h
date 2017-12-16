#ifndef ADCIRCNODALATTRIBUTE_H
#define ADCIRCNODALATTRIBUTE_H

#include "adcircnodalattributemetadata.h"
#include <vector>

class AdcircNodalAttribute {
public:
  explicit AdcircNodalAttribute();
  explicit AdcircNodalAttribute(AdcircNodalAttributeMetadata *metadata);

  AdcircNodalAttributeMetadata *metadata() const;

  void setMetadata(AdcircNodalAttributeMetadata *metadata);

  double value(int index) const;
  void setValues(const std::vector<double> &values);

private:
  /// Value(s) for nodal parameter at this node
  std::vector<double> m_values;

  /// Metadata for nodal parameters
  AdcircNodalAttributeMetadata *m_metadata;
};

#endif // ADCIRCNODALATTRIBUTE_H
