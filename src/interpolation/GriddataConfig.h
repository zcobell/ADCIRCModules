//
// Created by Zach Cobell on 1/17/21.
//

#ifndef ADCIRCMODULES_SRC_GRIDDATACONFIG_H_
#define ADCIRCMODULES_SRC_GRIDDATACONFIG_H_

#include <iostream>
#include <vector>

#include "InterpolationMethods.h"

namespace Adcirc {
namespace Private {

class GriddataConfig {
 public:
  GriddataConfig(bool useLookup,
                 Adcirc::Interpolation::Threshold thresholdMethod,
                 double thresholdValue, double datumShift,
                 double rasterMultiplier, double defaultValue,
                 std::vector<double> lookupTable)
      : m_useLookup(useLookup),
        m_thresholdMethod(thresholdMethod),
        m_thresholdValue(thresholdValue),
        m_datumShift(datumShift),
        m_rasterMultiplier(rasterMultiplier),
        m_defaultValue(defaultValue),
        m_lookup(std::move(lookupTable)) {}

  bool useLookup() const { return m_useLookup; }
  void setUseLookup(bool v) { m_useLookup = v; }

  Adcirc::Interpolation::Threshold thresholdMethod() const {
    return m_thresholdMethod;
  }
  void setThresholdMethod(Adcirc::Interpolation::Threshold m) {
    m_thresholdMethod = m;
  }

  double thresholdValue() const { return m_thresholdValue; }
  void setThresholdValue(double v) { m_thresholdValue = v; }

  double datumShift() const { return m_datumShift; }
  void setDatumShift(double v) { m_datumShift = v; }

  double rasterMultiplier() const { return m_rasterMultiplier; }
  void setRasterMultiplier(double v) { m_rasterMultiplier = v; }

  double defaultValue() const { return m_defaultValue; }
  void setDefaultValue(double v) { m_defaultValue = v; }

  std::vector<double> lookupTable() { return m_lookup; }
  void setLookupTable(const std::vector<double> &v) { m_lookup = v; }

  double getKeyValue(unsigned key) const {
    assert(key < m_lookup.size());
    return m_lookup[key];
  }

 private:
  bool m_useLookup;
  Adcirc::Interpolation::Threshold m_thresholdMethod;
  double m_thresholdValue;
  double m_datumShift;
  double m_rasterMultiplier;
  double m_defaultValue;
  std::vector<double> m_lookup;
};

}  // namespace Private
}  // namespace Adcirc

#endif  // ADCIRCMODULES_SRC_GRIDDATACONFIG_H_
