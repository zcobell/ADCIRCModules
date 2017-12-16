#ifndef ADCIRCNODALATTRIBUTEMETADATA_H
#define ADCIRCNODALATTRIBUTEMETADATA_H

#include <string>
#include <vector>

class AdcircNodalAttributeMetadata {
public:
  explicit AdcircNodalAttributeMetadata(std::string name = "defaultName",
                                std::string units = "none", int numValues = 1);

  std::string name() const;
  void setName(const std::string &name);

  std::string units() const;
  void setUnits(const std::string &units);

  int numValues() const;
  void setNumValues(int numValues);

  std::vector<double> getDefaultValue() const;
  void setDefaultValue(const std::vector<double> &value);

private:
  /// Name of the nodal attribute
  std::string m_name;

  /// Units for the nodal parameter
  std::string m_units;

  /// Number of values associated with each node for this nodal attribute
  int m_numValues;

  /// Default value(s) to be used when writing the fort.13 file
  std::vector<double> m_defaultValue;
};

#endif // ADCIRCNODALATTRIBUTEMETADATA_H
